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
#include "mondrian/ROIPrioritizer.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Utils.hpp"

namespace md {

Mondrian::Mondrian(const MondrianConfig& config, int numVideos, JNIEnv* env, jobject app)
    : config_(config), stop_(false), numFirstFrameReadyVideos_(0), numVideos_(numVideos),
      preprocessTargetSize_(int(config_.roiExtractorConfig.EXTRACTION_RESIZE_WIDTH),
                            int(config_.roiExtractorConfig.EXTRACTION_RESIZE_HEIGHT)),
      scheduleInterval_(config_.LATENCY_SLO_MS * 1000 / 2), numIntervals_(0),
      ROIResizer_(new ROIResizer(config.roiResizerConfig)), startTime_(NowMicros()),
      inferenceEngine_(new InferenceEngine(config.inferenceEngineConfig, env, app)),
      patchReconstructor_(new PatchReconstructor(config.patchReconstructorConfig,
                                                 ROIResizer_.get())) {
  config_.print();
  config_.test();
  ROI::PADDING = config.roiExtractorConfig.ROI_PADDING;
  MergedROI::BORDER = config.roiExtractorConfig.ROI_BORDER;

  // Create loggers
  if (config.LOG_BOXES) {
    loggerBoxes_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/boxes.csv");
    loggerBoxes_->logBoxesHeader();
  }
  if (config.LOG_ROI) {
    loggerROI_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/roi.csv");
    loggerROI_->logROIHeader();
  }
  if (config.LOG_FRAME) {
    loggerFrame_ = std::make_unique<Logger>("/data/data/hcs.offloading.mondrian/frame.csv");
    loggerFrame_->logTimelineHeader();
  }

  // Prepare frame buffers
  for (int vid = 0; vid < numVideos; vid++) {
    frameBuffers_[vid] = std::make_unique<FrameBuffer>(vid);
  }

  // Start preprocessing thread
  preprocessThread_ = std::thread([this]() { workPreprocess(); });

  // Start postprocessing thread
  postprocessThread_ = std::thread([this]() { workPostprocess(); });

  // Start logging thread
  logThread_ = std::thread([this]() { workLog(); });

  // If frame-wise inference, skip ROI extraction and scheduling
  if (config_.EXECUTION_TYPE == FRAME_WISE_INFERENCE) return;

  // Prepare ROI extractor and start scheduling
  inferenceEngine_->profileLatency();
  int maxMergeSize = config.EXECUTION_TYPE == MONDRIAN
                     ? *config_.inferenceEngineConfig.INPUT_SIZES.begin()
                     : config.ROI_SIZE;
  auto latencyTable = inferenceEngine_->latencyTable();
  auto inferencePlan = InferencePlanner::getInferencePlan(
      latencyTable, scheduleInterval_, config_.EXECUTION_TYPE == ROI_WISE_INFERENCE);
  ROIExtractor_ = std::make_unique<ROIExtractor>(
      config_.roiExtractorConfig, maxMergeSize, ROIResizer_.get(),
      config.EXECUTION_TYPE, config.ROI_SIZE);
  scheduleThread_ = std::thread([this]() { workSchedule(); });
}

Mondrian::~Mondrian() {
  stop_ = true;
  resultsCV_.notify_all();
  preprocessThread_.join();
  scheduleThread_.join();
  postprocessThread_.join();
  logThread_.join();
}

void Mondrian::workSchedule() {
  std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval_));
  while (!stop_) {
    time_us scheduleStart = NowMicros();
    int currID = numIntervals_++;
    bool fullFramePlan = currID % config_.FULL_FRAME_INTERVAL == 0;
    int fullFrameVid = fullFramePlan
        ? (currID / config_.FULL_FRAME_INTERVAL) % numVideos_
        : -1;

    LOGD("========== Schedule %d Start at %.2f ms ==========",
         currID, (float) (NowMicros() - startTime_) / 1000);
    // Inference planning
    auto latencyTable = inferenceEngine_->latencyTable();
    time_us fullStartTime = fullFramePlan
                            ? latencyTable[config_.FULL_DEVICE][{config_.FULL_FRAME_SIZE, true}]
                            : 0L;
    time_us budget = scheduleInterval_ - 500000;
    std::vector<InferenceInfo> inferencePlan = InferencePlanner::getInferencePlan(
        latencyTable, budget, config_.EXECUTION_TYPE == ROI_WISE_INFERENCE,
        {{config_.FULL_DEVICE, fullStartTime}});
    assert(!inferencePlan.empty());
    std::stringstream ss;
    for (auto& [device, sizeIsFullLatency] : latencyTable) {
      for (auto& [sizeIsFull, latency] : sizeIsFullLatency) {
        ss << sizeIsFull.first << ": " << latency << " us, ";
      }
    }
    LOGD("Schedule %d Plan at %lld ms: %lld us budget, %s | %s",
         currID, (NowMicros() - startTime_) / 1000,
         budget, str(inferencePlan).c_str(), ss.str().c_str());

    // Getting PackedFrames
    MultiStream streams = ROIExtractor_->collectFrames(currID);

    time_us startTime = NowMicros();
    std::vector<std::vector<IntRect>> freeRectsVec;
    for (const auto& info : inferencePlan) {
      freeRectsVec.push_back({IntRect(0, 0, info.size, info.size)});
    }

    // Full frame
    Frame* fullFrameTarget = nullptr;
    if (streams.find(fullFrameVid) != streams.end()) {
      fullFrameTarget = *streams.at(fullFrameVid).rbegin();
      streams[fullFrameVid].erase(fullFrameTarget);
      LOGD("XXX == Last Full Frame %d", fullFrameTarget->frameIndex);
    }

    // Last frames
    for (const auto& [vid, stream] : streams) {
      if (vid == fullFrameVid) {
        continue;
      }
      Frame* lastFrame = *stream.rbegin();
      if (config_.EXECUTION_TYPE == ROI_WISE_INFERENCE) {
        for (auto& mergedROI : lastFrame->mergedROIs) {
          auto [bw, bh] = mergedROI->borderedMatWH();
          if (!freeRectsVec.empty()) {
            mergedROI->setPackInfo({0, 0},
                                   (int) freeRectsVec.size() - 1,
                                   config_.EXECUTION_TYPE,
                                   config_.ROI_SIZE);
            freeRectsVec.erase(freeRectsVec.end() - 1);
          }
        }
      } else { // MONDRIAN, EMULATED_BATCH
        auto [indices, locations] = ROIPacker::pack(freeRectsVec, lastFrame->boxesIfLast,
            /*backward=*/false, config_.EXECUTION_TYPE, config_.ROI_SIZE);
        bool fullyPacked = indices.size() == lastFrame->boxesIfLast.size();
        int maxPackedCanvasIndex = -1;
        for (auto& [packedCanvasIndex, freeRectIndex] : indices) {
          maxPackedCanvasIndex = std::max(maxPackedCanvasIndex, packedCanvasIndex);
        }
        LOGD("XXX == Last Pack Frame %d: %lu / %lu Packed, Last Packed Frame=%d",
             lastFrame->frameIndex,
             indices.size(),
             lastFrame->boxesIfLast.size(),
             maxPackedCanvasIndex);
        if (fullyPacked) {
          ROIPacker::apply(freeRectsVec,
                           lastFrame->boxesIfLast,
                           indices,
                           config_.EXECUTION_TYPE,
                           config_.ROI_SIZE);
        } else {
          indices.resize(lastFrame->boxesIfLast.size());
          locations.resize(lastFrame->boxesIfLast.size());
          ROIPacker::apply(freeRectsVec,
                           lastFrame->boxesIfLast,
                           indices,
                           config_.EXECUTION_TYPE,
                           config_.ROI_SIZE);
        }
        lastFrame->prepareFrameLast(indices,
                                    locations,
                                    config_.EXECUTION_TYPE,
                                    config_.ROI_SIZE,
                                    config_.roiExtractorConfig.NO_DOWNSAMPLING_FOR_LAST_FRAME);
      }
    }
    time_us packLastTime = NowMicros();

    // Order MergedROIs
    auto orderedMergedROIs = ROIPrioritizer::order(streams,
                                                   fullFrameVid,
                                                   config_.roiExtractorConfig.ROI_PRIORITIZER_TYPE);
    time_us orderTime = NowMicros();

    // Pack MergedROIs
    for (MergedROI* mergedROI : orderedMergedROIs) {
      if (config_.EXECUTION_TYPE == ROI_WISE_INFERENCE) {
        if (!freeRectsVec.empty()) {
          mergedROI->setPackInfo({0, 0},
                                 freeRectsVec.size() - 1,
                                 config_.EXECUTION_TYPE,
                                 config_.ROI_SIZE);
          freeRectsVec.erase(freeRectsVec.end() - 1);
        }
      } else {
        auto [bw, bh] = mergedROI->borderedMatWH();
        auto [indices, locations] = ROIPacker::pack(freeRectsVec, {{bw, bh}},
                                                    /*backward=*/false,
                                                    config_.EXECUTION_TYPE,
                                                    config_.ROI_SIZE);
        if (!indices.empty()) {
          ROIPacker::apply(freeRectsVec,
                           {{bw, bh}},
                           indices,
                           config_.EXECUTION_TYPE,
                           config_.ROI_SIZE);
          mergedROI->setPackInfo(locations[0],
                                 indices[0].first,
                                 config_.EXECUTION_TYPE,
                                 config_.ROI_SIZE);
        }
      }
    }
    time_us packOthersTime = NowMicros();

    LOGD("Packing %d Frames with %lu ROIs | "
         "total: %lld, packLastTime: %lld, orderTime: %lld, packOthersTime: %lld",
         std::accumulate(streams.begin(), streams.end(), 0,
                         [](int sum, const auto& pair) {
                           return sum + pair.second.size();
                         }) + (streams.find(fullFrameVid) != streams.end() ? 1 : 0),
         orderedMergedROIs.size(),
         packOthersTime - startTime,
         packLastTime - startTime,
         orderTime - packLastTime,
         packOthersTime - orderTime);

    std::map<int, std::set<MergedROI*>> groupedMergedROIs;
    for (const auto& [vid, frames] : streams) {
      for (Frame* frame : frames) {
        assert(frame != fullFrameTarget);
        for (auto& mergedROI : frame->mergedROIs) {
          if (mergedROI->isPacked()) {
            groupedMergedROIs[mergedROI->relativePackedCanvasIndex()].insert(mergedROI.get());
          }
        }
        for (auto& probeROI : frame->probingROIs) {
          if (probeROI->isPacked()) {
            groupedMergedROIs[probeROI->relativePackedCanvasIndex()].insert(probeROI.get());
          }
        }
      }
    }

    std::vector<PackedCanvas> packedCanvases;
    for (auto& [relativePackedCanvasIndex, mergedROIs] : groupedMergedROIs) {
      assert(relativePackedCanvasIndex < inferencePlan.size());
      const auto& info = inferencePlan[relativePackedCanvasIndex];
      if (!mergedROIs.empty()) {
        packedCanvases.emplace_back(mergedROIs, info.size, info.device);
      }
    }
    packingResults_.put({streams, fullFrameTarget, packedCanvases});

    LOGD("========== Schedule %d End   at %.2f ms ==========",
         currID, (float) (NowMicros() - startTime_) / 1000);

    // Wait for scheduling interval
    time_us sleepTime = scheduleInterval_ - (NowMicros() - scheduleStart);
    if (sleepTime > 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
    }
  }
}

