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
#include "mondrian/PackedCanvas.hpp"
#include "mondrian/PatchReconstructor.hpp"
#include "mondrian/ROI.hpp"
#include "mondrian/ROIExtractor.hpp"
#include "mondrian/ROIPacker.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Utils.hpp"

namespace md {

Mondrian::Mondrian(const MondrianConfig& config, int numVideos, JNIEnv* env, jobject app)
    : config_(config),
      startTime_(NowMicros()),
      numFirstFrameReadyVideos_(0),
      numVideos_(numVideos),
      scheduleInterval_(config_.LATENCY_SLO_MS * 1000 / 2),
      planningTime_(0),
      numIntervals_(0),
      stop_(false),
      ROIResizer_(new ROIResizer(config.roiResizerConfig)),
      inferenceEngine_(new InferenceEngine(config.inferenceEngineConfig, env, app)) {
  ROIPacker_ = std::make_unique<ROIPacker>(
      config.roiPackerConfig, config.EXECUTION_TYPE, config.ROI_SIZE, ROIResizer_.get());
  patchReconstructor_ = std::make_unique<PatchReconstructor>(
      config.patchReconstructorConfig, ROIResizer_.get());
  config_.print();
  config_.test();

  // Create loggers
  if (config.LOG_FRAME) {
    loggerFrame_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/frame.csv");
    loggerFrame_->logFrameHeader();
  }
  if (config.LOG_ROI) {
    loggerROI_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/roi.csv");
    loggerROI_->logROIHeader();
  }
  if (config.LOG_MERGED_ROI) {
    loggerMergedROI_ = std::make_unique<Logger>(
        "/data/data/hcs.offloading.mondrian/merged_roi.csv");
    loggerMergedROI_->logMergedROIHeader();
  }
  if (config.LOG_BOXES) {
    loggerBoxes_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/boxes.csv");
    loggerBoxes_->logBoxesHeader();
  }

  // Prepare frame buffers
  for (VID vid = 0; vid < numVideos; vid++) {
    frameBuffers_[vid] = std::make_unique<FrameBuffer>(vid);
  }

  // Start logging thread
  logThread_ = std::thread([this]() { workLog(); });

  // If frame-wise inference, skip ROI extraction and scheduling
  if (config_.EXECUTION_TYPE == ExecutionType::FRAME_WISE_INFERENCE) return;

  // Latency profiling
  inferenceEngine_->profileLatency();

  // Start ROI extraction threads
  ROIExtractor_ = std::make_unique<ROIExtractor>(config_.roiExtractorConfig, ROIResizer_.get());

  // Start postprocessing thread
  postprocessThread_ = std::thread([this]() { workPostprocess(); });

  // Start scheduling thread
  scheduleThread_ = std::thread([this]() { workSchedule(); });
}

Mondrian::~Mondrian() {
  stop_ = true;
  resultsCV_.notify_all();
  scheduleThread_.join();
  postprocessThread_.join();
  logThread_.join();
}

void Mondrian::workSchedule() {
  {
    std::unique_lock<std::mutex> startLock(startMtx_);
    startCV_.wait(startLock, [this]() {
      assert(numFirstFrameReadyVideos_ <= numVideos_);
      return numFirstFrameReadyVideos_ == numVideos_;
    });
  }
  std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval_));
  while (!stop_) {
    time_us scheduleStart = NowMicros();
    int currID = numIntervals_++;
    LOGD("[Schedule %d] ========== Start at %lld ==========", currID, NowMicros() - startTime_);

    // Getting PackedFrames
    MultiStream streams = ROIExtractor_->collectFrames(currID);
    LOGD("[Schedule %d] Collect Frames // %s", currID, str(streams).c_str());

    // Prepare & Enqueue Full frame
    Frame* fullFrameTarget = nullptr;
    int fullFrameVid = (currID + 1) % config_.FULL_FRAME_INTERVAL == 0
                       ? ((currID + 1) / config_.FULL_FRAME_INTERVAL) % numVideos_
                       : -1;
    if (streams.find(fullFrameVid) != streams.end()) {
      fullFrameTarget = *streams.at(fullFrameVid).rbegin();
      inferenceEngine_->enqueue(
          /*rgbMat=*/fullFrameTarget->rgbMat(),
          /*device=*/config_.inferenceEngineConfig.FULL_DEVICE,
          /*inputSize=*/config_.inferenceEngineConfig.FULL_FRAME_SIZE,
          /*isFullFrame=*/true,
          /*key=*/fullFrameTarget->getKey());
      LOGD("[Schedule %d] Full Frame vid=%d fid=%d", currID,
           fullFrameTarget->vid, fullFrameTarget->fid);
    } else {
      LOGD("[Schedule %d] NO Full Frame", currID);
    }

    // Inference planning
    LatencyTable latencyTable = inferenceEngine_->latencyTable();
    std::map<Device, time_us> remainingTimes = inferenceEngine_->remainingTimes();

    time_us planStart = NowMicros();
    std::map<Device, time_us> remainingTimesAfterPlanning;
    for (auto& [device, remainingTime] : remainingTimes) {
      remainingTimesAfterPlanning[device] = std::max(remainingTime - planningTime_, 0LL);
    }
    std::vector<InferenceInfo> inferencePlan = InferencePlanner::getInferencePlan(
        /*latencyTable=*/latencyTable,
        /*interval=*/scheduleInterval_,
        /*roiWiseInference=*/config_.EXECUTION_TYPE == ExecutionType::ROI_WISE_INFERENCE,
        /*startTimes=*/remainingTimesAfterPlanning);
    assert(!inferencePlan.empty());
    LOGD("[Schedule %d] FullFid=%d | RemainingTimes %s | PlanningTime %lld | LatencyTable %s",
         currID,
         fullFrameTarget != nullptr ? fullFrameTarget->fid : -1,
         str(remainingTimes).c_str(),
         planningTime_,
         str(latencyTable).c_str());
    LOGD("[Schedule %d] InferencePlan %s", currID, str(inferencePlan).c_str());

    // Prepare Packed Canvases
    std::map<Device, std::vector<PackedCanvas>> packedCanvasesTable = ROIPacker_->packCanvases(
        /*currID=*/currID,
        /*streams=*/streams,
        /*inferencePlan=*/inferencePlan,
        /*fullFrameTarget=*/fullFrameTarget);

    std::unique_lock<std::mutex> packingResultsLock(packingResultsMtx_);
    packingResults_.push({streams, fullFrameTarget, packedCanvasesTable});
    packingResultsLock.unlock();
    packingResultsCV_.notify_one();

    time_us planEnd = NowMicros();
    planningTime_ = (7 * (planEnd - planStart) + 3 * planningTime_) / 10;

    LOGD("[Schedule %d] ========== End   at %lld ==========", currID, NowMicros() - startTime_);

    // Wait for scheduling interval
    time_us sleepTime = scheduleInterval_ - (NowMicros() - scheduleStart);
    if (sleepTime > 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
    }
  }
}

