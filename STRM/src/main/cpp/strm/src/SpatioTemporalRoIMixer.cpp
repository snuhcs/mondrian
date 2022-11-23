#include "strm/SpatioTemporalRoIMixer.hpp"

#include <cmath>
#include <memory>
#include <numeric>
#include <utility>

#include "strm/FrameBuffer.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/InferencePlanner.hpp"
#include "strm/Interpolator.hpp"
#include "strm/Log.hpp"
#include "strm/Logger.hpp"
#include "strm/MixedFrame.hpp"
#include "strm/RoIExtractor.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/PatchReconstructor.hpp"

namespace rm {

const bool SpatioTemporalRoIMixer::FAIR = true;

static auto printLatencyTable = [](const std::map<Device, std::map<int, time_us>>& latencyTable) {
  std::stringstream ss;
  for (const auto&[device, size_latency]: latencyTable) {
    for (const auto&[size, latency]: size_latency) {
      assert(device != NO_DEVICE);
      ss << (device == GPU ? "GPU" : "DSP") << " " << size << " " << latency << " us" << std::endl;
    }
  }
  LOGD("Latency Table:\n%s", ss.str().c_str());
};

static auto key_set = [](const std::map<int, int>& map) {
  std::set<int> keys;
  for (const auto&[k, v]: map) {
    keys.insert(k);
  }
  return keys;
};

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               std::map<int, int> startIndices,
                                               JavaVM* vm, JNIEnv* env, jobject emulator)
    : mConfig(config), mbStop(false),
      mResultLogger(new Logger("/data/data/hcs.offloading.strm/boxes.txt")),
      mStartIndices(std::move(startIndices)),
      mTargetSize(int(mConfig.roIExtractorConfig.EXTRACTION_RESIZE_WIDTH),
                  int(mConfig.roIExtractorConfig.EXTRACTION_RESIZE_HEIGHT)),
      mInputSizes(mConfig.inferenceEngineConfig.INPUT_SIZES),
      mScheduleInterval(mConfig.LATENCY_SLO_MS * 1000 / 2),
      mRoIResizer(new RoIResizer(config.roiResizerConfig)),
      mInferenceEngine(new InferenceEngine(config.inferenceEngineConfig, vm, env, emulator)),
      mPatchReconstructor(new PatchReconstructor(
          config.patchReconstructorConfig, mRoIResizer.get())) {
  assert(!config.USE_ROI_WISE_INFERENCE || mInputSizes.size() >= 2);
  int maxMergeSize = config.USE_EMULATED_BATCH
                     ? config.ROI_SIZE
                     : mInputSizes.front();
  bool runRoIExtractor = mConfig.FULL_FRAME_INTERVAL != 0;
  auto latencyTable = mInferenceEngine->getInferenceTimeTable();
  printLatencyTable(latencyTable);
  auto inferencePlan = InferencePlanner::getInferencePlan(latencyTable, mScheduleInterval,
                                                          mConfig.USE_ROI_WISE_INFERENCE);
  auto vids = key_set(mStartIndices);
  mRoIExtractor = std::make_unique<RoIExtractor>(
      mConfig.roIExtractorConfig, maxMergeSize, runRoIExtractor, mRoIResizer.get(),
      config.USE_EMULATED_BATCH, config.ROI_SIZE, inferencePlan, vids);

  if (config.LOG_EXECUTION) {
    mExecutionLogger = std::make_unique<Logger>("/data/data/hcs.offloading.strm/timeline.csv");
    mExecutionLogger->logExecutionHeader();
  }
  if (config.LOG_ROI) {
    mRoILogger = std::make_unique<Logger>("/data/data/hcs.offloading.strm/roi.csv");
    mRoILogger->logRoIHeader();
  }
  mThread = std::thread([this]() { work(); });
  mResultThread = std::thread([this]() { outputWork(); });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mResultsCv.notify_all();
  mThread.join();
  mResultThread.join();
}

