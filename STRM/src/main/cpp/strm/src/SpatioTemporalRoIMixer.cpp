#include "strm/SpatioTemporalRoIMixer.hpp"

#include <memory>
#include <utility>

#include "strm/Interpolator.hpp"

namespace rm {

FrameBuffer::FrameBuffer(const std::string& key, int capacity)
    : key(key), shortKey(key.substr(key.size() - 5, 1)), capacity(capacity), count(0) {
  frames.resize(capacity);
}

Frame* FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this, frameIndex]() { return frames[frameIndex % capacity].get() == nullptr; });
  Frame* prevFrame = frameIndex == 0 ? nullptr : frames[(frameIndex - 1) % capacity].get();
  frames[frameIndex % capacity] = std::make_unique<Frame>(key, frameIndex, mat, prevFrame,
                                                          NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[frameIndex % capacity].get();
  }
  Frame* currFrame = frames[frameIndex % capacity].get();
  lock.unlock();
  LOGD("FrameBuffer%s::enqueue  (%d)", shortKey.c_str(), frameIndex);
  return currFrame;
}

void FrameBuffer::freeImage(const std::vector<int>& frameIndices, Logger* logger) {
  std::unique_lock<std::mutex> lock(mtx);
  for (int frameIndex : frameIndices) {
    if (logger != nullptr) {
      logger->log(frames[frameIndex % capacity].get());
    }
    frames[frameIndex % capacity].reset();
  }
  lock.unlock();
  cv.notify_all();
  LOGD("FrameBuffer%s::freeImage(%lu)", shortKey.c_str(), frameIndices.size());
}

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               InferenceEngine* inferenceEngine,
                                               int numSourceVideo,
                                               JavaVM* vm, JNIEnv* env, jobject strm, bool draw)
    : mConfig(config), mbStop(false),
      mLogger(new Logger("/data/data/hcs.offloading.strm/execution_log.csv")),
      mResultLogger(new Logger("/data/data/hcs.offloading.strm/test.log")),
      mInferenceEngine(inferenceEngine),
      mNumSourceVideos(numSourceVideo),
      mRoIExtractor(new RoIExtractor(config.roIExtractorConfig,
                                     inferenceEngine->getInputSizes()[0])),
      mRoIResizer(new RoIResizer(config.roIResizerConfig)),
      mPatchReconstructor(new PatchReconstructor(config.patchReconstructorConfig, mRoIResizer.get())),
      jvm(vm), env(nullptr), strm(reinterpret_cast<jobject>(env->NewGlobalRef(strm))), draw(draw),
      mProbing(config.roIResizerConfig.PROBING) {
  mLogger->logHeader();
  mThread = std::thread([this]() { work(); });
  mResultThread = std::thread([this]() { outputWork(); });

  class_SpatioTemporalRoIMixer = reinterpret_cast<jclass>(env->NewGlobalRef(
      env->FindClass("hcs/offloading/strm/Emulator")));
  SpatioTemporalRoIMixer_drawObjectDetectionResult = env->GetMethodID(
      class_SpatioTemporalRoIMixer, "drawObjectDetectionResult", "(JLjava/util/List;)V");
  class_ArrayList = reinterpret_cast<jclass>(env->NewGlobalRef(
      env->FindClass("java/util/ArrayList")));
  ArrayList_init = env->GetMethodID(class_ArrayList, "<init>", "()V");
  ArrayList_add = env->GetMethodID(class_ArrayList, "add", "(ILjava/lang/Object;)V");
  class_BoundingBox = reinterpret_cast<jclass>(env->NewGlobalRef(
      env->FindClass("hcs/offloading/strm/BoundingBox")));
  BoundingBox_init = env->GetMethodID(class_BoundingBox, "<init>", "(IIIIIFIIZ)V");
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mResultsCv.notify_all();
  mThread.join();
  mResultThread.join();
}

