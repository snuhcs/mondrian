#include "strm/SpatioTemporalRoIMixer.hpp"

#include <memory>
#include <utility>

#include "strm/Interpolator.hpp"

namespace rm {

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               InferenceEngine* inferenceEngine,
                                               int numSourceVideo,
                                               JavaVM* vm, JNIEnv* env, jobject strm, bool draw)
    : mConfig(config), mbStop(false),
      mLogger(new Logger("/data/data/hcs.offloading.strm/execution_log.csv")),
      mResultLogger(new Logger("/data/data/hcs.offloading.strm/test.log")),
      mInferenceEngine(inferenceEngine),
      mNumSourceVideos(numSourceVideo),
      mInputSizes(inferenceEngine->getInputSizes()),
      mRoIExtractor(new RoIExtractor(config.roIExtractorConfig, config.FULL_FRAME_INTERVAL > 0)),
      mRoIResizer(new RoIResizer(config.roIResizerConfig)),
      mPatchMixer(new PatchMixer(config.patchMixerConfig)),
      mPatchReconstructor(
          new PatchReconstructor(config.patchReconstructorConfig, mRoIResizer.get())),
      jvm(vm), env(nullptr), strm(reinterpret_cast<jobject>(env->NewGlobalRef(strm))), draw(draw) {
  assert(!config.ROI_WISE_INFERENCE || inferenceEngine->getInputSizes().size() >= 2);
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
  // Wait sources for synced start
  std::unique_lock<std::mutex> startLock(mStartMtx);
  mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mNumSourceVideos; });
  startLock.unlock();
  mStartCv.notify_all();

  int scheduleID = -1;
  int fullFrameInferenceStreamIndex = 0;
  time_us scheduleInterval = mConfig.LATENCY_SLO_MS * 1000 / 2;
  TimeLogger logger;
  logger.start();

  // When FULL_FRAME_INTERVAL == 0, always run full frame inference
  // See SpatioTemporalRoIMixer::enqueueImage(...)
  while (!mbStop && mConfig.FULL_FRAME_INTERVAL > 0) {
    scheduleID++;
    // Wait for scheduling interval
    time_us elapsedTime = logger.getElapsedTime();
    if (scheduleInterval > elapsedTime) {
      std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval - elapsedTime));
    }

    // Extract RoIs
    logger.start();
    std::map<std::string, SortedFrames> frames = mRoIExtractor->getExtractedFrames();
    logger.step("roi");
    LOGD("===== Schedule %d start =====", scheduleID);
    LOGD("STRM::work() getExtractedFrames() took %lld us  // %s",
         logger.getDuration("roi"), toString(frames).c_str());
    if (std::all_of(frames.begin(), frames.end(), [](auto& it) { return it.second.empty(); })) {
      LOGD("===== Schedule %d end with no RoIs =====", scheduleID);
      continue;
    }

    // Full Frame Inference
    Frame* fullFrameTarget = nullptr;
    if (scheduleID % mConfig.FULL_FRAME_INTERVAL == 0) {
      fullFrameTarget = getFullFrameInferenceFrame(frames, fullFrameInferenceStreamIndex++);
      fullFrameInference(fullFrameTarget);
    }
    logger.step("full");
    LOGD("STRM::work() fullFrameInference(%s, %d) took %lld us", fullFrameTarget->shortKey.c_str(),
         fullFrameTarget->frameIndex, logger.getDuration("full"));

    // Prepare packing. resize and merge RoIs
    int inferenceFrameSize = mConfig.ROI_WISE_INFERENCE ? mInputSizes.front() : mInputSizes.back();
    std::vector<RoI*> candidateRoIs = mPatchMixer->prepareRoIs(
        frames, fullFrameTarget, mRoIResizer.get(), inferenceFrameSize, mRoIResizer->isProbing(),
        mRoIResizer->getNumProbeSteps(), mRoIResizer->getProbeStepSize());
    logger.step("pre");
    LOGD("STRM::work() inferencePreparation took %lld us", logger.getDuration("pre"));

    // Inference
    // TODO : handle numInferences <= 0 case
    time_us remainingTime = scheduleInterval - logger.getDuration("pre", "start");
    time_us inferenceTime = mInferenceEngine->getInferenceTimeMs(inferenceFrameSize) * 1000;
    int maxNumInferences = remainingTime < inferenceTime ? 1 :
                           (int) (remainingTime / inferenceTime);
    if (mConfig.ROI_WISE_INFERENCE) {
      roiWiseInference(candidateRoIs, inferenceFrameSize, maxNumInferences);
    } else {
      mixedInference(candidateRoIs, inferenceFrameSize, maxNumInferences);
    }
    logger.step("inf");
    LOGD("STRM::work() inference took %lld us             // %lld / %lld = %d",
         logger.getDuration("inf"), remainingTime, inferenceTime,
         maxNumInferences);

    // Interpolate results
    std::set<idType> droppedIDs = Interpolator::interpolate(frames);
    logger.step("itp");
    LOGD("STRM::work() interpolate took %lld us", logger.getDuration("itp"));

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

    // Release used frames
    releaseFrames(frames);
    logger.step("post");

    LOGD("===== Schedule %d end (%s) =====", scheduleID, logger.getLog().c_str());
  }
}