void Mondrian::handleFullFrameResults(Frame* frame, int currID) {
  Result result = inferenceEngine_->getResults(frame->getKey());
  LOGD("[Schedule %d] FULL Inference at %lld // vid=%d fid=%d",
       currID, NowMicros() - startTime_, frame->vid, frame->fid);
  frame->inferenceFrameSize = config_.inferenceEngineConfig.FULL_FRAME_SIZE;
  frame->deviceIfFullFrame = config_.inferenceEngineConfig.FULL_DEVICE;
  frame->fullInferenceStartTime = result.detectionStart;
  frame->fullInferenceEndTime = result.detectionEnd;
  for (const BoundingBox& box : result.boxes) {
    auto& loc = box.loc;
    frame->boxes.push_back(std::make_unique<BoundingBox>(
        INVALID_OID, -1, box.loc, box.confidence, box.label, Origin::FULL_FRAME));
  }
  if (config_.EXECUTION_TYPE != ExecutionType::FRAME_WISE_INFERENCE) {
    patchReconstructor_->matchBoxesROIs(frame, true);

    for (auto& box : frame->boxes) {
      assert(box->oid != INVALID_OID);
    }
    frame->isBoxesReady = true;
    ROIExtractor_->cv().notify_all();
  }

  frame->endTime = NowMicros();

  std::unique_lock<std::mutex> resultLock(logMtx_);
  std::vector<BoundingBox> resultBoxes;
  std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(resultBoxes),
                 [](const std::unique_ptr<BoundingBox>& box) { return *box; });
  results_[frame->vid][frame->fid] = {frame->endTime, std::move(resultBoxes)};
  resultLock.unlock();
  resultsCV_.notify_all();
}