void SpatioTemporalRoIMixer::work() {
  LOGD("STRM::work()");
  int scheduleID = -1;

  // Wait sources for synced start
  waitForStart();
  std::this_thread::sleep_for(std::chrono::microseconds(mScheduleInterval));

  TimeLogger logger;
  logger.start();

  // When FULL_FRAME_INTERVAL == 0, always run full frame inference
  // See SpatioTemporalRoIMixer::enqueueImage(...)
  while (!mbStop && mConfig.FULL_FRAME_INTERVAL > 0) {
    scheduleID++;
    bool fullFramePlan = scheduleID % mConfig.FULL_FRAME_INTERVAL == 0;

    // Wait for scheduling interval
    time_us elapsedTime = logger.getElapsedTime();
    if (mConfig.ALLOW_INTERPOLATION && mScheduleInterval > elapsedTime) {
      std::this_thread::sleep_for(std::chrono::microseconds(mScheduleInterval - elapsedTime));
    }

    LOGD("========== Start ==========");
    // 1. Inference planning
    logger.start();
    auto latencyTable = mInferenceEngine->getInferenceTimeTable();
    std::vector<InferenceInfo> inferencePlan = InferencePlanner::getInferencePlan(
        latencyTable, mScheduleInterval, mConfig.USE_ROI_WISE_INFERENCE,
        {{GPU, fullFramePlan ? mConfig.FULL_FRAME_SIZE : 0L}});
    logger.step("plan");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         "STRM::getInferencePlan", logger.getDuration("plan"), toString(inferencePlan).c_str());
    assert(!inferencePlan.empty());

    // 2. Prepare inference
    auto results = mRoIExtractor->prepareInference(inferencePlan, fullFramePlan);
    auto&[mixedFrames, fullFrameTarget, selectedFrames, droppedFrames] = results;
    logger.step("prep");
    LOGD("%-25s took %-7lld us                            "
         "// %4lu MixedFrames with %s, %lu droppedFrames",
         "RE::prepareInference", logger.getDuration("prep"),
         mixedFrames.size(), toString(selectedFrames).c_str(), droppedFrames.size());
    assert(fullFrameTarget != nullptr || !mixedFrames.empty());

    // 3. Enqueue full frame
    if (fullFrameTarget != nullptr) {
      mInferenceEngine->enqueue(fullFrameTarget->mat, GPU, mConfig.FULL_FRAME_SIZE,
                                fullFrameTarget->getKey());
      fullFrameTarget->inferenceFrameSize = mConfig.FULL_FRAME_SIZE;
      fullFrameTarget->inferenceDevice = GPU;
      LOGD("mInferenceEngine->enqueue %d sized fullFrame to %s | %d",
           mConfig.FULL_FRAME_SIZE, "GPU", fullFrameTarget->frameIndex);
      logger.step("full");
    } else {
      logger.step("full");
    }

    // 4. Enqueue mixed Frames
    for (const auto& mixedFrame: mixedFrames) {
      assert(mixedFrame.device != NO_DEVICE);
      mInferenceEngine->enqueue(mixedFrame.packedMat, mixedFrame.device,
                                mixedFrame.mixedFrameSize, mixedFrame.getKey());
      LOGD("mInferenceEngine->enqueue %d sized %d mixedFrame to %s | %s",
           mixedFrame.mixedFrameSize, mixedFrame.mixedFrameIndex,
           mixedFrame.device == GPU ? "GPU" : "DSP",
           toString(mixedFrame.getPackedFrames()).c_str());
    }

    // 5. Handle full frame inference results
    if (fullFrameTarget != nullptr) {
      handleFullFrameInferenceResults(fullFrameTarget);
      LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
           "STRM::fullFrameInference", logger.getDuration("full"),
           fullFrameTarget->vid, fullFrameTarget->frameIndex);
    }

    // 6. Handle mixed frame or RoI-wise inference results
    if (mConfig.USE_ROI_WISE_INFERENCE) {
      handleRoIWiseInferenceResults(mixedFrames);
    } else {
      handleMixedFrameInferenceResults(mixedFrames);
    }
    logger.step("inf");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         mConfig.USE_ROI_WISE_INFERENCE ? "STRM::handleRoIWiseInferenceResults"
                                        : "STRM::handleMixedFrameInferenceResults",
         logger.getDuration("inf"), toString(inferencePlan).c_str());

    // 7. Interpolate results
    Interpolator::interpolate(selectedFrames, mConfig.INTERPOLATION_THRESHOLD);
    logger.step("itp");
    LOGD("%-25s took %-7lld us", "Interpolator::interpolate", logger.getDuration("itp"));

    // 8. Notify results of rest of the frames
    for (auto& it: selectedFrames) {
      for (Frame* frame: it.second) {
        for (auto& cRoI: frame->childRoIs) {
          if (cRoI->box == nullptr) {
            continue;
          }
          assert(cRoI->box->srcRoI == cRoI.get());
          assert(cRoI->box->id == cRoI->id);
          assert(cRoI->box->label == cRoI->label);
        }
        frame->isBoxesReady = true;
        frame->endTime = NowMicros();
      }
    }
    mRoIExtractor->notify();

    // 9. Update results for system output
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    for (const auto& it: selectedFrames) {
      for (Frame* frame: it.second) {
        if (frame != fullFrameTarget) {
          std::vector<BoundingBox> boxes;
          std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(boxes),
                         [](const std::unique_ptr<BoundingBox>& box) { return *box; });
          mResults[frame->vid][frame->frameIndex] = {
              frame->endTime, nms(boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold())};
        }
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    // 10. Release used frames
    releaseFrames(selectedFrames);
    logger.step("post");

    LOGD("Latency breakdown: %s", logger.getLog().c_str());
    LOGD("==========  End  ==========");
  }
}