void SpatioTemporalRoIMixer::work() {
  int scheduleID = 1;
  int fullFrameInferenceStreamIndex = 0;
  time_us scheduleInterval = mConfig.LATENCY_SLO_MS * 1000 / 2;
  time_us startTime, roiGettingTime, fullFrameInferenceTime, mixingPreparationTime, mixedFrameInferenceTime;
  time_us remainingTime;

  std::unique_lock<std::mutex> startLock(mStartMtx);
  mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mNumSourceVideos; });
  mStartMtx.unlock();
  std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval));

  while (!mbStop) {
    LOGD("========== Schedule %d Start ==========", scheduleID);
    startTime = NowMicros();
    std::map<std::string, SortedFrames> frames = mRoIExtractor->getExtractedFrames();
    roiGettingTime = NowMicros();

    std::stringstream ss;
    for (const auto& it : frames) {
      ss << it.first.substr(it.first.size() - 5, 1) << ": ";
      if (!it.second.empty()) {
        ss << (*it.second.begin())->frameIndex << " ~ " << (*it.second.rbegin())->frameIndex
           << ", ";
      }
    }
    LOGD("STRM::work() getExtractedFrames() took %lu us  // %s",
         roiGettingTime - startTime, ss.str().c_str());

    if (frames.empty()) {
      if (scheduleInterval > roiGettingTime - startTime) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(scheduleInterval - (roiGettingTime - startTime)));
      }
      LOGD("========== Schedule %d End   ==========", scheduleID++);
      continue;
    }

    // Full Frame Inference
    Frame* fullFrameTarget = nullptr;
    if (scheduleID % mConfig.FULL_FRAME_INTERVAL == 0) {
      fullFrameTarget = getFullFrameInferenceFrame(frames, fullFrameInferenceStreamIndex++);
      LOGD("STRM::work() fullFrameTarget: (%s, %d)",
           fullFrameTarget->shortKey.c_str(), fullFrameTarget->frameIndex);
      fullFrameInference(fullFrameTarget);
    }
    fullFrameInferenceTime = NowMicros();

    // Prepare packing. resize and merge RoIs
    PatchMixer::preparePack(frames, mRoIResizer.get(), mInferenceEngine->getInputSizes()[0],
                            mConfig.MERGE_THRESHOLD);
    mixingPreparationTime = NowMicros();

    // Pack RoIs into mixed frames
    // TODO : handle numMixedFrames <= 0 case
    remainingTime = scheduleInterval < (mixingPreparationTime - startTime) ? 0 :
                    scheduleInterval - (mixingPreparationTime - startTime);
    long long inferenceTimeUs = mInferenceEngine->getInferenceTimeMs() * 1000;
    int numMixedFrames = remainingTime > inferenceTimeUs ?
                         (int) (remainingTime / inferenceTimeUs) : 1;
    LOGD("STRM::work() remain %lu us | inference %lu us | %lld mixedFrames",
         mixingPreparationTime - roiGettingTime, remainingTime, inferenceTimeUs);
    std::vector<MixedFrame> mixedFrames = PatchMixer::pack(frames, fullFrameTarget,
                                                           mInferenceEngine->getInputSizes()[0],
                                                           numMixedFrames, mProbing,
                                                           mRoIResizer->getProbingStep());

    // Inference Mixed Frames
    std::vector<int> handles;
    std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                   [this](const MixedFrame& mixedFrame) {
                     return mInferenceEngine->enqueue(mixedFrame.packedMat);
                   });

    // Get Mixed Frame results sequentially
    SortedFrames matchedFrames;
    for (int i = 0; i < mixedFrames.size(); i++) {
      std::vector<BoundingBox> results = mInferenceEngine->getResults(handles[i]);
      mixedFrames[i].packedMat.release();
      mPatchReconstructor->assignBoxesToFrame(mixedFrames[i], results);

      // Notify results of processed frames
      for (Frame* frame : mixedFrames[i].getPackedFrames()) {
        if (frame->isReadyToMarry(i)) {
          // Match boxes with RoIs (per frame)
          nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
          mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
          matchedFrames.insert(frame);

          bool allMarried = true;
          for (auto& cRoI : frame->childRoIs) {
            allMarried &= (cRoI->box != nullptr);
          }
          if (allMarried) {
            frame->isBoxesReady = true;
          }
        }
      }
      mRoIExtractor->notify();
    }

    // Interpolate results
    std::set<idType> droppedIDs = Interpolator::interpolate(frames);

    // Notify results of rest of the frames
    for (auto& it : frames) {
      for (Frame* frame : it.second) {
        for (auto& cRoI : frame->childRoIs) {
          if (droppedIDs.find(cRoI->id) != droppedIDs.end()) {
            continue;
          }
          assert(cRoI->box != nullptr);
          assert(cRoI->box->srcRoI == cRoI.get());
          assert(cRoI->box->id == cRoI->id);
          assert(cRoI->box->label == cRoI->label);
        }
        frame->isBoxesReady = true;
      }
    }
    mRoIExtractor->notify();

    // Update results for system output
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    for (const auto& it : frames) {
      for (Frame* frame : it.second) {
        if (frame != fullFrameTarget) {
          std::vector<BoundingBox> boxes;
          std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(boxes),
                         [](const std::unique_ptr<BoundingBox>& box) { return *box; });
          mResults[frame->key][frame->frameIndex] = std::make_tuple(
              NowMicros(), (draw ? frame->mat : cv::Mat()),
              nms(boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold()));
        }
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    // Free frames for memory optimization
    std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
    for (auto& it : frames) {
      if (it.second.empty()) {
        continue;
      }
      Frame* firstFrame = *it.second.begin();
      Frame* lastFrame = *it.second.rbegin();
      std::vector<int> frameIndices;
      if (firstFrame->prevFrame != nullptr) {
        frameIndices.push_back(firstFrame->prevFrame->frameIndex);
      }
      for (Frame* frame : it.second) {
        if (frame != lastFrame) {
          frameIndices.push_back(frame->frameIndex);
        }
      }
      mFrameBuffers.at(it.first)->freeImage(frameIndices, mLogger.get());
    }
    framesLock.unlock();
    mixedFrameInferenceTime = NowMicros();

    LOGD("========== Schedule %d End (get %lu + full %lu + pre %lu + mix %lu us) ==========",
         scheduleID++,
         roiGettingTime - startTime,
         fullFrameInferenceTime - roiGettingTime,
         mixingPreparationTime - fullFrameInferenceTime,
         mixedFrameInferenceTime - mixingPreparationTime);

    if (scheduleInterval > mixedFrameInferenceTime - startTime) {
      std::this_thread::sleep_for(std::chrono::microseconds(
          scheduleInterval - (mixedFrameInferenceTime - startTime)));
    }
  }
}

