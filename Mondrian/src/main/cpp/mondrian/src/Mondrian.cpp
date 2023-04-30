#include "mondrian/Mondrian.hpp"

#include <cmath>
#include <memory>
#include <numeric>
#include <utility>

#include "mondrian/FrameBuffer.hpp"
#include "mondrian/InferenceEngine.hpp"
#include "mondrian/InferencePlanner.hpp"
#include "mondrian/Interpolator.hpp"
#include "mondrian/Log.hpp"
#include "mondrian/Logger.hpp"
#include "mondrian/MixedFrame.hpp"
#include "mondrian/ROIExtractor.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/PatchReconstructor.hpp"

namespace md {

static auto printLatencyTable = [](const std::map<Device, std::map<std::tuple<int, bool>, time_us>>& latencyTable) {
  std::stringstream ss;
  for (const auto&[device, size_latency]: latencyTable) {
    for (const auto&[size_forFullFrame, latency]: size_latency) {
      assert(device != NO_DEVICE);
      auto [size, forFullFrame] = size_forFullFrame;
      ss << toConstStr(device) << " " << forFullFrame << " " << size << " " << latency << " us" << std::endl;
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

Mondrian::Mondrian(const MondrianConfig& config, std::map<int, int> startIndices,
                   JNIEnv* env, jobject app)
    : mConfig(config), mbStop(false),
      mResultLogger(new Logger("/data/data/hcs.offloading.mondrian/boxes.txt")),
      mStartIndices(std::move(startIndices)),
      mTargetSize(int(mConfig.roiExtractorConfig.EXTRACTION_RESIZE_WIDTH),
                  int(mConfig.roiExtractorConfig.EXTRACTION_RESIZE_HEIGHT)),
      mInputSizes(mConfig.inferenceEngineConfig.INPUT_SIZES),
      mScheduleInterval(mConfig.LATENCY_SLO_MS * 1000 / 2),
      mROIResizer(new ROIResizer(config.roiResizerConfig)),
      mInferenceEngine(new InferenceEngine(config.inferenceEngineConfig, env, app)),
      mPatchReconstructor(new PatchReconstructor(config.patchReconstructorConfig,
                                                 mROIResizer.get())) {
  assert(!config.USE_ROI_WISE_INFERENCE || mInputSizes.size() >= 2);
  int maxMergeSize = config.FULL_FRAME_INTERVAL == 0 ? 0 : (config.USE_EMULATED_BATCH
                                                            ? config.ROI_SIZE
                                                            : mInputSizes.front());
  bool runROIExtractor = mConfig.FULL_FRAME_INTERVAL != 0;
  auto latencyTable = mInferenceEngine->getInferenceTimeTable();
  printLatencyTable(latencyTable);
  auto inferencePlan = InferencePlanner::getInferencePlan(latencyTable, mScheduleInterval,
                                                          mConfig.USE_ROI_WISE_INFERENCE);
  auto vids = key_set(mStartIndices);
  mROIExtractor = std::make_unique<ROIExtractor>(
          mConfig.roiExtractorConfig, maxMergeSize, runROIExtractor, mROIResizer.get(),
          config.USE_EMULATED_BATCH, config.ROI_SIZE, inferencePlan, vids);

  if (config.LOG_EXECUTION) {
    mExecutionLogger = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/timeline.csv");
    mExecutionLogger->logExecutionHeader();
  }
  if (config.LOG_ROI) {
    mROILogger = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/roi.csv");
    mROILogger->logROIHeader();
  }
  mThread = std::thread([this]() { work(); });
  mResultThread = std::thread([this]() { outputWork(); });
}

Mondrian::~Mondrian() {
  mbStop = true;
  mResultsCv.notify_all();
  mThread.join();
  mResultThread.join();
}

void Mondrian::work() {
  LOGD("Mondrian::work()");
  int scheduleID = -1;

  // Wait sources for synced start
  waitForStart();
  std::this_thread::sleep_for(std::chrono::microseconds(mScheduleInterval));

  TimeLogger logger;
  logger.start();

  // When FULL_FRAME_INTERVAL == 0, always run full frame inference
  // See Mondrian::enqueueImage(...)
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
        {{mConfig.FULL_DEVICE, fullFramePlan ? latencyTable[mConfig.FULL_DEVICE][{mConfig.FULL_FRAME_SIZE, true}] : 0L}});
    logger.step("plan");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         "Mondrian::getInferencePlan", logger.getDuration("plan"), toString(inferencePlan).c_str());
    assert(!inferencePlan.empty());

    // 2. Prepare inference
    auto results = mROIExtractor->prepareInference(inferencePlan, fullFramePlan, scheduleID);
    auto&[mixedFrames, fullFrameTarget, selectedFrames, droppedFrames] = results;
    logger.step("prep");
    LOGD("%-25s took %-7lld us                            "
         "// %4lu MixedFrames with %s, %lu droppedFrames",
         "RE::prepareInference", logger.getDuration("prep"),
         mixedFrames.size(), toString(selectedFrames).c_str(), droppedFrames.size());

    // 3. Enqueue full frame
    if (fullFrameTarget != nullptr) {
      mInferenceEngine->enqueue(fullFrameTarget->rgbMat, mConfig.FULL_DEVICE, mConfig.FULL_FRAME_SIZE,
                                true, fullFrameTarget->getKey());
      fullFrameTarget->inferenceFrameSize = mConfig.FULL_FRAME_SIZE;
      fullFrameTarget->inferenceDevice = mConfig.FULL_DEVICE;
      LOGD("mInferenceEngine->enqueue %d sized fullFrame to %s | %d",
           mConfig.FULL_FRAME_SIZE, toConstStr(mConfig.FULL_DEVICE), fullFrameTarget->frameIndex);
      logger.step("full");
    } else {
      logger.step("full");
    }

    // 4. Enqueue mixed Frames
    for (const auto& mixedFrame: mixedFrames) {
      assert(mixedFrame.device != NO_DEVICE);
      mInferenceEngine->enqueue(mixedFrame.packedMat, mixedFrame.device,
                                mixedFrame.mixedFrameSize, false, mixedFrame.getKey());
      LOGD("mInferenceEngine->enqueue %d sized %d mixedFrame to %s | %s",
           mixedFrame.mixedFrameSize, mixedFrame.mixedFrameIndex, toConstStr(mixedFrame.device),
           toString(mixedFrame.getPackedFrames()).c_str());
    }

    // 5. Handle full frame inference results
    if (fullFrameTarget != nullptr) {
      handleFullFrameInferenceResults(fullFrameTarget);
      LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
           "Mondrian::fullFrameInference", logger.getDuration("full"),
           fullFrameTarget->vid, fullFrameTarget->frameIndex);
    }

    // 6. Handle mixed frame or ROI-wise inference results
    if (mConfig.USE_ROI_WISE_INFERENCE) {
      handleROIWiseInferenceResults(mixedFrames);
    } else {
      handleMixedFrameInferenceResults(mixedFrames);
    }
    logger.step("inf");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         mConfig.USE_ROI_WISE_INFERENCE ? "Mondrian::handleROIWiseInferenceResults"
                                        : "Mondrian::handleMixedFrameInferenceResults",
         logger.getDuration("inf"), toString(inferencePlan).c_str());

