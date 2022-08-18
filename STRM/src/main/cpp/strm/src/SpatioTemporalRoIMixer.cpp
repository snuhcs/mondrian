#include "strm/SpatioTemporalRoIMixer.hpp"

#include <cmath>
#include <memory>
#include <numeric>
#include <utility>

#include "strm/Interpolator.hpp"

namespace rm {

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               InferenceEngine* inferenceEngine,
                                               int numSourceVideo,
                                               JavaVM* vm, JNIEnv* env, jobject strm, bool draw)
    : mConfig(config), mbStop(false),
      mResultLogger(new Logger("/data/data/hcs.offloading.strm/test.log")),
      mInferenceEngine(inferenceEngine),
      mNumSourceVideos(numSourceVideo),
      mTargetSize(config.roIExtractorConfig.EXTRACTION_RESIZE_WIDTH,
                  config.roIExtractorConfig.EXTRACTION_RESIZE_HEIGHT),
      mInputSizes(inferenceEngine->getInputSizes()),
      mInferenceFrameSize(mConfig.ROI_WISE_INFERENCE ? mInputSizes.front() : mInputSizes.back()),
      mScheduleInterval(mConfig.LATENCY_SLO_MS * 1000 / 2),
      mRoIResizer(new RoIResizer(config.roiResizerConfig)),
      mPatchMixer(new PatchMixer(config.patchMixerConfig)),
      mPatchReconstructor(
          new PatchReconstructor(config.patchReconstructorConfig, mRoIResizer.get())),
      jvm(vm), env(nullptr), strm(reinterpret_cast<jobject>(env->NewGlobalRef(strm))), draw(draw) {
  assert(!config.ROI_WISE_INFERENCE || mInputSizes.size() >= 2);
  for (int inputSize : mInputSizes) {
    if (inputSize != mInputSizes.back()) {
      for (int i = 0; i < 3; i++) {
        mInferenceEngine->getResults(mInferenceEngine->enqueue(
            cv::Mat::zeros(inputSize, inputSize, CV_8UC4), inputSize));
      }
    }
  }

  if (config.LOG_EXECUTION) {
    mLogger = std::make_unique<Logger>("/data/data/hcs.offloading.strm/execution_log.csv");
    mLogger->logHeader();
  }
  if (config.LOG_ROI) {
    mRoILogger = std::make_unique<Logger>("/data/data/hcs.offloading.strm/roi_log.csv");
    mRoILogger->logRoIHeader();
  }

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
  int scheduleID = -1;
  int fullFrameStreamIndex = 0;

  // Wait sources for synced start
  waitForStart();

  TimeLogger logger;
  logger.start();

  // When FULL_FRAME_INTERVAL == 0, always run full frame inference
  // See SpatioTemporalRoIMixer::enqueueImage(...)
  while (!mbStop && mConfig.FULL_FRAME_INTERVAL > 0) {
    scheduleID++;
    // Wait for scheduling interval
    time_us elapsedTime = logger.getElapsedTime();
    if (mConfig.ALLOW_INTERPOLATION && mScheduleInterval > elapsedTime) {
      std::this_thread::sleep_for(std::chrono::microseconds(mScheduleInterval - elapsedTime));
    }

    // Extract RoIs
    logger.start();
    bool runFull = scheduleID % mConfig.FULL_FRAME_INTERVAL == 0;
    int numInferences = getNumInferences(
        mScheduleInterval - (runFull ? mInferenceEngine->getInferenceTimeMs(mInputSizes.back()) * 1000 : 0),
        mInferenceEngine->getInferenceTimeMs(mInferenceFrameSize) * 1000);
    MultiStream frames = mRoIExtractor->getExtractedFrames(numInferences);
    logger.step("roi");
    LOGD("===== Schedule %d start =====", scheduleID);
    LOGD("%-25s took %-7lld us for %s",
         "RE::getExtractedFrames", logger.getDuration("roi"), toString(frames).c_str());
    if (std::all_of(frames.begin(), frames.end(), [](auto& it) { return it.second.empty(); })) {
      LOGD("===== Schedule %d end with no RoIs =====", scheduleID);
      continue;
    }

    // Schedule
    auto[mixedFrames, fullFrameTarget, selectedFrames, droppedFrames] = mPatchMixer->packRoIs(
        frames, (runFull ? fullFrameStreamIndex++ : -1), mInferenceFrameSize,
        numInferences, mConfig.ALLOW_INTERPOLATION, mConfig.ROI_WISE_INFERENCE,
        mRoIResizer->isProbing(), mRoIResizer->getNumProbeSteps(), mRoIResizer->getProbeStepSize());
    assert(runFull == (fullFrameTarget != nullptr));
    if (!mConfig.ALLOW_INTERPOLATION) {
      testNoInterpolationPacking(frames, droppedFrames, fullFrameTarget);
    }
    mRoIExtractor->reEnqueueFrames(droppedFrames);
    logger.step("pack");
    LOGD("%-25s took %-7lld us                            // %4lu MixedFrames",
         "PatchMixer::packRoIs", logger.getDuration("pack"), mixedFrames.size());

    // Full Frame Inference Target Stream
    if (runFull) {
      fullFrameInference(fullFrameTarget);
    }
    logger.step("full");
    LOGD("%-25s took %-7lld us for video %-5s frame %-4d",
         "STRM::fullFrameInference", logger.getDuration("full"),
         fullFrameTarget != nullptr ? fullFrameTarget->shortKey.c_str() : "-1",
         fullFrameTarget != nullptr ? fullFrameTarget->frameIndex : -1);

    // Inference
    if (mConfig.ROI_WISE_INFERENCE) {
      roiWiseInference(mixedFrames);
    } else {
      mixedInference(mixedFrames);
    }
    logger.step("inf");
    LOGD("%-25s took %-7lld us                            // %4d inferences",
         mConfig.ROI_WISE_INFERENCE ? "STRM::roiWiseInference" : "STRM:mixedFrameInference",
         logger.getDuration("inf"), numInferences);

    // Interpolate results
    std::set<idType> droppedIDs = Interpolator::interpolate(selectedFrames);
    logger.step("itp");
    LOGD("%-25s took %-7lld us                            // %4lu droppedIDs",
         "Interpolator::interpolate", logger.getDuration("itp"), droppedIDs.size());

    // Notify results of rest of the frames
    for (auto& it : selectedFrames) {
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
    for (const auto& it : selectedFrames) {
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
    releaseFrames(selectedFrames);
    logger.step("post");

    LOGD("===== Schedule %d end (%s) =====", scheduleID, logger.getLog().c_str());
  }
}

void SpatioTemporalRoIMixer::testNoInterpolationPacking(
    const MultiStream& frames, const Stream& droppedFrames, Frame* fullFrameTarget) {
  int numWrong = 0;
  int numCorrect = 0;
  for (auto&[aStreamKey, aStreamFrames]: frames) {
    for (Frame* frame : aStreamFrames) {
      if (!std::all_of(frame->parentRoIs.begin(), frame->parentRoIs.end(),
                       [](const std::unique_ptr<RoI>& pRoI) { return pRoI->isPacked(); })) {
        if (droppedFrames.find(frame) == droppedFrames.end() && frame != fullFrameTarget) {
          numWrong++;
        }
      } else {
        numCorrect++;
      }
    }
  }
  if (numCorrect > 0) {
    assert(numWrong == 0);
  } else {
    assert(numWrong <= 1);
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
        new BoundingBox(UNASSIGNED_ID, box.location, box.confidence, box.label, origin_FF));
  }
  mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, true);

  for (auto& box : frame->boxes) {
    assert(box->id != UNASSIGNED_ID);
  }
  frame->isBoxesReady = true;
  if (mRoIExtractor != nullptr) {
    mRoIExtractor->notify();
  }

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

void SpatioTemporalRoIMixer::mixedInference(std::vector<MixedFrame>& mixedFrames) {
  time_us inferenceStartTime = NowMicros();
  // Enqueue Mixed Frames
  std::vector<int> handles;
  std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                 [this](const MixedFrame& mixedFrame) {
                   return mInferenceEngine->enqueue(mixedFrame.packedMat, mInferenceFrameSize);
                 });

  // Get results of mixed frames sequentially
  for (int i = 0; i < mixedFrames.size(); i++) {
    std::vector<BoundingBox> results = mInferenceEngine->getResults(handles[i]);
    time_us inferenceEndTime = NowMicros();
    for (Frame* frame : mixedFrames[i].getPackedFrames()) {
      frame->inferenceStartTime = inferenceStartTime;
      frame->inferenceEndTime = inferenceEndTime;
    }
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
  for (int i = 0; i < mixedFrames.size(); i++) {
    for (Frame* frame : mixedFrames[i].getPackedFrames()) {
      if (std::any_of(frame->boxes.begin(), frame->boxes.end(),
                      [](auto& box) { return box->id == UNASSIGNED_ID; })) {
        // Match boxes with RoIs (per frame)
        nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
        mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
      }
    }
  }
}

void SpatioTemporalRoIMixer::roiWiseInference(std::vector<MixedFrame>& mixedFrames) {
  time_us inferenceStartTime = NowMicros();
  std::vector<int> handles;
  std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                 [this](const MixedFrame& mixedFrame) {
                   return mInferenceEngine->enqueue(mixedFrame.packedMat, mInferenceFrameSize);
                 });

  Stream inferenceFrames;
  for (int i = 0; i < handles.size(); i++) {
    std::vector<BoundingBox> boxes = mInferenceEngine->getResults(handles[i]);
    assert(mixedFrames[i].packedRoIs.size() == 1);
    auto&[x, y] = (*mixedFrames[i].packedRoIs.begin())->packedLocation;
    RoI* pRoI = *mixedFrames[i].packedRoIs.begin();
    pRoI->frame->inferenceStartTime = inferenceStartTime;
    pRoI->frame->inferenceEndTime = NowMicros();
    inferenceFrames.insert(pRoI->frame);
    for (BoundingBox& b : boxes) {
      pRoI->frame->boxes.emplace_back(new BoundingBox(
          UNASSIGNED_ID,Rect(
              (b.location.left - x) * pRoI->maxEdgeLength / pRoI->getTargetSize() + pRoI->origLoc.left,
              (b.location.top - y) * pRoI->maxEdgeLength / pRoI->getTargetSize() + pRoI->origLoc.top,
              (b.location.right - x) * pRoI->maxEdgeLength / pRoI->getTargetSize() + pRoI->origLoc.left,
              (b.location.bottom - y) * pRoI->maxEdgeLength / pRoI->getTargetSize() + pRoI->origLoc.top),
          b.confidence, b.label, pRoI->origin));
    }
  }

  for (Frame* frame : inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
    mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
  }
}