void SpatioTemporalRoIMixer::fullFrameInference(Frame* frame) {
  std::vector<RoI> emptyRoIs;
  assert(frame->isFullFrameTarget);
  frame->fullFrameEnqueueTime = NowMicros();
  std::vector<BoundingBox> results = mInferenceEngine->getResults(
      mInferenceEngine->enqueue(frame->mat));
  frame->fullFrameGetResultsTime = NowMicros();
  for (const BoundingBox& box : results) {
    frame->boxes.emplace_back(
        new BoundingBox(UNASSIGNED_ID, box.location, box.confidence, box.label, box.origin));
  }
  mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, true);
  for (auto& box : frame->boxes) {
    assert(box->id != UNASSIGNED_ID);
  }
  frame->isBoxesReady = true;
  mRoIExtractor->notify();

  assert(std::all_of(frame->boxes.begin(), frame->boxes.end(),
                     [](const std::unique_ptr<BoundingBox>& box) { return box->label == 0; }));

  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  std::vector<BoundingBox> boxes;
  std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(boxes),
                 [](const std::unique_ptr<BoundingBox>& box) { return *box; });
  mResults[frame->key][frame->frameIndex] = std::make_tuple(
      NowMicros(), (draw ? frame->mat : cv::Mat()), std::move(boxes));
  resultLock.unlock();
  mResultsCv.notify_all();
}

