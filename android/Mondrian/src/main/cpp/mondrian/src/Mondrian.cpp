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
      ROIResizer_(new ROIResizer(config.roiResizerConfig)) {
  tracer_ = std::make_unique<chrome_tracer::ChromeTracer>();
  inferenceEngine_ = std::make_unique<InferenceEngine>(
      config.inferenceEngineConfig, env, app, tracer_.get());
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
  int maxMergeSize = -1;
  if (config_.EXECUTION_TYPE == ExecutionType::ROI_WISE_INFERENCE
      || config_.EXECUTION_TYPE == ExecutionType::EMULATED_BATCH) {
    maxMergeSize = config_.ROI_SIZE;
  } else if (config_.EXECUTION_TYPE == ExecutionType::MONDRIAN) {
    maxMergeSize = INT_MAX;
    for (const auto& [device, workerConfig] : config_.inferenceEngineConfig.WORKER_CONFIGS) {
      for (const int inputSize : workerConfig.INPUT_SIZES) {
        maxMergeSize = std::min(maxMergeSize, inputSize);
      }
    }
  }
  ROIExtractor_ = std::make_unique<ROIExtractor>(config_.roiExtractorConfig,
                                                 config_.EXECUTION_TYPE,
                                                 maxMergeSize,
                                                 config_.ROI_SIZE,
                                                 ROIResizer_.get(),
                                                 tracer_.get());

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
    int handle = tracer_->BeginEvent(scheduleThreadTag,
                                     "schedule(" + std::to_string(currID) + ")");
    LOGD("[Schedule %d] ========== Start at %lld ==========", currID, NowMicros() - startTime_);

    // Getting PackedFrames
    // TODO: ROIExtractors for each video
    time_us collectStart = NowMicros();
    std::list<Frame*> stream = ROIExtractor_->collectFrames(currID);
    MultiStream streams;
    while (!stream.empty()) {
      Frame* frame = stream.front();
      stream.pop_front();
      streams[frame->vid].insert(frame);
    }
    time_us collectEnd = NowMicros();
    LOGD("[Schedule %d] Collect Frames %lld us // %s",
         currID,
         collectEnd - collectStart,
         str(streams).c_str());

    // Prepare & Enqueue Full frame
    Frame* fullFrameTarget = nullptr;
    int fullFrameVid = (currID + 1) % config_.FULL_FRAME_INTERVAL == 0
                       ? ((currID + 1) / config_.FULL_FRAME_INTERVAL) % numVideos_
                       : -1;
    tracer_->EndEvent(scheduleThreadTag, handle);
    handle = tracer_->BeginEvent(scheduleThreadTag,
                                 "fullFrameHandling(" + std::to_string(currID) + ")");
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
    tracer_->EndEvent(scheduleThreadTag, handle);

    // Inference planning
    handle = tracer_->BeginEvent(scheduleThreadTag,
                                 "inferencePlanning(" + std::to_string(currID) + ")");
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
    tracer_->EndEvent(scheduleThreadTag, handle);

    // Prepare Packed Canvases
    handle = tracer_->BeginEvent(scheduleThreadTag, "packCanvases(" + std::to_string(currID) + ")");
    std::map<Device, std::vector<PackedCanvas>> packedCanvasesTable = ROIPacker_->packCanvases(
        /*currID=*/currID,
        /*streams=*/streams,
        /*inferencePlan=*/inferencePlan,
        /*fullFrameTarget=*/fullFrameTarget);
    tracer_->EndEvent(scheduleThreadTag, handle);

    handle = tracer_->BeginEvent(scheduleThreadTag,
                                 "enqueueAndLock(" + std::to_string(currID) + ")");
    // Enqueue packed canvases
    for (const auto& [device, packedCanvases] : packedCanvasesTable) {
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
    // Pass to postprocess thread
    std::unique_lock<std::mutex> packingResultsLock(packingResultsMtx_);
    packingResults_.push({streams, fullFrameTarget, packedCanvasesTable});
    packingResultsLock.unlock();
    packingResultsCV_.notify_one();

    time_us planEnd = NowMicros();
    planningTime_ = (7 * (planEnd - planStart) + 3 * planningTime_) / 10;

    LOGD("[Schedule %d] ========== End   at %lld ==========", currID, NowMicros() - startTime_);
    tracer_->EndEvent(scheduleThreadTag, handle);

    // Wait for scheduling interval
    time_us sleepTime = scheduleInterval_ - (NowMicros() - scheduleStart);
    if (sleepTime > 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
    }
  }
}

