#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

ROIExtractor::ROIExtractor(
    const ROIExtractorConfig& config,
    int maxMergeSize,
    ROIResizer* roiResizer,
    ExecutionType executionType,
    int roiSize)
    : config_(config),
      maxMergeSize_(maxMergeSize),
      ROIResizer_(roiResizer),
      executionType_(executionType),
      ROISize_(roiSize),
      stop_(false) {
  assert(executionType_ == MONDRIAN || ROISize_ == maxMergeSize_);
  threads_.reserve(config.NUM_WORKERS);
  for (int extractorId = 0; extractorId < config.NUM_WORKERS; extractorId++) {
    threads_.emplace_back([this, extractorId]() { work(extractorId); });
  }
}

ROIExtractor::~ROIExtractor() {
  stop_ = true;
  cv_.notify_all();
  for (auto& thread : threads_) {
    thread.join();
  }
}

void ROIExtractor::enqueue(Frame* frame) {
  std::lock_guard<std::mutex> lock(mtx_);
  PDWaiting_.insert(frame);
  cv_.notify_all();
  LOGD("[ROIExtractor] enqueue "
       "// OFWaiting=%lu OFProcessing=%lu OFProcessed=%d | OFWaiting.front()=%d",
       OFWaiting_.size(), OFProcessing_.size(),
       std::accumulate(OFProcessed_.begin(), OFProcessed_.end(), 0,
                       [](int sum, const auto& pair) { return sum + pair.second.size(); }),
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
}

MultiStream ROIExtractor::collectFrames(int currID) {
  std::unique_lock<std::mutex> lock(mtx_);
  cv_.wait(lock, [this]() { return OFProcessing_.empty(); });

  for (Frame* frame : OFProcessing_) {
    frame->resetOFROIExtraction();
  }

  time_us scheduledTime = NowMicros();
  for (auto& [vid, frames] : OFProcessed_) {
    for (auto& frame : frames) {
      frame->scheduledTime = scheduledTime;
      frame->scheduleID = currID;
      frame->useInferenceResultForOF = true;
    }
  }

  MultiStream streams = std::move(OFProcessed_);
  OFProcessed_.clear();
  cv_.notify_all();

  LOGD("[Schedule %d] Collect Frames "
       "// OFWaiting=%lu OFProcessing=%lu OFWaiting.front()=%d",
       currID,
       OFWaiting_.size(),
       OFProcessing_.size(),
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
  return streams;
}

void ROIExtractor::work(int extractorId) {
  auto getPDJob = [this]() {
    if (!PDWaiting_.empty()) {
      return *PDWaiting_.begin();
    } else {
      return (Frame*) nullptr;
    }
  };
  auto getOFJob = [this, &extractorId]() {
    bool ofFrameExists = !OFWaiting_.empty();
    bool readyForOFExtraction = ofFrameExists && (*OFWaiting_.begin())->readyForOFExtraction();
    if (ofFrameExists && !readyForOFExtraction) {
      LOGD("[ROIExtractor %d] OF frame exists but not ready for OF extraction "
           "// fid=%d, useInfResult=%d",
           extractorId,
           (*OFWaiting_.begin())->frameIndex,
           (*OFWaiting_.begin())->useInferenceResultForOF);
    }
    if (ofFrameExists && readyForOFExtraction) {
      return *OFWaiting_.begin();
    } else {
      return (Frame*) nullptr;
    }
  };

  while (true) {
    bool pd = false;
    Frame* frame = nullptr;

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this, &pd, &frame, &getPDJob, &getOFJob]() {
      if (stop_) return true;
      frame = getOFJob();
      if (frame != nullptr) {
        pd = false;
        return true;
      }
      frame = getPDJob();
      if (frame != nullptr) {
        pd = true;
        return true;
      }
      return false;
    });

    if (stop_) return;

    time_us start = NowMicros();
    if (pd) {
      PDWaiting_.erase(frame);
      PDProcessing_.insert(frame);
    } else {
      OFWaiting_.erase(frame);
      OFProcessing_.insert(frame);
    }
    lock.unlock();

    if (pd) {
      frame->PDExtractorID = extractorId;
      processPD(frame);
    } else {
      frame->OFExtractorID = extractorId;
      processOF(frame);
    }

    lock.lock();
    if (pd) {
      PDProcessing_.erase(frame);
      OFWaiting_.insert(frame);
    } else {
      OFProcessing_.erase(frame);
      if (!frame->reprocessOF) {
        // Common case
        OFProcessed_[frame->vid].insert(frame);
        frame->isROIsReady = true;
      } else {
        // When scheduling is triggered while OF processing
        frame->resetOFROIExtraction();
        OFWaiting_.insert(frame);
      }
    }
    lock.unlock();
    cv_.notify_one();

    time_us end = NowMicros();
    if (pd) {
      LOGD("[ROIExtractor %d] PDTime=%lld // vid=%d fid=%d #PDROIs=%lu",
           extractorId, end - start, frame->vid, frame->frameIndex,
           std::count_if(frame->rois.begin(), frame->rois.end(),
                         [](auto& roi) { return roi->type == PD; }));
    } else {
      LOGD("[ROIExtractor %d] OFTime=%lld "
           "// vid=%d fid=%d #OFROIs=%lu resizeTime=%lld mergeTime=%lld",
           extractorId, end - start, frame->vid, frame->frameIndex,
           std::count_if(frame->rois.begin(), frame->rois.end(),
                         [](auto& roi) { return roi->type == OF; }),
           frame->resizeEndTime - frame->resizeStartTime,
           frame->mergeROIEndTime - frame->mergeROIStartTime);
    }
  }
}

