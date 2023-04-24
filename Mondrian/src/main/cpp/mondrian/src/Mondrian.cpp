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
    : config_(config), stop_(false),
      resultLogger_(new Logger("/data/data/hcs.offloading.mondrian/boxes.txt")),
      startIndices_(std::move(startIndices)),
      targetSize_(int(config_.roiExtractorConfig.EXTRACTION_RESIZE_WIDTH),
                  int(config_.roiExtractorConfig.EXTRACTION_RESIZE_HEIGHT)),
      inputSizes_(config_.inferenceEngineConfig.INPUT_SIZES),
      scheduleInterval_(config_.LATENCY_SLO_MS * 1000 / 2),
      ROIResizer_(new ROIResizer(config.roiResizerConfig)),
      inferenceEngine_(new InferenceEngine(config.inferenceEngineConfig, env, app)),
      patchReconstructor_(new PatchReconstructor(config.patchReconstructorConfig,
                                                 ROIResizer_.get())) {
  assert(!config.USE_ROI_WISE_INFERENCE || inputSizes_.size() >= 2);
  int maxMergeSize = config.FULL_FRAME_INTERVAL == 0 ? 0 : (config.USE_EMULATED_BATCH
                                                            ? config.ROI_SIZE
                                                            : inputSizes_.front());
  bool runROIExtractor = config_.FULL_FRAME_INTERVAL != 0;
  auto latencyTable = inferenceEngine_->getInferenceTimeTable();
  printLatencyTable(latencyTable);
  auto inferencePlan = InferencePlanner::getInferencePlan(latencyTable, scheduleInterval_,
                                                          config_.USE_ROI_WISE_INFERENCE);
  auto vids = key_set(startIndices_);
  ROIExtractor_ = std::make_unique<ROIExtractor>(
      config_.roiExtractorConfig, maxMergeSize, runROIExtractor, ROIResizer_.get(),
      config.USE_EMULATED_BATCH, config.ROI_SIZE, inferencePlan, vids);

  if (config.LOG_EXECUTION) {
    executionLogger_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/timeline.csv");
    executionLogger_->logExecutionHeader();
  }
  if (config.LOG_ROI) {
    ROILogger_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/roi.csv");
    ROILogger_->logROIHeader();
  }
  thread_ = std::thread([this]() { work(); });
  resultThread_ = std::thread([this]() { outputWork(); });
}

Mondrian::~Mondrian() {
  stop_ = true;
  resultsCV_.notify_all();
  thread_.join();
  resultThread_.join();
}