void Mondrian::handlePackedCanvasesResults(std::map<Device,
                                                    std::vector<PackedCanvas>>& packedCanvasesTable, int currID) {
  // Get results of packed canvases sequentially


  for (auto& it : packedCanvasesTable) {
    Device device = it.first;
    auto& packedCanvases = it.second;
    for (int i = 0; i < packedCanvases.size(); i++) {
      Result result = inferenceEngine_->getResults(packedCanvases[i].getKey());
      packedCanvases[i].packedMat.release();
      const int inputSize = packedCanvases[i].packedCanvasSize;
      LOGD("[Schedule %d] Pack Inference on %s at %lld // %s",
           currID, str(result.device).c_str(), NowMicros() - startTime_,
           str(packedCanvases[i].getPackedFrames()).c_str());
      assert(result.device == packedCanvases[i].device);
      for (Frame* frame : packedCanvases[i].getPackedFrames()) {
        if (frame->deviceIfFullFrame == Device::INVALID) {
          frame->inferenceFrameSize = inputSize;
          frame->packedInferenceStartTime = result.detectionStart;
          frame->packedInferenceEndTime = result.detectionEnd;
        }
      }
      patchReconstructor_->assignBoxesToFrame(packedCanvases[i], result.boxes);

      for (Frame* frame : packedCanvases[i].getPackedFrames()) {
        if (frame->isReadyToMarry(i)) { // If all mergedROIs are packed and inferenced
          // Match boxes with ROIs (per frame)
          nms(frame->boxes, NUM_LABELS, patchReconstructor_->iouThres());
          patchReconstructor_->matchBoxesROIs(frame, false);
          frame->isBoxesReady = true;
        }
      }
      // Notify results of processed frames
      ROIExtractor_->cv().notify_all();
    }
  }
}

void Mondrian::handleROIWiseResults(std::map<Device, std::vector<PackedCanvas>>& packedCanvasesTable) {
  Stream inferenceFrames;

  for (auto& it : packedCanvasesTable) {
    Device device = it.first;
    auto& packedCanvases = it.second;
    for (auto& packedCanvas : packedCanvases) {
      Result result = inferenceEngine_->getResults(packedCanvas.getKey());
      packedCanvas.packedMat.release();
      assert(packedCanvas.packedROIs.size() == 1);
      assert(result.device == packedCanvas.device);
      auto [x, y] = (*packedCanvas.packedROIs.begin())->packedXY();
      MergedROI* mergedROI = *packedCanvas.packedROIs.begin();
      if (mergedROI->frame()->deviceIfFullFrame == Device::INVALID) {
        mergedROI->frame()->inferenceFrameSize = packedCanvas.packedCanvasSize;
        mergedROI->frame()->packedInferenceStartTime = result.detectionStart;
        mergedROI->frame()->packedInferenceEndTime = result.detectionEnd;
      }
      inferenceFrames.insert(mergedROI->frame());
      const float mergedScale = mergedROI->targetScale();
      for (BoundingBox& b : result.boxes) {
        float newL = std::max(0.0f, (b.loc.l - float(x)) / mergedScale + mergedROI->loc().l);
        float newT = std::max(0.0f, (b.loc.t - float(y)) / mergedScale + mergedROI->loc().t);
        float newR = std::max(0.0f, (b.loc.r - float(x)) / mergedScale + mergedROI->loc().l);
        float newB = std::max(0.0f, (b.loc.b - float(y)) / mergedScale + mergedROI->loc().t);
        assert(0 <= newL && 0 <= newT && newL <= newR && newT <= newB);
        mergedROI->frame()->boxes.push_back(std::make_unique<BoundingBox>(
            INVALID_OID,
            packedCanvas.pid,
            Rect(newL, newT, newR, newB),
            b.confidence,
            b.label,
            Origin::FULL_FRAME));
      }
    }
  }

  for (Frame* frame : inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, patchReconstructor_->iouThres());
    patchReconstructor_->matchBoxesROIs(frame, false);
  }
}