void ROIExtractor::processPD(Frame* currFrame) const {
  assert(currFrame->rois.empty());
  currFrame->pixelDiffROIProcessStartTime = NowMicros();

  float wRatio = (float) currFrame->resizedGrayMat.cols / (float) currFrame->width();
  float hRatio = (float) currFrame->resizedGrayMat.rows / (float) currFrame->height();

  // Get prevFrame
  const Frame* prevFrame = currFrame;
  for (int i = 0; i < config_.PD_INTERVAL; i++) {
    if (prevFrame->frameIndex == 0) break;
    prevFrame = prevFrame->prevFrame;
  }
  assert(prevFrame != nullptr && prevFrame != currFrame);

  // PD ROI Extraction with Scaled Image
  std::vector<Rect> scaledPDRects = extractPD(prevFrame->resizedGrayMat,
                                              currFrame->resizedGrayMat);

  // Rescale PD ROI size
  std::vector<Rect> pdRects;
  std::transform(
      scaledPDRects.begin(), scaledPDRects.end(), std::back_inserter(pdRects),
      [wRatio, hRatio](Rect& resizedPDRect) -> Rect {
        return {resizedPDRect.l / wRatio, resizedPDRect.t / hRatio,
                resizedPDRect.r / wRatio, resizedPDRect.b / hRatio};
      });

  // Generate PD ROIs
  for (const Rect& pdRect : pdRects) {
    if (config_.MIN_PD_ROI_SIZE <= pdRect.minWH && pdRect.maxWH <= config_.MAX_PD_ROI_SIZE) {
      currFrame->rois.emplace_back(new ROI(
          /*prevROI=*/nullptr,
          /*id=*/INVALID_ID,
          /*frame=*/currFrame,
          /*origLoc=*/pdRect,
          /*type=*/PD,
          /*origin=*/O_PD,
          /*label=*/-1,
          /*ofFeatures=*/OFFeatures(),
          /*confidence=*/ROI::INVALID_CONF));
    }
  }
  currFrame->pixelDiffROIProcessEndTime = NowMicros();
}