void SpatioTemporalRoIMixer::releaseFrames(const MultiStream& frames) {
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
    mFrameBuffers.at(aStreamKey)->freeImage(frameIndices, mLogger.get(), mRoILogger.get());
  }
  framesLock.unlock();
}

void SpatioTemporalRoIMixer::waitForStart() {
  std::unique_lock<std::mutex> startLock(mStartMtx);
  mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mNumSourceVideos; });
  mRoIExtractor = std::make_unique<RoIExtractor>(
      mConfig.roIExtractorConfig, mConfig.FULL_FRAME_INTERVAL > 0, mConfig.ALLOW_INTERPOLATION,
      mConfig.ROI_WISE_INFERENCE, mPatchMixer.get(), mRoIResizer.get(), mInferenceFrameSize,
      getNumInferences(mScheduleInterval, mInferenceEngine->getInferenceTimeMs(mInferenceFrameSize) * 1000));
  startEnqueue = true;
  startLock.unlock();
  mEnqueueCv.notify_all();
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
  time_us startTime = NowMicros();
  preprocess(frame);
  LOGD("%-25s took %-7lld us for video %-5s frame %-4d",
       "STRM::preprocess", NowMicros() - startTime, frame->shortKey.c_str(), frame->frameIndex);
  if (mConfig.FULL_FRAME_INTERVAL == 0 || frame->frameIndex == 0) {
    frame->useInferenceResultForOF = true;
    frame->isFullFrameTarget = true;
    fullFrameInference(frame);
    if (mConfig.FULL_FRAME_INTERVAL == 0) {
      std::lock_guard<std::mutex> framesLock(mFrameBuffersMtx);
      mFrameBuffers.at(frame->key)->freeImage({frame->frameIndex}, mLogger.get(), mRoILogger.get());
    }
  } else {
    if (frame->frameIndex == 1) {
      std::unique_lock<std::mutex> startLock(mStartMtx);
      mNumStartedFrameBuffers++;
      mStartCv.notify_one();
      mEnqueueCv.wait(startLock, [this]() { return startEnqueue; });
      startLock.unlock();
      LOGD("Start %s video at %lld us", key.c_str(), NowMicros());
    }
    mRoIExtractor->enqueue(frame);
  }
  return frame->frameIndex;
}