void Mondrian::releaseFrames(const MultiStream& streams) {
  for (const auto& [vid, stream] : streams) {
    if (stream.empty()) continue;
    for (Frame* frame : stream) {
      log(frame);
    }
    int lastFrameIndex = (*stream.rbegin())->fid;
    frameBuffers_.at(vid)->free(lastFrameIndex - config_.roiExtractorConfig.PD_INTERVAL);
  }
}

void Mondrian::log(const Frame* frame) {
  if (loggerFrame_) {
    loggerFrame_->logFrame(frame);
  }
  if (loggerROI_) {
    for (const auto& roi : frame->rois) {
      loggerROI_->logROI(roi.get());
    }
  }
  if (loggerMergedROI_) {
    for (const auto& mergedROI : frame->mergedROIs) {
      loggerMergedROI_->logMergedROI(mergedROI.get());
    }
    if (frame->probingROIsTable.find(Device::GPU) != frame->probingROIsTable.end()) {
      for (const auto& probingROI : frame->probingROIsTable.at(Device::GPU)) {
        loggerMergedROI_->logMergedROI(probingROI.get());
      }
    }
  }
}

void Mondrian::enqueue(const VID vid, const cv::Mat& yuvMat) {
  assert(!yuvMat.empty());

  Frame* frame = frameBuffers_.at(vid)->enqueue(yuvMat);
  frame->prepareResizedGrayMat(config_.roiExtractorConfig.EXTRACTION_SIZE);
  if (frame->fid == 1) {
    std::unique_lock<std::mutex> startLock(startMtx_);
    startCV_.wait(startLock, [this]() {
      assert(numFirstFrameReadyVideos_ <= numVideos_);
      return numFirstFrameReadyVideos_ == numVideos_;
    });
  }

  if (config_.EXECUTION_TYPE == ExecutionType::FRAME_WISE_INFERENCE) {
    enqueueFrameWise(frame);
  } else {
    enqueue(frame);
  }

  // Notify when a video is ready
  if (frame->fid == 0) {
    std::lock_guard<std::mutex> startLock(startMtx_);
    numFirstFrameReadyVideos_++;
    startCV_.notify_all();
  }
}

void Mondrian::enqueueFrameWise(Frame* frame) {
  inferenceEngine_->enqueue(
      /*rgbMat=*/frame->rgbMat(),
      /*device=*/config_.inferenceEngineConfig.FULL_DEVICE,
      /*inputSize=*/config_.inferenceEngineConfig.FULL_FRAME_SIZE,
      /*isFullFrame=*/true,
      /*key=*/frame->getKey());
  handleFullFrameResults(frame, frame->fid);
  log(frame);
  std::lock_guard<std::mutex> framesLock(frameBuffersMtx_);
  frameBuffers_.at(frame->vid)->free(frame->fid);
}

void Mondrian::enqueue(Frame* frame) {
  // Handle the first frame
  if (frame->fid == 0) {
    frame->useInferenceResultForOF = true;
    inferenceEngine_->enqueue(
        /*rgbMat=*/frame->rgbMat(),
        /*device=*/config_.inferenceEngineConfig.FULL_DEVICE,
        /*inputSize=*/config_.inferenceEngineConfig.FULL_FRAME_SIZE,
        /*isFullFrame=*/true,
        /*key=*/frame->getKey());
    handleFullFrameResults(frame, -1);
    return;
  } else {
    ROIExtractor_->enqueue(frame);
  }
}