void Mondrian::work() {
  LOGD("Mondrian::work()");
  int scheduleID = -1;

  // Wait sources for synced start
  waitForStart();
  std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval_));

  TimeLogger logger;
  logger.start();

  // When FULL_FRAME_INTERVAL == 0, always run full frame inference
  // See Mondrian::enqueueImage(...)
  while (!stop_ && config_.FULL_FRAME_INTERVAL > 0) {
    scheduleID++;
    bool fullFramePlan = scheduleID % config_.FULL_FRAME_INTERVAL == 0;

    // Wait for scheduling interval
    time_us elapsedTime = logger.getElapsedTime();
    if (config_.ALLOW_INTERPOLATION && scheduleInterval_ > elapsedTime) {
      std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval_ - elapsedTime));
    }

    LOGD("========== Start ==========");
    // 1. Inference planning
    logger.start();
    auto latencyTable = inferenceEngine_->getInferenceTimeTable();
    std::vector<InferenceInfo> inferencePlan = InferencePlanner::getInferencePlan(
        latencyTable, scheduleInterval_, config_.USE_ROI_WISE_INFERENCE,
        {{config_.FULL_DEVICE, fullFramePlan ? latencyTable[config_.FULL_DEVICE][{config_.FULL_FRAME_SIZE, true}] : 0L}});
    logger.step("plan");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         "Mondrian::getInferencePlan", logger.getDuration("plan"), toString(inferencePlan).c_str());
    assert(!inferencePlan.empty());

    // 2. Prepare inference
    auto results = ROIExtractor_->prepareInference(inferencePlan, fullFramePlan, scheduleID);
    auto&[mixedFrames, fullFrameTarget, selectedFrames, droppedFrames] = results;
    logger.step("prep");
    LOGD("%-25s took %-7lld us                            "
         "// %4lu MixedFrames with %s, %lu droppedFrames",
         "RE::prepareInference", logger.getDuration("prep"),
         mixedFrames.size(), toString(selectedFrames).c_str(), droppedFrames.size());

    // 3. Enqueue full frame
    if (fullFrameTarget != nullptr) {
      inferenceEngine_->enqueue(fullFrameTarget->rgbMat, config_.FULL_DEVICE, config_.FULL_FRAME_SIZE,
                                true, fullFrameTarget->getKey());
      fullFrameTarget->inferenceFrameSize = config_.FULL_FRAME_SIZE;
      fullFrameTarget->inferenceDevice = config_.FULL_DEVICE;
      LOGD("inferenceEngine_->enqueue %d sized fullFrame to %s | %d",
           config_.FULL_FRAME_SIZE, toConstStr(config_.FULL_DEVICE), fullFrameTarget->frameIndex);
      logger.step("full");
    } else {
      logger.step("full");
    }

    // 4. Enqueue mixed Frames
    for (const auto& mixedFrame: mixedFrames) {
      assert(mixedFrame.device != NO_DEVICE);
      inferenceEngine_->enqueue(mixedFrame.packedMat, mixedFrame.device,
                                mixedFrame.mixedFrameSize, false, mixedFrame.getKey());
      LOGD("inferenceEngine_->enqueue %d sized %d mixedFrame to %s | %s",
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
    if (config_.USE_ROI_WISE_INFERENCE) {
      handleROIWiseInferenceResults(mixedFrames);
    } else {
      handleMixedFrameInferenceResults(mixedFrames);
    }
    logger.step("inf");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         config_.USE_ROI_WISE_INFERENCE ? "Mondrian::handleROIWiseInferenceResults"
                                        : "Mondrian::handleMixedFrameInferenceResults",
         logger.getDuration("inf"), toString(inferencePlan).c_str());

    // 7. Interpolate results
    Interpolator::interpolate(selectedFrames, config_.INTERPOLATION_THRESHOLD);
    logger.step("itp");
    LOGD("%-25s took %-7lld us", "Interpolator::interpolate", logger.getDuration("itp"));

    // 8. Notify results of rest of the frames
    for (auto& it: selectedFrames) {
      for (Frame* frame: it.second) {
        for (auto& cROI: frame->rois) {
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
    ROIExtractor_->notify();

    // 9. Update results for system output
    std::unique_lock<std::mutex> resultLock(resultsMtx_);
    for (const auto& it: selectedFrames) {
      for (Frame* frame: it.second) {
        if (frame != fullFrameTarget) {
          std::vector<BoundingBox> boxes;
          std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(boxes),
                         [](const std::unique_ptr<BoundingBox>& box) { return *box; });
          results_[frame->vid][frame->frameIndex] = {
              frame->endTime, nms(boxes, NUM_LABELS, patchReconstructor_->getIoUThreshold())};
        }
      }
    }
    resultLock.unlock();
    resultsCV_.notify_all();

    // 10. Release used frames
    releaseFrames(selectedFrames);
    logger.step("post");

    LOGD("Latency breakdown: %s", logger.getLog().c_str());
    LOGD("==========  End  ==========");
  }
}

void Mondrian::handleFullFrameInferenceResults(Frame* frame) {
  auto[boxes, times, device] = inferenceEngine_->getResults(frame->getKey());
  assert(device == config_.FULL_DEVICE);
  frame->fullInferenceStartTime = times.first;
  frame->fullInferenceEndTime = times.second;
  for (const BoundingBox& box: boxes) {
    auto& loc = box.location;
    frame->boxes.push_back(std::make_unique<BoundingBox>(
        UNASSIGNED_ID, box.location, box.confidence, box.label, O_FULL_FRAME));
  }
  patchReconstructor_->matchBoxesWithChildROIs(frame, true);

  for (auto& box: frame->boxes) {
    assert(box->id != UNASSIGNED_ID);
  }
  frame->isBoxesReady = true;
  frame->endTime = NowMicros();
  if (ROIExtractor_ != nullptr) {
    ROIExtractor_->notify();
  }

  log(frame);
  assert(std::all_of(frame->boxes.begin(), frame->boxes.end(),
                     [](const std::unique_ptr<BoundingBox>& box) { return box->label == 0; }));

  std::unique_lock<std::mutex> resultLock(resultsMtx_);
  std::vector<BoundingBox> resultBoxes;
  std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(resultBoxes),
                 [](const std::unique_ptr<BoundingBox>& box) { return *box; });
  results_[frame->vid][frame->frameIndex] = {NowMicros(), std::move(resultBoxes)};
  resultLock.unlock();
  resultsCV_.notify_all();
}

