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
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Utils.hpp"

namespace md {

Mondrian::Mondrian(const MondrianConfig& config, int numVideos, JNIEnv* env, jobject app)
    : config_(config), numVideos_(numVideos), stop_(false),
      resultLogger_(new Logger("/data/data/hcs.offloading.mondrian/boxes.txt")),
      targetSize_(int(config_.roiExtractorConfig.EXTRACTION_RESIZE_WIDTH),
                  int(config_.roiExtractorConfig.EXTRACTION_RESIZE_HEIGHT)),
      inputSizes_(config_.inferenceEngineConfig.INPUT_SIZES),
      scheduleInterval_(config_.LATENCY_SLO_MS * 1000 / 2),
      ROIResizer_(new ROIResizer(config.roiResizerConfig)),
      inferenceEngine_(new InferenceEngine(config.inferenceEngineConfig, env, app)),
      patchReconstructor_(new PatchReconstructor(config.patchReconstructorConfig,
                                                 ROIResizer_.get())) {
  config_.print();
  assert(config_.isValid());
  ROI::PADDING = config.roiExtractorConfig.ROI_PADDING;
  MergedROI::BORDER = config.roiExtractorConfig.ROI_BORDER;
  inferenceEngine_->profileLatency();
  if (config_.FULL_FRAME_INTERVAL != 0) {
    int maxMergeSize = config.EXECUTION_TYPE == MONDRIAN
                       ? *inputSizes_.begin()
                       : config.ROI_SIZE;
    auto latencyTable = inferenceEngine_->latencyTable();
    auto inferencePlan = InferencePlanner::getInferencePlan(
        latencyTable, scheduleInterval_, config_.EXECUTION_TYPE == ROI_WISE_INFERENCE);
    ROIExtractor_ = std::make_unique<ROIExtractor>(
        config_.roiExtractorConfig, maxMergeSize, ROIResizer_.get(),
        config.EXECUTION_TYPE, config.ROI_SIZE, inferencePlan, numVideos_);
  }

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
  std::unique_lock<std::mutex> startLock(startMtx_);
  startCV_.wait(startLock, [this]() { return frameBuffers_.size() == numVideos_; });
  startLock.unlock();
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
    if (scheduleInterval_ > elapsedTime) {
      std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval_ - elapsedTime));
    }

    LOGD("========== Start ==========");
    // 1. Inference planning
    logger.start();
    auto latencyTable = inferenceEngine_->latencyTable();
    time_us fullStartTime = fullFramePlan
        ? latencyTable[config_.FULL_DEVICE][{config_.FULL_FRAME_SIZE, true}]
        : 0L;
    std::vector<InferenceInfo> inferencePlan = InferencePlanner::getInferencePlan(
        latencyTable, scheduleInterval_, config_.EXECUTION_TYPE == ROI_WISE_INFERENCE,
        {{config_.FULL_DEVICE, fullStartTime}});
    logger.step("plan");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         "Mondrian::getInferencePlan", logger.getDuration("plan"), str(inferencePlan).c_str());
    assert(!inferencePlan.empty());

    // 2. Prepare inference
    auto results = ROIExtractor_->prepareInference(inferencePlan, fullFramePlan, scheduleID);
    auto&[packedCanvases, fullFrameTarget, selectedFrames, droppedFrames] = results;
    logger.step("prep");
    LOGD("%-25s took %-7lld us                            "
         "// %4lu PackedCanvases with %s, %lu droppedFrames",
         "RE::prepareInference", logger.getDuration("prep"),
         packedCanvases.size(), str(selectedFrames).c_str(), droppedFrames.size());

    // 3. Enqueue full frame
    if (fullFrameTarget != nullptr) {
      inferenceEngine_->enqueue(fullFrameTarget->rgbMat, config_.FULL_DEVICE, config_.FULL_FRAME_SIZE,
                                true, fullFrameTarget->getKey());
      fullFrameTarget->inferenceFrameSize = config_.FULL_FRAME_SIZE;
      fullFrameTarget->inferenceDevice = config_.FULL_DEVICE;
      LOGD("inferenceEngine_->enqueue %d sized fullFrame to %s | %d",
           config_.FULL_FRAME_SIZE, str(config_.FULL_DEVICE).c_str(), fullFrameTarget->frameIndex);
      logger.step("full");
    } else {
      logger.step("full");
    }

    // 4. Enqueue packed canvases
    for (const auto& packedCanvas: packedCanvases) {
      assert(packedCanvas.device != NO_DEVICE);
      inferenceEngine_->enqueue(packedCanvas.packedMat, packedCanvas.device,
                                packedCanvas.packedCanvasSize, false, packedCanvas.getKey());
      LOGD("inferenceEngine_->enqueue %d sized %d packedCanvas to %s | %s",
           packedCanvas.packedCanvasSize, packedCanvas.absolutePackedCanvasIndex,
           str(packedCanvas.device).c_str(), str(packedCanvas.getPackedFrames()).c_str());
    }

    // 5. Handle full frame inference results
    if (fullFrameTarget != nullptr) {
      handleFullFrameResults(fullFrameTarget);
      LOGD("%-25s took %-7lld us for video %-5d frame %-4d",
           "Mondrian::fullFrameInference", logger.getDuration("full"),
           fullFrameTarget->vid, fullFrameTarget->frameIndex);
    }

    // 6. Handle packed canvases or ROI-wise inference results
    if (config_.EXECUTION_TYPE == ROI_WISE_INFERENCE) {
      handleROIWiseResults(packedCanvases);
    } else {
      handlePackedCanvasesResults(packedCanvases);
    }
    logger.step("inf");
    LOGD("%-25s took %-7lld us                            // Plan: %s",
         config_.EXECUTION_TYPE == ROI_WISE_INFERENCE
         ? "Mondrian::handleROIWiseResults"
         : "Mondrian::handlePackedCanvasesResults",
         logger.getDuration("inf"), str(inferencePlan).c_str());

    // 7. Interpolate results
    Interpolator::interpolate(selectedFrames, config_.INTERPOLATION_THRESHOLD);
    logger.step("itp");
    LOGD("%-25s took %-7lld us", "Interpolator::interpolate", logger.getDuration("itp"));

    // 8. Notify results of rest of the frames
    for (auto& it: selectedFrames) {
      for (Frame* frame: it.second) {
        for (auto& roi: frame->rois) {
          if (roi->box == nullptr) {
            continue;
          }
          assert(roi->box->srcROI == roi.get());
          assert(roi->box->id == roi->id);
          assert(roi->box->label == roi->label);
        }
        nms(frame->boxes, NUM_LABELS, patchReconstructor_->getIoUThreshold());
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
          results_[frame->vid][frame->frameIndex] = std::move(boxes);
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

void Mondrian::handleFullFrameResults(Frame* frame) {
  auto[boxes, times, device] = inferenceEngine_->getResults(frame->getKey());
  assert(device == config_.FULL_DEVICE);
  frame->fullInferenceStartTime = times.first;
  frame->fullInferenceEndTime = times.second;
  for (const BoundingBox& box: boxes) {
    auto& loc = box.loc;
    frame->boxes.push_back(std::make_unique<BoundingBox>(
        INVALID_ID, box.loc, box.confidence, box.label, O_FULL_FRAME));
  }
  patchReconstructor_->matchBoxesROIs(frame, true);

  for (auto& box: frame->boxes) {
    assert(box->id != INVALID_ID);
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
  results_[frame->vid][frame->frameIndex] = std::move(resultBoxes);
  resultLock.unlock();
  resultsCV_.notify_all();
}

void Mondrian::handlePackedCanvasesResults(std::vector<PackedCanvas>& packedCanvases) {
  // Get results of packed canvases sequentially
  for (int i = 0; i < packedCanvases.size(); i++) {
    auto[boxes, times, device] = inferenceEngine_->getResults(packedCanvases[i].getKey());
    assert(device == packedCanvases[i].device);
    LOGD("inferenceEngine_->getResults %d sized %d packedCanvases to %s", packedCanvases[i].packedCanvasSize,
         packedCanvases[i].absolutePackedCanvasIndex, str(packedCanvases[i].device).c_str());
    for (Frame* frame: packedCanvases[i].getPackedFrames()) {
      if (frame->inferenceDevice == NO_DEVICE) {
        frame->inferenceFrameSize = packedCanvases[i].packedCanvasSize;
        frame->inferenceDevice = device;
        frame->packedInferenceStartTime = times.first;
        frame->packedInferenceEndTime = times.second;
      }
    }
    packedCanvases[i].packedMat.release();
    patchReconstructor_->assignBoxesToFrame(packedCanvases[i], boxes);

    for (Frame* frame: packedCanvases[i].getPackedFrames()) {
      if (frame->isReadyToMarry(i)) { // If all mergedROIs are packed and inferenced
        // Match boxes with ROIs (per frame)
        nms(frame->boxes, NUM_LABELS, patchReconstructor_->getIoUThreshold());
        patchReconstructor_->matchBoxesROIs(frame, false);
        frame->isBoxesReady = true;
      }
    }
    // Notify results of processed frames
    ROIExtractor_->notify();
  }
}

void Mondrian::handleROIWiseResults(std::vector<PackedCanvas>& packedCanvases) {
  Stream inferenceFrames;
  for (auto& packedCanvas: packedCanvases) {
    auto[boxes, times, device] = inferenceEngine_->getResults(packedCanvas.getKey());
    assert(packedCanvas.packedROIs.size() == 1);
    assert(device == packedCanvas.device);
    auto[x, y] = (*packedCanvas.packedROIs.begin())->packedXY();
    MergedROI* mergedROI = *packedCanvas.packedROIs.begin();
    if (mergedROI->frame()->inferenceDevice == NO_DEVICE) {
      mergedROI->frame()->inferenceFrameSize = packedCanvas.packedCanvasSize;
      mergedROI->frame()->inferenceDevice = device;
      mergedROI->frame()->packedInferenceStartTime = times.first;
      mergedROI->frame()->packedInferenceEndTime = times.second;
    }
    inferenceFrames.insert(mergedROI->frame());
    for (BoundingBox& b: boxes) {
      mergedROI->frame()->boxes.push_back(std::make_unique<BoundingBox>(
          INVALID_ID, Rect(
              (b.loc.l - float(x)) / mergedROI->targetScale() + mergedROI->loc().l,
              (b.loc.t - float(y)) / mergedROI->targetScale() + mergedROI->loc().t,
              (b.loc.r - float(x)) / mergedROI->targetScale() + mergedROI->loc().l,
              (b.loc.b - float(y)) / mergedROI->targetScale() + mergedROI->loc().t),
          b.confidence, b.label, O_FULL_FRAME));
    }
  }

  for (Frame* frame: inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, patchReconstructor_->getIoUThreshold());
    patchReconstructor_->matchBoxesROIs(frame, false);
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
    for (auto& roi: frame->rois) {
      ROILogger_->logROI(roi.get());
    }
  }
}

int Mondrian::enqueueImage(const int vid, const cv::Mat& yuvMat) {
  assert(!yuvMat.empty());

  std::unique_lock<std::mutex> lock(frameBuffersMtx_);
  if (frameBuffers_.find(vid) == frameBuffers_.end()) {
    frameBuffers_[vid] = std::make_unique<FrameBuffer>(vid, config_.BUFFER_SIZE);
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
  if (config_.FULL_FRAME_INTERVAL == 0 || frame->frameIndex == 0) {
    frame->useInferenceResultForOF = true;
    inferenceEngine_->enqueue(frame->rgbMat, config_.FULL_DEVICE, config_.FULL_FRAME_SIZE, true,
                              frame->getKey());
    frame->inferenceFrameSize = config_.FULL_FRAME_SIZE;
    frame->inferenceDevice = config_.FULL_DEVICE;
    LOGD("inferenceEngine_->enqueue %d sized fullFrame to %s | %d",
         config_.FULL_FRAME_SIZE, str(config_.FULL_DEVICE).c_str(), frame->frameIndex);
    handleFullFrameResults(frame);
    if (config_.FULL_FRAME_INTERVAL == 0) {
      std::lock_guard<std::mutex> framesLock(frameBuffersMtx_);
      log(frame);
      frameBuffers_.at(frame->vid)->freeImage({frame->frameIndex});
    }
  } else {
    if (frame->frameIndex == 1) {
      std::unique_lock<std::mutex> startLock(startMtx_);
      startCV_.wait(startLock, [this]() { return frameBuffers_.size() == numVideos_; });
      startLock.unlock();
      startCV_.notify_all();
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
      return std::any_of(results_.begin(), results_.end(),
                         [](const auto& it) { return !it.second.empty(); });
    });
    for (const auto&[vid, frameResults]: results_) {
      for (const auto&[frameIndex, boxes]: frameResults) {
        resultLogger_->logResult(vid, frameIndex, boxes);
        LOGD("Logger::logResult                         for video %-5d frame %-4d // %4lu boxes",
             vid, frameIndex, boxes.size());
      }
    }
    results_.clear();
    resultLock.unlock();
    resultsCV_.notify_all();
  }
}

} // namespace md