void SpatioTemporalRoIMixer::handleFullFrameInferenceResults(Frame* frame) {
  auto[boxes, times, device] = mInferenceEngine->getResults(frame->getKey());
  assert(device == GPU);
  frame->fullInferenceStartTime = times.first;
  frame->fullInferenceEndTime = times.second;
  for (const BoundingBox& box: boxes) {
    auto& loc = box.location;
    frame->boxes.push_back(std::make_unique<BoundingBox>(
        UNASSIGNED_ID, box.location, box.confidence, box.label, origin_FF));
  }
  mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, true);

  for (auto& box: frame->boxes) {
    assert(box->id != UNASSIGNED_ID);
  }
  frame->isBoxesReady = true;
  frame->endTime = NowMicros();
  if (mRoIExtractor != nullptr) {
    mRoIExtractor->notify();
  }

  log(frame);
  assert(std::all_of(frame->boxes.begin(), frame->boxes.end(),
                     [](const std::unique_ptr<BoundingBox>& box) { return box->label == 0; }));

  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  std::vector<BoundingBox> resultBoxes;
  std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(resultBoxes),
                 [](const std::unique_ptr<BoundingBox>& box) { return *box; });
  mResults[frame->vid][frame->frameIndex] = {NowMicros(), std::move(resultBoxes)};
  resultLock.unlock();
  mResultsCv.notify_all();
}

void SpatioTemporalRoIMixer::handleMixedFrameInferenceResults(
    std::vector<MixedFrame>& mixedFrames) {
  // Get results of mixed frames sequentially
  for (int i = 0; i < mixedFrames.size(); i++) {
    auto[boxes, times, device] = mInferenceEngine->getResults(mixedFrames[i].getKey());
    assert(device == mixedFrames[i].device);
    LOGD("mInferenceEngine->getResults %d sized %d mixedFrame to %s", mixedFrames[i].mixedFrameSize,
         mixedFrames[i].mixedFrameIndex, mixedFrames[i].device == GPU ? "GPU" : "DSP");
    for (Frame* frame: mixedFrames[i].getPackedFrames()) {
      if (frame->inferenceDevice == NO_DEVICE) {
        frame->inferenceFrameSize = mixedFrames[i].mixedFrameSize;
        frame->inferenceDevice = device;
        frame->mixedInferenceStartTime = times.first;
        frame->mixedInferenceEndTime = times.second;
      }
    }
    mixedFrames[i].packedMat.release();
    mPatchReconstructor->assignBoxesToFrame(mixedFrames[i], boxes);

    // Notify results of processed frames
    for (Frame* frame: mixedFrames[i].getPackedFrames()) {
      if (frame->isReadyToMarry(i)) {
        // Match boxes with RoIs (per frame)
        nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
        mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);

        bool allMarried = true;
        for (auto& cRoI: frame->childRoIs) {
          allMarried &= (cRoI->box != nullptr);
        }
        if (allMarried) {
          frame->isBoxesReady = true;
        }
      }
    }
    mRoIExtractor->notify();
  }
  for (auto& mixedFrame: mixedFrames) {
    for (Frame* frame: mixedFrame.getPackedFrames()) {
      bool isAllUnassigned = std::all_of(frame->boxes.begin(), frame->boxes.end(),
                                         [](auto& box) { return box->id == UNASSIGNED_ID; });
      bool isAllAssigned = std::all_of(frame->boxes.begin(), frame->boxes.end(),
                                       [](auto& box) { return box->id != UNASSIGNED_ID; });
      assert(isAllUnassigned || isAllAssigned);
      if (isAllUnassigned) {
        // Match boxes with RoIs (per frame)
        nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
        mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
      }
    }
  }
}