void Mondrian::handleMixedFrameInferenceResults(std::vector<MixedFrame>& mixedFrames) {
  // Get results of mixed frames sequentially
  for (int i = 0; i < mixedFrames.size(); i++) {
    auto[boxes, times, device] = inferenceEngine_->getResults(mixedFrames[i].getKey());
    assert(device == mixedFrames[i].device);
    LOGD("inferenceEngine_->getResults %d sized %d mixedFrame to %s", mixedFrames[i].mixedFrameSize,
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
    patchReconstructor_->assignBoxesToFrame(mixedFrames[i], boxes);

    for (Frame* frame: mixedFrames[i].getPackedFrames()) {
      if (frame->isReadyToMarry(i)) { // If all pROIs packed and inference ended
        // Match boxes with ROIs (per frame)
        nms(frame->boxes, NUM_LABELS, patchReconstructor_->getIoUThreshold());
        patchReconstructor_->matchBoxesWithChildROIs(frame, false);
        frame->isBoxesReady = true;
      }
    }
    // Notify results of processed frames
    ROIExtractor_->notify();
  }
}

void Mondrian::handleROIWiseInferenceResults(std::vector<MixedFrame>& mixedFrames) {
  Stream inferenceFrames;
  for (auto& mixedFrame: mixedFrames) {
    auto[boxes, times, device] = inferenceEngine_->getResults(mixedFrame.getKey());
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
    nms(frame->boxes, NUM_LABELS, patchReconstructor_->getIoUThreshold());
    patchReconstructor_->matchBoxesWithChildROIs(frame, false);
  }
}

void Mondrian::releaseFrames(const MultiStream& frames) {
  std::unique_lock<std::mutex> framesLock(frameBuffersMtx_);
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
    for (int i = 0; i < config_.roiExtractorConfig.PD_INTERVAL; i++) {
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
      frameBuffers_.at(vid)->freeImage(freeFrameIndices);
    }
  }
  framesLock.unlock();
}

void Mondrian::log(const Frame* frame) {
  if (executionLogger_) {
    executionLogger_->logExecution(frame);
  }
  if (ROILogger_) {
    for (auto& cROI: frame->childROIs) {
      ROILogger_->logROI(cROI.get());
    }
  }
}

void Mondrian::waitForStart() {
  std::unique_lock<std::mutex> startLock(startMtx_);
  startCV_.wait(startLock, [this]() { return numStartedFrameBuffers_ == startIndices_.size(); });
  startEnqueue_ = true;
  startLock.unlock();
  enqueueCV_.notify_all();
}