void SpatioTemporalRoIMixer::fullFrameInference(Frame* frame) {
  std::vector<RoI> emptyRoIs;
  assert(frame->isFullFrameTarget);
  frame->fullFrameEnqueueTime = NowMicros();
  std::vector<BoundingBox> results = mInferenceEngine->getResults(
      mInferenceEngine->enqueue(frame->mat, mInputSizes.back()));
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

void SpatioTemporalRoIMixer::mixedInference(std::vector<RoI*>& candidateRoIs, int frameSize,
                                            int maxNumInferences) {
  // Pack into mixed frames
  std::vector<MixedFrame> mixedFrames = mPatchMixer->packRoIs(candidateRoIs, frameSize,
                                                              maxNumInferences);

  // Enqueue Mixed Frames
  std::vector<int> handles;
  std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                 [this, frameSize](const MixedFrame& mixedFrame) {
                   return mInferenceEngine->enqueue(mixedFrame.packedMat, frameSize);
                 });

  // Get results of mixed frames sequentially
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
}

void SpatioTemporalRoIMixer::roiWiseInference(std::vector<RoI*>& candidateRoIs, int frameSize,
                                              int maxNumInferences) {
  std::vector<std::pair<int, int>> packedLocations;
  std::vector<int> handles;
  for (int i = 0; i < std::min(maxNumInferences, (int) candidateRoIs.size()); i++) {
    RoI* pRoI = candidateRoIs[i];
    cv::Mat input = cv::Mat::zeros(frameSize, frameSize, CV_8UC4);
    cv::Mat roi = pRoI->getPaddedMat();
    if (pRoI->maxEdgeLength > frameSize) {
      std::pair<int, int> wh = pRoI->getResizedWidthHeight();
      cv::resize(roi, roi, cv::Size(wh.first, wh.second));
    }
    int x = (frameSize - roi.cols) / 2;
    int y = (frameSize - roi.rows) / 2;
    roi.copyTo(input(cv::Rect(x, y, roi.cols, roi.rows)));
    packedLocations.emplace_back(x, y);
    handles.push_back(mInferenceEngine->enqueue(input, frameSize));
  }
  assert(handles.size() == std::min(maxNumInferences, (int) candidateRoIs.size()));

  SortedFrames inferenceFrames;
  for (int i = 0; i < handles.size(); i++) {
    std::vector<BoundingBox> boxes = mInferenceEngine->getResults(handles[i]);
    std::pair<int, int>& xy = packedLocations[i];
    RoI* pRoI = candidateRoIs[i];
    inferenceFrames.insert(pRoI->frame);
    for (BoundingBox& box : boxes) {
      pRoI->frame->boxes.emplace_back(new BoundingBox(
          UNASSIGNED_ID, Rect(
              (box.location.left - xy.first) * pRoI->maxEdgeLength / pRoI->targetSize +
              pRoI->origLoc.left,
              (box.location.top - xy.second) * pRoI->maxEdgeLength / pRoI->targetSize +
              pRoI->origLoc.top,
              (box.location.right - xy.first) * pRoI->maxEdgeLength / pRoI->targetSize +
              pRoI->origLoc.left,
              (box.location.bottom - xy.second) * pRoI->maxEdgeLength / pRoI->targetSize +
              pRoI->origLoc.top),
          box.confidence, box.label, pRoI->origin));
    }
  }

  for (Frame* frame : inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
    mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
  }

}

void SpatioTemporalRoIMixer::releaseFrames(const std::map<std::string, SortedFrames>& frames) {
  std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
  for (auto&[aStreamKey, aStreamFrames] : frames) {
    if (aStreamFrames.empty()) {
      continue;
    }
    Frame* firstFrame = *aStreamFrames.begin();
    Frame* lastFrame = *aStreamFrames.rbegin();
    std::vector<int> frameIndices;
    if (firstFrame->prevFrame != nullptr) {
      frameIndices.push_back(firstFrame->prevFrame->frameIndex);
    }
    for (Frame* frame : aStreamFrames) {
      if (frame != lastFrame) {
        frameIndices.push_back(frame->frameIndex);
      }
    }
    mFrameBuffers.at(aStreamKey)->freeImage(frameIndices, mLogger.get());
  }
  framesLock.unlock();
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
  if (mConfig.FULL_FRAME_INTERVAL == 0 || frame->frameIndex == 0) {
    mRoIExtractor->preprocess(frame);
    frame->useInferenceResultForOF = true;
    frame->isFullFrameTarget = true;
    fullFrameInference(frame);
    if (mConfig.FULL_FRAME_INTERVAL == 0) {
      std::lock_guard<std::mutex> framesLock(mFrameBuffersMtx);
      mFrameBuffers.at(frame->key)->freeImage({frame->frameIndex}, mLogger.get());
    }
  } else {
    if (frame->frameIndex == 1) {
      std::unique_lock<std::mutex> startLock(mStartMtx);
      mNumStartedFrameBuffers++;
      mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mNumSourceVideos; });
      mStartMtx.unlock();
      mStartCv.notify_all();
      LOGD("Start %s video at %lld us", key.c_str(), NowMicros());
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
         key.substr(key.find_last_of('.') - 1, 1).c_str(), frameIndex, boxes.size());
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