Frame* SpatioTemporalRoIMixer::getFullFrameInferenceFrame(
    const std::map<std::string, SortedFrames>& frames,
    int fullFrameInferenceStreamIndex) {
  std::vector<std::string> nonEmptyStreamKeys;
  for (const auto& it : frames) {
    if (!it.second.empty()) {
      nonEmptyStreamKeys.push_back(it.first);
    }
  }
  fullFrameInferenceStreamIndex %= (int) nonEmptyStreamKeys.size();
  Frame* fullFrameTarget = *frames.at(nonEmptyStreamKeys[fullFrameInferenceStreamIndex]).rbegin();
  fullFrameTarget->isFullFrameTarget = true;
  return fullFrameTarget;
}

int SpatioTemporalRoIMixer::enqueueImage(const std::string& key, const cv::Mat& mat) {
  assert(!mat.empty());
  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(key) == mFrameBuffers.end()) {
    mFrameBuffers[key] = std::make_unique<FrameBuffer>(key, mConfig.BUFFER_SIZE);
  }
  FrameBuffer* frameBuffer = mFrameBuffers.at(key).get();
  lock.unlock();

  Frame* frame = frameBuffer->enqueue(mat);
  if (frame->frameIndex == 0) {
    mRoIExtractor->preprocess(frame);
    frame->useInferenceResultForOF = true;
    frame->isFullFrameTarget = true;
    fullFrameInference(frame);
  } else {
    if (frame->frameIndex == 1) {
      std::unique_lock<std::mutex> startLock(mStartMtx);
      mNumStartedFrameBuffers++;
      mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mNumSourceVideos; });
      mStartMtx.unlock();
      mStartCv.notify_all();
      LOGD("Start %s video at %lu us", key.c_str(), NowMicros());
    }
    mRoIExtractor->enqueue(frame);
  }
  return frame->frameIndex;
}

void SpatioTemporalRoIMixer::outputWork() {
  while (!mbStop) {
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    mResultsCv.wait(resultLock, [this]() {
      if (mbStop) {
        return true;
      }
      for (auto& streamIt : mResults) {
        std::string key = streamIt.first;
        if (mResultIndices.find(key) == mResultIndices.end()) {
          mResultIndices[key] = 0;
        }
        int frameIndex = mResultIndices[key];
        if (streamIt.second.find(frameIndex) != streamIt.second.end()) {
          return true;
        }
      }
      return false;
    });
    std::string key;
    int frameIndex;
    FrameResult result;
    for (auto& streamIt : mResults) {
      key = streamIt.first;
      frameIndex = mResultIndices[key];
      auto frameIt = streamIt.second.find(frameIndex);
      if (frameIt != streamIt.second.end()) {
        mResultIndices[key]++;
        result = frameIt->second;
        streamIt.second.erase(frameIt);
        break;
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    const time_us& time = std::get<0>(result);
    cv::Mat mat = std::get<1>(result);
    const std::vector<BoundingBox>& boxes = std::get<2>(result);
    LOGD("Logger::logResult(%s, %4d)                     // %lu boxes",
         key.substr(key.size() - 5, 1).c_str(), frameIndex, boxes.size());
    mResultLogger->logResult(key, frameIndex, time, boxes);
    if (draw) {
      drawObjectDetectionResult(mat, boxes);
    }
  }
}

void SpatioTemporalRoIMixer::drawObjectDetectionResult(const cv::Mat& mat,
                                                       const std::vector<BoundingBox>& boxes) {
  if (jvm->AttachCurrentThread(&env, nullptr) != 0) {
    return;
  }

  jobject jBoxes = env->NewObject(class_ArrayList, ArrayList_init);
  for (int i = 0; i < boxes.size(); i++) {
    const rm::BoundingBox& b = boxes.at(i);
    jobject box = env->NewObject(class_BoundingBox, BoundingBox_init,
                                 b.id,
                                 b.location.left, b.location.top, b.location.right,
                                 b.location.bottom,
                                 b.confidence, b.label, int(b.origin), (b.srcRoI == nullptr));
    env->CallVoidMethod(jBoxes, ArrayList_add, i, box);
  }
  auto* jMat = new cv::Mat();
  mat.copyTo(*jMat);
  env->CallVoidMethod(strm, SpatioTemporalRoIMixer_drawObjectDetectionResult, (long) jMat, jBoxes);

  jvm->DetachCurrentThread();
}

} // namespace rm