void SpatioTemporalRoIMixer::handleRoIWiseInferenceResults(
    std::vector<MixedFrame>& mixedFrames) {
  Stream inferenceFrames;
  for (auto& mixedFrame: mixedFrames) {
    auto[boxes, times, device] = mInferenceEngine->getResults(mixedFrame.getKey());
    assert(mixedFrame.packedRoIs.size() == 1);
    assert(device == mixedFrame.device);
    auto[x, y] = (*mixedFrame.packedRoIs.begin())->getPackedXY();
    RoI* pRoI = *mixedFrame.packedRoIs.begin();
    if (pRoI->frame->inferenceDevice == NO_DEVICE) {
      pRoI->frame->inferenceFrameSize = mixedFrame.mixedFrameSize;
      pRoI->frame->inferenceDevice = device;
      pRoI->frame->mixedInferenceStartTime = times.first;
      pRoI->frame->mixedInferenceEndTime = times.second;
    }
    inferenceFrames.insert(pRoI->frame);
    for (BoundingBox& b: boxes) {
      pRoI->frame->boxes.push_back(std::make_unique<BoundingBox>(
          UNASSIGNED_ID, Rect(
              (b.location.left - x) / pRoI->getTargetScale() + pRoI->origLoc.left,
              (b.location.top - y) / pRoI->getTargetScale() + pRoI->origLoc.top,
              (b.location.right - x) / pRoI->getTargetScale() + pRoI->origLoc.left,
              (b.location.bottom - y) / pRoI->getTargetScale() + pRoI->origLoc.top),
          b.confidence, b.label, pRoI->origin));
    }
  }

  for (Frame* frame: inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
    mPatchReconstructor->matchBoxesWithRoIs(frame->childRoIs, frame->boxes, false);
  }
}

void SpatioTemporalRoIMixer::releaseFrames(const MultiStream& frames) {
  std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
  for (const auto& it: frames) {
    const int vid = it.first;
    const Stream& aStreamFrames = it.second;
    if (aStreamFrames.empty()) {
      continue;
    }
    for (Frame* frame: aStreamFrames) {
      log(frame);
    }
    Frame* lastFrame = *aStreamFrames.rbegin();
    std::vector<int> freeFrameIndices;

    Frame* handle = lastFrame;
    // Skip {pdInterval} frames
    for (int i = 0; i < mConfig.roIExtractorConfig.PD_INTERVAL; i++) {
      assert(handle != nullptr);
      handle = handle->prevFrame;
      if (handle == nullptr) {
        break;
      }
    }
    while (handle != nullptr) {
      freeFrameIndices.push_back(handle->frameIndex);
      handle = handle->prevFrame;
    }

    if (!freeFrameIndices.empty()) {
      mFrameBuffers.at(vid)->freeImage(freeFrameIndices);
    }
  }
  framesLock.unlock();
}

void SpatioTemporalRoIMixer::log(const Frame* frame) {
  if (mExecutionLogger) {
    mExecutionLogger->logExecution(frame);
  }
  if (mRoILogger) {
    for (auto& cRoI: frame->childRoIs) {
      mRoILogger->logRoI(cRoI.get());
    }
  }
}

void SpatioTemporalRoIMixer::waitForStart() {
  std::unique_lock<std::mutex> startLock(mStartMtx);
  mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mStartIndices.size(); });
  mbStartEnqueue = true;
  startLock.unlock();
  mEnqueueCv.notify_all();
}