void ROIExtractor::processOF(Frame* currFrame) const {
  assert(std::all_of(currFrame->rois.begin(), currFrame->rois.end(),
                     [](auto& roi) { return roi->type == PD; }));
  currFrame->opticalFlowROIProcessStartTime = NowMicros();

  Rect imageSize(0.0f, 0.0f, float(currFrame->width()), float(currFrame->height()));
  float wRatio = (float) currFrame->resizedGrayMat.cols / (float) currFrame->width();
  float hRatio = (float) currFrame->resizedGrayMat.rows / (float) currFrame->height();

  // Prepare prevBoxes to track
  std::vector<BoundingBox> prevBoxes;
  const Frame* prevFrame = currFrame->prevFrame;
  if (prevFrame->useInferenceResultForOF) {
    for (const auto& box : prevFrame->boxes) {
      if (box->confidence < config_.OF_CONF_THRESHOLD) continue;
      Rect clippedLoc = box->loc.clip(imageSize);
      if (clippedLoc.minWH < 1) continue;
      BoundingBox prevBox(
          /*id=*/box->id,
          /*location=*/clippedLoc,
          /*confidence=*/box->confidence,
          /*label=*/box->label,
          /*origin=*/O_PACKED_CANVAS);
      prevBox.srcROI = box->srcROI;
      prevBoxes.push_back(prevBox);
    }
  } else {
    for (auto& roi : currFrame->prevFrame->rois) {
      BoundingBox prevBox(
          /*id=*/roi->id,
          /*location=*/roi->origLoc,
          /*confidence=*/1,
          /*label=*/roi->label,
          /*origin=*/roi->origin);
      prevBox.srcROI = roi.get();
      prevBoxes.push_back(prevBox);
    }
  }

  // Convert prevBoxes into scaled prevRects
  std::vector<Rect> scaledPrevRects;
  std::transform(
      prevBoxes.begin(), prevBoxes.end(), std::back_inserter(scaledPrevRects),
      [wRatio, hRatio](const BoundingBox& box) -> Rect {
        return {box.loc.l * wRatio,
                box.loc.t * hRatio,
                box.loc.r * wRatio,
                box.loc.b * hRatio};
      });
  for (const auto& scaledRect: scaledPrevRects) {
    assert(0 <= scaledRect.l && scaledRect.l <= scaledRect.r &&
           scaledRect.r <= currFrame->resizedGrayMat.cols &&
           0 <= scaledRect.t && scaledRect.t <= scaledRect.b &&
           scaledRect.b <= currFrame->resizedGrayMat.rows);
  }

  // OF ROI Extraction with Scaled Image
  std::vector<RectTrackingResult> trackingResults = extractOF(
      prevFrame->resizedGrayMat, currFrame->resizedGrayMat, scaledPrevRects);
  assert(trackingResults.size() == prevBoxes.size());

  for (int i = 0; i < trackingResults.size(); i++) {
    const auto& trackingResult = trackingResults[i];
    std::vector<Shift> _shifts;
    for (int j = 0; j < trackingResult.prevPoints.size(); j++) {
      float x = trackingResult.nextPoints[j].x - trackingResult.prevPoints[j].x;
      float y = trackingResult.nextPoints[j].y - trackingResult.prevPoints[j].y;
      _shifts.emplace_back(x / wRatio, y / hRatio);
    }
    OFFeatures ofFeatures(_shifts, trackingResult.statuses, trackingResult.errors);
    const BoundingBox& box = prevBoxes[i];
    auto [shiftX, shiftY] = ofFeatures.shiftAvg;
    Rect currLoc = Rect(box.loc.l + shiftX,
                        box.loc.t + shiftY,
                        box.loc.r + shiftX,
                        box.loc.b + shiftY).clip(imageSize);
    if (currLoc.minWH < 1) continue;
    currFrame->rois.emplace_back(new ROI(
        /*prevROI=*/box.srcROI,
        /*id=*/box.id,
        /*frame=*/currFrame,
        /*origLoc=*/currLoc,
        /*type=*/OF,
        /*origin=*/box.origin,
        /*label=*/box.label,
        /*ofFeatures=*/ofFeatures,
        /*confidence=*/box.confidence));
  }
  currFrame->opticalFlowROIProcessEndTime = NowMicros();

  currFrame->filterPDROIs(config_.PD_FILTER_THRESHOLD, config_.EAT_PD);

  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeROIs(ROIResizer_, executionType_, ROISize_);
  currFrame->resizeEndTime = NowMicros();

  currFrame->mergeROIStartTime = NowMicros();
  currFrame->resetMergedROIs();
  if (config_.MERGE) {
    currFrame->mergeMergedROIs(maxMergeSize_);
  }
  currFrame->sortMergedROIs();
  currFrame->mergeROIEndTime = NowMicros();

  currFrame->setBoxesIfLast(ROIResizer_,
                            executionType_,
                            config_.NO_DOWNSAMPLING_FOR_LAST_FRAME);
}

} // namespace md