    // 7. Interpolate results
    Interpolator::interpolate(selectedFrames, mConfig.INTERPOLATION_THRESHOLD);
    logger.step("itp");
    LOGD("%-25s took %-7lld us", "Interpolator::interpolate", logger.getDuration("itp"));

    // 8. Notify results of rest of the frames
    for (auto& it: selectedFrames) {
      for (Frame* frame: it.second) {
        for (auto& cROI: frame->childROIs) {
          if (cROI->box == nullptr) {
            continue;
          }
          assert(cROI->box->srcROI == cROI.get());
          assert(cROI->box->id == cROI->id);
          assert(cROI->box->label == cROI->label);
        }
        frame->isBoxesReady = true;
        frame->endTime = NowMicros();
      }
    }
    mROIExtractor->notify();

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

void Mondrian::handleFullFrameInferenceResults(Frame* frame) {
  auto[boxes, times, device] = mInferenceEngine->getResults(frame->getKey());
  assert(device == mConfig.FULL_DEVICE);
  frame->fullInferenceStartTime = times.first;
  frame->fullInferenceEndTime = times.second;
  for (const BoundingBox& box: boxes) {
    auto& loc = box.location;
    frame->boxes.push_back(std::make_unique<BoundingBox>(
        UNASSIGNED_ID, box.location, box.confidence, box.label, origin_FF));
  }
  mPatchReconstructor->matchBoxesWithChildROIs(frame, true);

  for (auto& box: frame->boxes) {
    assert(box->id != UNASSIGNED_ID);
  }
  frame->isBoxesReady = true;
  frame->endTime = NowMicros();
  if (mROIExtractor != nullptr) {
    mROIExtractor->notify();
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

void Mondrian::handleMixedFrameInferenceResults(std::vector<MixedFrame>& mixedFrames) {
  // Get results of mixed frames sequentially
  for (int i = 0; i < mixedFrames.size(); i++) {
    auto[boxes, times, device] = mInferenceEngine->getResults(mixedFrames[i].getKey());
    assert(device == mixedFrames[i].device);
    LOGD("mInferenceEngine->getResults %d sized %d mixedFrame to %s", mixedFrames[i].mixedFrameSize,
         mixedFrames[i].mixedFrameIndex, toConstStr(mixedFrames[i].device));
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

    for (Frame* frame: mixedFrames[i].getPackedFrames()) {
      if (frame->isReadyToMarry(i)) { // If all pROIs packed and inference ended
        // Match boxes with ROIs (per frame)
        nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
        mPatchReconstructor->matchBoxesWithChildROIs(frame, false);
        frame->isBoxesReady = true;
      }
    }
    // Notify results of processed frames
    mROIExtractor->notify();
  }
}

void Mondrian::handleROIWiseInferenceResults(std::vector<MixedFrame>& mixedFrames) {
  Stream inferenceFrames;
  for (auto& mixedFrame: mixedFrames) {
    auto[boxes, times, device] = mInferenceEngine->getResults(mixedFrame.getKey());
    assert(mixedFrame.packedROIs.size() == 1);
    assert(device == mixedFrame.device);
    auto[x, y] = (*mixedFrame.packedROIs.begin())->getPackedXY();
    ROI* pROI = *mixedFrame.packedROIs.begin();
    if (pROI->frame->inferenceDevice == NO_DEVICE) {
      pROI->frame->inferenceFrameSize = mixedFrame.mixedFrameSize;
      pROI->frame->inferenceDevice = device;
      pROI->frame->mixedInferenceStartTime = times.first;
      pROI->frame->mixedInferenceEndTime = times.second;
    }
    inferenceFrames.insert(pROI->frame);
    for (BoundingBox& b: boxes) {
      pROI->frame->boxes.push_back(std::make_unique<BoundingBox>(
          UNASSIGNED_ID, Rect(
              (b.location.l - x) / pROI->getTargetScale() + pROI->origLoc.l,
              (b.location.t - y) / pROI->getTargetScale() + pROI->origLoc.t,
              (b.location.r - x) / pROI->getTargetScale() + pROI->origLoc.l,
              (b.location.b - y) / pROI->getTargetScale() + pROI->origLoc.t),
          b.confidence, b.label, pROI->origin));
    }
  }

  for (Frame* frame: inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, mPatchReconstructor->getIoUThreshold());
    mPatchReconstructor->matchBoxesWithChildROIs(frame, false);
  }
}

void Mondrian::releaseFrames(const MultiStream& frames) {
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
    for (int i = 0; i < mConfig.roiExtractorConfig.PD_INTERVAL; i++) {
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

void Mondrian::log(const Frame* frame) {
  if (mExecutionLogger) {
    mExecutionLogger->logExecution(frame);
  }
  if (mROILogger) {
    for (auto& cROI: frame->childROIs) {
      mROILogger->logROI(cROI.get());
    }
  }
}

void Mondrian::waitForStart() {
  std::unique_lock<std::mutex> startLock(mStartMtx);
  mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mStartIndices.size(); });
  mbStartEnqueue = true;
  startLock.unlock();
  mEnqueueCv.notify_all();
}

int Mondrian::enqueueImage(const int vid, const cv::Mat& yuvMat) {
  assert(!yuvMat.empty());

  std::unique_lock<std::mutex> fairLock(mFairEnqueueMtx);
  mFairCv.wait(fairLock, [vid, this]() {
    return mStartIndices.size() == 1 || mPrevEnqueuedVid != vid;
  });
  mPrevEnqueuedVid = vid;
  fairLock.unlock();
  mFairCv.notify_all();

  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(vid) == mFrameBuffers.end()) {
    mFrameBuffers[vid] = std::make_unique<FrameBuffer>(
        vid, mConfig.BUFFER_SIZE, mStartIndices[vid]);
  }
  FrameBuffer* frameBuffer = mFrameBuffers.at(vid).get();
  lock.unlock();

  cv::Mat rgbMat;
  cv::cvtColor(yuvMat, rgbMat, cv::COLOR_YUV2RGB_NV12, 3);
  Frame* frame = frameBuffer->enqueue(rgbMat);
  time_us startTime = NowMicros();
  preprocess(frame);
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
       "Mondrian::preprocess", NowMicros() - startTime, frame->vid, frame->frameIndex);
  if (mConfig.FULL_FRAME_INTERVAL == 0 || frame->frameIndex == mStartIndices[vid]) {
    frame->useInferenceResultForOF = true;
    mInferenceEngine->enqueue(frame->rgbMat, mConfig.FULL_DEVICE, mConfig.FULL_FRAME_SIZE, true,
                              frame->getKey());
    frame->inferenceFrameSize = mConfig.FULL_FRAME_SIZE;
    frame->inferenceDevice = mConfig.FULL_DEVICE;
    LOGD("mInferenceEngine->enqueue %d sized fullFrame to %s | %d",
         mConfig.FULL_FRAME_SIZE, toConstStr(mConfig.FULL_DEVICE), frame->frameIndex);
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
    mROIExtractor->enqueue(frame);
  }
  return frame->frameIndex;
}

void Mondrian::preprocess(Frame* frame) const {
  cv::resize(frame->rgbMat, frame->resizedGrayMat, mTargetSize, 0, 0, CV_INTER_LINEAR);
  cv::cvtColor(frame->resizedGrayMat, frame->resizedGrayMat, cv::COLOR_RGB2GRAY);
}

void Mondrian::outputWork() {
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

} // namespace md