void SpatioTemporalRoIMixer::preprocess(Frame* frame) const {
  cv::resize(frame->mat, frame->preProcessedMat, mTargetSize, 0, 0, CV_INTER_NN);
  cv::cvtColor(frame->preProcessedMat, frame->preProcessedMat, cv::COLOR_BGRA2GRAY);
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
    LOGD("Logger::logResult                         for video %-5s frame %-4d // %4lu boxes",
         Frame::toShortKey(key).c_str(), frameIndex, boxes.size());
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
                                 int(std::round(b.location.left)), int(std::round(b.location.top)),
                                 int(std::round(b.location.right)),
                                 int(std::round(b.location.bottom)),
                                 b.confidence, b.label, int(b.origin), (b.srcRoI == nullptr));
    env->CallVoidMethod(jBoxes, ArrayList_add, i, box);
  }
  auto* jMat = new cv::Mat();
  mat.copyTo(*jMat);
  env->CallVoidMethod(strm, SpatioTemporalRoIMixer_drawObjectDetectionResult, (long) jMat, jBoxes);

  jvm->DetachCurrentThread();
}

int SpatioTemporalRoIMixer::getNumInferences(time_us remainingTime, time_us inferenceTime) {
  if (remainingTime < inferenceTime) {
    LOGE("RemainingTime: %7lld < InferenceTime: %7lld", remainingTime, inferenceTime);
  }
  return std::max(1, int(remainingTime / inferenceTime));
}

} // namespace rm