void Mondrian::workPostprocess() {
  int scheduleID = 0;
  while (!stop_) {
    int currID = scheduleID++;
    std::unique_lock<std::mutex> packingResultsLock(packingResultsMtx_);
    packingResultsCV_.wait(packingResultsLock, [this]() {
      return !packingResults_.empty() || stop_;
    });
    PackingResult packingResult = std::move(packingResults_.front());
    packingResults_.pop();
    packingResultsLock.unlock();

    auto& streams = packingResult.streams;
    auto& fullFrameTarget = packingResult.fullFrameTarget;
    auto& packedCanvasesTable = packingResult.packedCanvasesTable;

    // Enqueue packed canvases
    for (auto& it : packedCanvasesTable) {
      Device device = it.first;
      auto& packedCanvases = it.second;
      for (const auto& packedCanvas : packedCanvases) {
        assert(packedCanvas.device != Device::INVALID);
        inferenceEngine_->enqueue(
            /*rgbMat=*/packedCanvas.packedMat,
            /*device=*/packedCanvas.device,
            /*inputSize=*/packedCanvas.packedCanvasSize,
            /*isFullFrame=*/false,
            /*key=*/packedCanvas.getKey());
      }
    }

    // Handle full frame inference results
    if (fullFrameTarget != nullptr) {
      handleFullFrameResults(fullFrameTarget, currID);
    }

    // Handle packed canvases or ROI-wise inference results
    if (config_.EXECUTION_TYPE == ExecutionType::ROI_WISE_INFERENCE) {
      handleROIWiseResults(packedCanvasesTable);
    } else {
      handlePackedCanvasesResults(packedCanvasesTable, currID);
    }

    // Interpolate results
    Interpolator::interpolate(streams, config_.INTERPOLATION_THRES);

    // Notify results of rest of the frames
    for (const auto& [vid, stream] : streams) {
      for (Frame* frame : stream) {
        if (frame == fullFrameTarget) continue;
        for (auto& roi : frame->rois) {
          if (roi->box() == nullptr) {
            continue;
          }
          assert(roi->box()->srcROI() == roi.get());
          assert(roi->box()->oid == roi->oid);
          assert(roi->box()->label == roi->label());
        }
        nms(frame->boxes, NUM_LABELS, patchReconstructor_->iouThres());
        frame->isBoxesReady = true;
        frame->endTime = NowMicros();
      }
    }
    ROIExtractor_->cv().notify_all();

    // Update results for system output
    std::unique_lock<std::mutex> resultLock(logMtx_);
    for (const auto& it : streams) {
      for (Frame* frame : it.second) {
        if (frame == fullFrameTarget) continue;
        std::vector<BoundingBox> boxes;
        std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(boxes),
                        [](const std::unique_ptr<BoundingBox>& box) { return *box; });
        results_[frame->vid][frame->fid] = {frame->endTime, std::move(boxes)};
      }
    }
    resultLock.unlock();
    resultsCV_.notify_all();

    // Release used frames
    releaseFrames(streams);
  }
}

void Mondrian::workLog() {
  while (!stop_) {
    std::unique_lock<std::mutex> resultLock(logMtx_);
    resultsCV_.wait(resultLock, [this]() {
      return stop_ || std::any_of(results_.begin(), results_.end(),
                                  [](const auto& it) { return !it.second.empty(); });
    });
    for (const auto& [vid, frameResults] : results_) {
      for (const auto& [fid, endTimeBoxes] : frameResults) {
        const auto& [endTime, boxes] = endTimeBoxes;
        loggerBoxes_->logBoxes(vid, fid, boxes);
      }
      LOGD("[Logger] vid=%d %d ~ %d frames logged",
           vid, frameResults.begin()->first, frameResults.rbegin()->first);
    }
    results_.clear();
    resultLock.unlock();
    resultsCV_.notify_all();
  }
}

} // namespace md