void Mondrian::handleFullFrameResults(Frame* frame, int currID) {
  auto [boxes, times, device] = inferenceEngine_->getResults(frame->getKey());
  LOGD("Inference %d (%d, %s, FULL) at %lld ms | %d",
       currID, config_.FULL_FRAME_SIZE, str(device).c_str(),
       (NowMicros() - startTime_) / 1000, frame->frameIndex);
  frame->fullInferenceStartTime = times.first;
  frame->fullInferenceEndTime = times.second;
  for (const BoundingBox& box : boxes) {
    auto& loc = box.loc;
    frame->boxes.push_back(std::make_unique<BoundingBox>(
        INVALID_ID, box.loc, box.confidence, box.label, O_FULL_FRAME));
  }
  patchReconstructor_->matchBoxesROIs(frame, true);

  for (auto& box : frame->boxes) {
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

  std::unique_lock<std::mutex> resultLock(logMtx_);
  std::vector<BoundingBox> resultBoxes;
  std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(resultBoxes),
                 [](const std::unique_ptr<BoundingBox>& box) { return *box; });
  results_[frame->vid][frame->frameIndex] = {frame->endTime, std::move(resultBoxes)};
  resultLock.unlock();
  resultsCV_.notify_all();
}

void Mondrian::handlePackedCanvasesResults(std::vector<PackedCanvas>& packedCanvases, int currID) {
  // Get results of packed canvases sequentially
  for (int i = 0; i < packedCanvases.size(); i++) {
    auto [boxes, times, device] = inferenceEngine_->getResults(packedCanvases[i].getKey());
    const int inputSize = packedCanvases[i].packedCanvasSize;
    std::stringstream ss;
    for (Frame* frame : packedCanvases[i].getPackedFrames()) {
      ss << frame->frameIndex << ", ";
    }
    LOGD("Inference %d (%d, %s, PACK) at %lld ms | %s",
         currID, inputSize, str(device).c_str(),
         (NowMicros() - startTime_) / 1000, ss.str().c_str());
    assert(device == packedCanvases[i].device);
    for (Frame* frame : packedCanvases[i].getPackedFrames()) {
      if (frame->inferenceDevice == NO_DEVICE) {
        frame->inferenceFrameSize = inputSize;
        frame->inferenceDevice = device;
        frame->packedInferenceStartTime = times.first;
        frame->packedInferenceEndTime = times.second;
      }
    }
    packedCanvases[i].packedMat.release();
    patchReconstructor_->assignBoxesToFrame(packedCanvases[i], boxes);

    for (Frame* frame : packedCanvases[i].getPackedFrames()) {
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
  for (auto& packedCanvas : packedCanvases) {
    auto [boxes, times, device] = inferenceEngine_->getResults(packedCanvas.getKey());
    assert(packedCanvas.packedROIs.size() == 1);
    assert(device == packedCanvas.device);
    auto [x, y] = (*packedCanvas.packedROIs.begin())->packedXY();
    MergedROI* mergedROI = *packedCanvas.packedROIs.begin();
    if (mergedROI->frame()->inferenceDevice == NO_DEVICE) {
      mergedROI->frame()->inferenceFrameSize = packedCanvas.packedCanvasSize;
      mergedROI->frame()->inferenceDevice = device;
      mergedROI->frame()->packedInferenceStartTime = times.first;
      mergedROI->frame()->packedInferenceEndTime = times.second;
    }
    inferenceFrames.insert(mergedROI->frame());
    const float mergedScale = mergedROI->targetScale();
    for (BoundingBox& b : boxes) {
      float newL = std::max(0.0f, (b.loc.l - float(x)) / mergedScale + mergedROI->loc().l);
      float newT = std::max(0.0f, (b.loc.t - float(y)) / mergedScale + mergedROI->loc().t);
      float newR = std::max(0.0f, (b.loc.r - float(x)) / mergedScale + mergedROI->loc().l);
      float newB = std::max(0.0f, (b.loc.b - float(y)) / mergedScale + mergedROI->loc().t);
      assert(0 <= newL && 0 <= newT && newL <= newR && newT <= newB);
      mergedROI->frame()->boxes.push_back(std::make_unique<BoundingBox>(
          INVALID_ID, Rect(newL, newT, newR, newB), b.confidence, b.label, O_FULL_FRAME));
    }
  }

  for (Frame* frame : inferenceFrames) {
    nms(frame->boxes, NUM_LABELS, patchReconstructor_->getIoUThreshold());
    patchReconstructor_->matchBoxesROIs(frame, false);
  }
}

void Mondrian::releaseFrames(const MultiStream& multiStream) {
  for (const auto& [vid, stream] : multiStream) {
    if (stream.empty()) continue;
    for (Frame* frame : stream) {
      log(frame);
    }
    int lastFrameIndex = (*stream.rbegin())->frameIndex;
    frameBuffers_.at(vid)->free(lastFrameIndex - config_.roiExtractorConfig.PD_INTERVAL);
  }
}

void Mondrian::log(const Frame* frame) {
  if (loggerFrame_) {
    loggerFrame_->logTimeline(frame);
  }
  if (loggerROI_) {
    for (auto& roi : frame->rois) {
      loggerROI_->logROI(roi.get());
    }
  }
}

void Mondrian::enqueue(const int vid, const cv::Mat& yuvMat) {
  assert(!yuvMat.empty());

  Frame* frame = frameBuffers_.at(vid)->enqueue(yuvMat);
  if (numVideos_ > 1 && frame->frameIndex == 1) {
    std::unique_lock<std::mutex> startLock(startMtx_);
    startCV_.wait(startLock, [this]() {
      assert(numFirstFrameReadyVideos_ <= numVideos_);
      return numFirstFrameReadyVideos_ == numVideos_;
    });
  }
  preprocessQueue_.put(frame);
}

void Mondrian::workPreprocess() {
  int id = 0;
  while (!stop_) {
    int currID = id++;
    Frame* frame = preprocessQueue_.take();

    frame->prepareRgbMatAndResizedGrayMat(preprocessTargetSize_);

    if (config_.EXECUTION_TYPE == FRAME_WISE_INFERENCE) {
      inferenceEngine_->enqueue(frame->rgbMat, config_.FULL_DEVICE, config_.FULL_FRAME_SIZE, true,
                                frame->getKey());
      frame->inferenceFrameSize = config_.FULL_FRAME_SIZE;
      frame->inferenceDevice = config_.FULL_DEVICE;
      handleFullFrameResults(frame, currID);
      std::lock_guard<std::mutex> framesLock(frameBuffersMtx_);
      log(frame);
      frameBuffers_.at(frame->vid)->free(frame->frameIndex);
      continue;
    }

    if (frame->frameIndex == 0) {
      frame->useInferenceResultForOF = true;
      inferenceEngine_->enqueue(frame->rgbMat, config_.FULL_DEVICE, config_.FULL_FRAME_SIZE, true,
                                frame->getKey());
      frame->inferenceFrameSize = config_.FULL_FRAME_SIZE;
      frame->inferenceDevice = config_.FULL_DEVICE;
      LOGD("inferenceEngine_->enqueue %d sized fullFrame to %s | %d",
           config_.FULL_FRAME_SIZE, str(config_.FULL_DEVICE).c_str(), frame->frameIndex);
      handleFullFrameResults(frame, currID);
      {
        std::lock_guard<std::mutex> startLock(startMtx_);
        numFirstFrameReadyVideos_++;
      }
      startCV_.notify_all();
    } else {
      ROIExtractor_->enqueue(frame);
    }
  }
}

void Mondrian::workPostprocess() {
  int id = 0;
  while (!stop_) {
    int currID = id++;
    PackingResult packingResult = packingResults_.take();
    auto& packedCanvases = packingResult.packedCanvases;
    auto& fullFrameTarget = packingResult.fullFrameTarget;
    auto& selectedFrames = packingResult.streams;

    // Enqueue full frame
    if (fullFrameTarget != nullptr) {
      inferenceEngine_->enqueue(
          fullFrameTarget->rgbMat, config_.FULL_DEVICE, config_.FULL_FRAME_SIZE,
          /*isFullFrame=*/true, fullFrameTarget->getKey());
      fullFrameTarget->inferenceFrameSize = config_.FULL_FRAME_SIZE;
      fullFrameTarget->inferenceDevice = config_.FULL_DEVICE;
    }

    // Enqueue packed canvases
    for (const auto& packedCanvas : packedCanvases) {
      assert(packedCanvas.device != NO_DEVICE);
      inferenceEngine_->enqueue(
          packedCanvas.packedMat, packedCanvas.device, packedCanvas.packedCanvasSize,
          /*isFullFrame=*/false, packedCanvas.getKey());
    }

    // Handle full frame inference results
    if (fullFrameTarget != nullptr) {
      handleFullFrameResults(fullFrameTarget, currID);
    }

    // Handle packed canvases or ROI-wise inference results
    if (config_.EXECUTION_TYPE == ROI_WISE_INFERENCE) {
      handleROIWiseResults(packedCanvases);
    } else {
      handlePackedCanvasesResults(packedCanvases, currID);
    }

    // Interpolate results
    Interpolator::interpolate(selectedFrames, config_.INTERPOLATION_THRESHOLD);

    // Notify results of rest of the frames
    for (auto& it : selectedFrames) {
      for (Frame* frame : it.second) {
        for (auto& roi : frame->rois) {
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

    // Update results for system output
    std::unique_lock<std::mutex> resultLock(logMtx_);
    for (const auto& it : selectedFrames) {
      for (Frame* frame : it.second) {
        if (frame != fullFrameTarget) {
          std::vector<BoundingBox> boxes;
          std::transform(frame->boxes.begin(), frame->boxes.end(), std::back_inserter(boxes),
                         [](const std::unique_ptr<BoundingBox>& box) { return *box; });
          results_[frame->vid][frame->frameIndex] = {frame->endTime, std::move(boxes)};
        }
      }
    }
    resultLock.unlock();
    resultsCV_.notify_all();

    // Release used frames
    releaseFrames(selectedFrames);
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
      for (const auto& [frameIndex, endTimeBoxes] : frameResults) {
        const auto& [endTime, boxes] = endTimeBoxes;
        loggerBoxes_->logBoxes(vid, frameIndex, boxes);
      }
      LOGD("Boxes from video %d frame %d ~ %d logged",
           vid, frameResults.begin()->first, frameResults.rbegin()->first);
    }
    results_.clear();
    resultLock.unlock();
    resultsCV_.notify_all();
  }
}

} // namespace md