int SpatioTemporalRoIMixer::enqueueImage(const int vid, const cv::Mat& mat) {
  assert(!mat.empty());

  if (FAIR) {
    std::unique_lock<std::mutex> fairLock(mFairEnqueueMtx);
    mFairCv.wait(fairLock, [vid, this]() {
      return mStartIndices.size() == 1 || mPrevEnqueuedVid != vid;
    });
    mPrevEnqueuedVid = vid;
    fairLock.unlock();
    mFairCv.notify_all();
  }

  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(vid) == mFrameBuffers.end()) {
    mFrameBuffers[vid] = std::make_unique<FrameBuffer>(
        vid, mConfig.BUFFER_SIZE, mStartIndices[vid]);
  }
  FrameBuffer* frameBuffer = mFrameBuffers.at(vid).get();
  lock.unlock();

  Frame* frame = frameBuffer->enqueue(mat);
  time_us startTime = NowMicros();
  preprocess(frame);
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
       "STRM::preprocess", NowMicros() - startTime, frame->vid, frame->frameIndex);
  if (mConfig.FULL_FRAME_INTERVAL == 0 || frame->frameIndex == mStartIndices[vid]) {
    frame->useInferenceResultForOF = true;
    mInferenceEngine->enqueue(frame->mat, GPU, mConfig.FULL_FRAME_SIZE, frame->getKey());
    frame->inferenceFrameSize = mConfig.FULL_FRAME_SIZE;
    frame->inferenceDevice = GPU;
    LOGD("mInferenceEngine->enqueue %d sized fullFrame to %s | %d",
         mConfig.FULL_FRAME_SIZE, "GPU", frame->frameIndex);
    handleFullFrameInferenceResults(frame);
    if (mConfig.FULL_FRAME_INTERVAL == 0) {
      std::lock_guard<std::mutex> framesLock(mFrameBuffersMtx);
      log(frame);
      mFrameBuffers.at(frame->vid)->freeImage({frame->frameIndex});
    }
  } else {
    if (frame->frameIndex == mStartIndices[vid] + 1) {
      std::unique_lock<std::mutex> startLock(mStartMtx);
      mNumStartedFrameBuffers++;
      mStartCv.notify_one();
      mEnqueueCv.wait(startLock, [this]() { return mbStartEnqueue; });
      startLock.unlock();
      LOGD("Start %d video at %lld us", vid, NowMicros());
    }
    mRoIExtractor->enqueue(frame);
  }
  return frame->frameIndex;
}

void SpatioTemporalRoIMixer::preprocess(Frame* frame) const {
  cv::resize(frame->mat, frame->preProcessedMat, mTargetSize, 0, 0, CV_INTER_LINEAR);
  cv::cvtColor(frame->preProcessedMat, frame->preProcessedMat, cv::COLOR_BGRA2GRAY);
}

void SpatioTemporalRoIMixer::outputWork() {
  while (!mbStop) {
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    mResultsCv.wait(resultLock, [this]() {
      if (mbStop) {
        return true;
      }
      for (auto& streamIt: mResults) {
        int vid = streamIt.first;
        if (mResultIndices.find(vid) == mResultIndices.end()) {
          mResultIndices[vid] = mStartIndices[vid];
        }
        int frameIndex = mResultIndices[vid];
        if (streamIt.second.find(frameIndex) != streamIt.second.end()) {
          return true;
        }
      }
      return false;
    });
    int vid;
    int frameIndex;
    FrameResult result;
    for (auto& streamIt: mResults) {
      vid = streamIt.first;
      frameIndex = mResultIndices[vid];
      auto frameIt = streamIt.second.find(frameIndex);
      if (frameIt != streamIt.second.end()) {
        mResultIndices[vid]++;
        result = frameIt->second;
        streamIt.second.erase(frameIt);
        break;
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    auto&[time, boxes] = result;
    LOGD("Logger::logResult                         for video %-5d frame %-4d // %4lu boxes",
         vid, frameIndex, boxes.size());
    mResultLogger->logResult(vid, frameIndex, time, boxes);
  }
}

} // namespace rm