int Mondrian::enqueueImage(const int vid, const cv::Mat& yuvMat) {
  assert(!yuvMat.empty());

  std::unique_lock<std::mutex> fairLock(fairEnqueueMtx_);
  fairCV_.wait(fairLock, [vid, this]() {
    return startIndices_.size() == 1 || prevEnqueuedVid_ != vid;
  });
  prevEnqueuedVid_ = vid;
  fairLock.unlock();
  fairCV_.notify_all();

  std::unique_lock<std::mutex> lock(frameBuffersMtx_);
  if (frameBuffers_.find(vid) == frameBuffers_.end()) {
    frameBuffers_[vid] = std::make_unique<FrameBuffer>(
        vid, config_.BUFFER_SIZE, startIndices_[vid]);
  }
  FrameBuffer* frameBuffer = frameBuffers_.at(vid).get();
  lock.unlock();

  cv::Mat rgbMat;
  cv::cvtColor(yuvMat, rgbMat, cv::COLOR_YUV2RGB_NV12, 3);
  Frame* frame = frameBuffer->enqueue(rgbMat);
  time_us startTime = NowMicros();
  preprocess(frame);
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
       "Mondrian::preprocess", NowMicros() - startTime, frame->vid, frame->frameIndex);
  if (config_.FULL_FRAME_INTERVAL == 0 || frame->frameIndex == startIndices_[vid]) {
    frame->useInferenceResultForOF = true;
    inferenceEngine_->enqueue(frame->rgbMat, config_.FULL_DEVICE, config_.FULL_FRAME_SIZE, true,
                              frame->getKey());
    frame->inferenceFrameSize = config_.FULL_FRAME_SIZE;
    frame->inferenceDevice = config_.FULL_DEVICE;
    LOGD("inferenceEngine_->enqueue %d sized fullFrame to %s | %d",
         config_.FULL_FRAME_SIZE, toConstStr(config_.FULL_DEVICE), frame->frameIndex);
    handleFullFrameInferenceResults(frame);
    if (config_.FULL_FRAME_INTERVAL == 0) {
      std::lock_guard<std::mutex> framesLock(frameBuffersMtx_);
      log(frame);
      frameBuffers_.at(frame->vid)->freeImage({frame->frameIndex});
    }
  } else {
    if (frame->frameIndex == startIndices_[vid] + 1) {
      std::unique_lock<std::mutex> startLock(startMtx_);
      numStartedFrameBuffers_++;
      startCV_.notify_one();
      enqueueCV_.wait(startLock, [this]() { return startEnqueue_; });
      startLock.unlock();
      LOGD("Start %d video at %lld us", vid, NowMicros());
    }
    ROIExtractor_->enqueue(frame);
  }
  return frame->frameIndex;
}

void Mondrian::preprocess(Frame* frame) const {
  cv::resize(frame->rgbMat, frame->resizedGrayMat, targetSize_, 0, 0, CV_INTER_LINEAR);
  cv::cvtColor(frame->resizedGrayMat, frame->resizedGrayMat, cv::COLOR_RGB2GRAY);
}

void Mondrian::outputWork() {
  while (!stop_) {
    std::unique_lock<std::mutex> resultLock(resultsMtx_);
    resultsCV_.wait(resultLock, [this]() {
      if (stop_) {
        return true;
      }
      for (auto& streamIt: results_) {
        int vid = streamIt.first;
        if (resultIndices_.find(vid) == resultIndices_.end()) {
          resultIndices_[vid] = startIndices_[vid];
        }
        int frameIndex = resultIndices_[vid];
        if (streamIt.second.find(frameIndex) != streamIt.second.end()) {
          return true;
        }
      }
      return false;
    });
    int vid;
    int frameIndex;
    FrameResult result;
    for (auto& streamIt: results_) {
      vid = streamIt.first;
      frameIndex = resultIndices_[vid];
      auto frameIt = streamIt.second.find(frameIndex);
      if (frameIt != streamIt.second.end()) {
        resultIndices_[vid]++;
        result = frameIt->second;
        streamIt.second.erase(frameIt);
        break;
      }
    }
    resultLock.unlock();
    resultsCV_.notify_all();

    auto&[time, boxes] = result;
    LOGD("Logger::logResult                         for video %-5d frame %-4d // %4lu boxes",
         vid, frameIndex, boxes.size());
    resultLogger_->logResult(vid, frameIndex, time, boxes);
  }
}

} // namespace md