void Mondrian::handleFullFrameResults(Frame* frame, int currID) {
  Result result = inferenceEngine_->getResult(frame->getKey(), /*isCheckedKey=*/false);
  LOGD("[Schedule %d] FULL Inference at %lld // vid=%d fid=%d",
       currID, NowMicros() - startTime_, frame->vid, frame->fid);

  int handle = tracer_->BeginEvent(postprocessThreadTag,
                                   "postprocess" + std::to_string(currID) + " full");

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
    ROIExtractor_->notify();
  }

  frame->endTime = NowMicros();
  tracer_->EndEvent(postprocessThreadTag, handle);
}

static std::pair<Device, int> getReadyPackedCanvas(
    const std::map<Device, std::vector<PackedCanvas>>& packedCanvasesTable,
    InferenceEngine* inferenceEngine) {
  while (true) {
    inferenceEngine->waitForAnyResults();
    for (const auto& [device, packedCanvases] : packedCanvasesTable) {
      for (int i = 0; i < packedCanvases.size(); i++) {
        if (inferenceEngine->isReady(packedCanvases[i].getKey())) {
          return {device, i};
        }
      }
    }
  }
}

void Mondrian::handlePackedCanvasesResults(
    std::map<Device, std::vector<PackedCanvas>>& packedCanvasesTable,
    int currID) {
  // Get results of packed canvases
  int numTotalCanvases = std::accumulate(
      packedCanvasesTable.begin(), packedCanvasesTable.end(), 0,
      [](int sum, const auto& it) { return sum + it.second.size(); });
  for (int numReadyCanvases = 0; numReadyCanvases < numTotalCanvases; numReadyCanvases++) {
    const auto& [device, canvasIndex] = getReadyPackedCanvas(packedCanvasesTable,
                                                             inferenceEngine_.get());
    PackedCanvas& packedCanvas = packedCanvasesTable[device][canvasIndex];
    Result result = inferenceEngine_->getResult(packedCanvas.getKey(), /*isCheckedKey=*/true);
    int handle = tracer_->BeginEvent(
        postprocessThreadTag,
        "postprocess" + std::to_string(currID) + " pack" + std::to_string(packedCanvas.pid));
    packedCanvas.packedMat.release();
    const int inputSize = packedCanvas.packedCanvasSize;
    LOGD("[Schedule %d] Pack Inference on %s at %lld // %s",
         currID, str(result.device).c_str(), NowMicros() - startTime_,
         str(packedCanvas.getPackedFrames()).c_str());
    assert(result.device == packedCanvas.device);
    for (MergedROI* packedROI : packedCanvas.packedROIs) {
      packedROI->setInferenced(true);
    }
    for (Frame* frame : packedCanvas.getPackedFrames()) {
      if (frame->deviceIfFullFrame == Device::INVALID) {
        frame->inferenceFrameSize = inputSize;
        frame->packedInferenceStartTime = result.detectionStart;
        frame->packedInferenceEndTime = result.detectionEnd;
      }
    }
    patchReconstructor_->assignBoxesToFrame(packedCanvas, result.boxes);

    for (Frame* frame : packedCanvas.getPackedFrames()) {
      if (frame->isReadyToMarry()) { // If all mergedROIs are packed and inferenced
        // Match boxes with ROIs (per frame)
        nms(frame->boxes, NUM_LABELS, patchReconstructor_->iouThres());
        patchReconstructor_->matchBoxesROIs(frame, false);
        frame->isBoxesReady = true;
      }
    }
    // Notify results of processed frames
    ROIExtractor_->notify();
    tracer_->EndEvent(postprocessThreadTag, handle);
  }
}

void Mondrian::handleROIWiseResults(
    std::map<Device, std::vector<PackedCanvas>>& packedCanvasesTable, int currID) {
  Stream inferenceFrames;
  int numTotalCanvases = std::accumulate(
      packedCanvasesTable.begin(), packedCanvasesTable.end(), 0,
      [](int sum, const auto& it) { return sum + it.second.size(); });
  for (int numReadyCanvases = 0; numReadyCanvases < numTotalCanvases; numReadyCanvases++) {
    const auto& [device, canvasIndex] = getReadyPackedCanvas(packedCanvasesTable,
                                                             inferenceEngine_.get());
    PackedCanvas& packedCanvas = packedCanvasesTable[device][canvasIndex];
    Result result = inferenceEngine_->getResult(packedCanvas.getKey(), /*isCheckedKey=*/true);
    int handle = tracer_->BeginEvent(
        postprocessThreadTag,
        "postprocess" + std::to_string(currID) + " pack" + std::to_string(packedCanvas.pid));
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
    tracer_->EndEvent(postprocessThreadTag, handle);
  }

  for (Frame* frame : inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, patchReconstructor_->iouThres());
    patchReconstructor_->matchBoxesROIs(frame, false);
  }
}

void Mondrian::releaseFrames(const MultiStream& streams) {
  for (const auto& [vid, stream] : streams) {
    if (stream.empty()) continue;
    int lastFrameIndex = (*stream.rbegin())->fid;
    frameBuffers_.at(vid)->free(lastFrameIndex - config_.roiExtractorConfig.PD_INTERVAL);
  }
}

void Mondrian::logFrame(const Frame* frame) {
  if (loggerFrame_) {
    std::lock_guard<std::mutex> lock(loggerFrame_->mtx());
    loggerFrame_->logFrame(frame);
    loggerFrame_->flush();
  }
  if (loggerROI_) {
    std::lock_guard<std::mutex> lock(loggerROI_->mtx());
    loggerROI_->logROIs(frame);
    loggerROI_->flush();
  }
  if (loggerBoxes_) {
    std::lock_guard<std::mutex> lock(loggerBoxes_->mtx());
    loggerBoxes_->logBoxes(frame);
    loggerBoxes_->flush();
  }
}

void Mondrian::logFrames(const MultiStream& streams) {
  if (loggerFrame_) {
    std::lock_guard<std::mutex> lock(loggerFrame_->mtx());
    for (const auto& [vid, stream] : streams) {
      for (const auto& frame : stream) {
        loggerFrame_->logFrame(frame);
      }
    }
    loggerFrame_->flush();
  }
  if (loggerROI_) {
    std::lock_guard<std::mutex> lock(loggerROI_->mtx());
    for (const auto& [vid, stream] : streams) {
      for (const auto& frame : stream) {
        loggerROI_->logROIs(frame);
      }
    }
    loggerROI_->flush();
  }
  if (loggerBoxes_) {
    std::lock_guard<std::mutex> lock(loggerBoxes_->mtx());
    for (const auto& [vid, stream] : streams) {
      for (const auto& frame : stream) {
        loggerBoxes_->logBoxes(frame);
      }
    }
    loggerBoxes_->flush();
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
  logFrame(frame);
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
    logFrame(frame);
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

    // Handle full frame inference results
    if (fullFrameTarget != nullptr) {
      handleFullFrameResults(fullFrameTarget, currID);
    }

    // Handle packed canvases or ROI-wise inference results
    if (config_.EXECUTION_TYPE == ExecutionType::ROI_WISE_INFERENCE) {
      handleROIWiseResults(packedCanvasesTable, currID);
    } else {
      handlePackedCanvasesResults(packedCanvasesTable, currID);
    }

    // Interpolate results
    int handle = tracer_->BeginEvent(postprocessThreadTag,
                                     "postprocess" + std::to_string(currID) + " interp");
    Interpolator::interpolate(streams, config_.INTERPOLATION_THRES);
    tracer_->EndEvent(postprocessThreadTag, handle);

    // Notify results of rest of the frames
    handle = tracer_->BeginEvent(postprocessThreadTag,
                                 "postprocess" + std::to_string(currID) + " nms");
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
    ROIExtractor_->notify();

    // Update results for system output
    std::unique_lock<std::mutex> resultLock(logMtx_);
    results_.push_back(streams);
    resultLock.unlock();
    resultsCV_.notify_all();
    tracer_->EndEvent(postprocessThreadTag, handle);
  }
}

void Mondrian::workLog() {
  while (!stop_) {
    std::unique_lock<std::mutex> resultLock(logMtx_);
    resultsCV_.wait(resultLock, [this]() { return stop_ || !results_.empty(); });
    MultiStream streams = std::move(results_.front());
    results_.pop_front();
    resultLock.unlock();
    resultsCV_.notify_all();

    int handle = tracer_->BeginEvent(logThreadTag, "log");
    logFrames(streams);
    releaseFrames(streams);
    bool success = tracer_->DumpToFile(
        /*logPath=*/"/data/data/hcs.offloading.mondrian/trace.json",
        /*do_validate=*/false);
    assert(success);
    tracer_->EndEvent(logThreadTag, handle);
  }
}

} // namespace md
