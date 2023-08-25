#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

ROIExtractor::ROIExtractor(const ROIExtractorConfig& config,
                           const ExecutionType executionType,
                           const int roiSize,
                           ROIResizer* roiResizer)
    : config_(config),
      executionType_(executionType),
      roiSize_(roiSize),
      ROIResizer_(roiResizer),
      stop_(false) {
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
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->fid);
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
//    if (ofFrameExists && !readyForOFExtraction) {
//      LOGD("[ROIExtractor %d] OF frame exists but not ready for OF extraction "
//           "// fid=%d, useInfResult=%d",
//           extractorId,
//           (*OFWaiting_.begin())->fid,
//           (*OFWaiting_.begin())->prevFrame->useInferenceResultForOF);
//    }
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

    time_us startTime = NowMicros();
    if (pd) {
      PDWaiting_.erase(frame);
      PDProcessing_.insert(frame);
    } else {
      OFWaiting_.erase(frame);
      OFProcessing_.insert(frame);
    }
    lock.unlock();
    time_us gettingTime = NowMicros();

    if (pd) {
      frame->PDExtractorID = extractorId;
      processPD(frame);
    } else {
      frame->OFExtractorID = extractorId;
      processOF(frame);
    }
    time_us processingTime = NowMicros();

    lock.lock();
    time_us lockingTime = NowMicros();
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
    time_us endTime = NowMicros();

    std::stringstream ss;
    ss << "[ROIExtractor " << extractorId << "] "
       << (pd ? " PD" : " OF")
       << " (" << frame->vid << ", " << frame->fid << ")"
       << " #=" << std::count_if(frame->rois.begin(), frame->rois.end(),
                                 [pd](auto& roi) {
                                   return roi->type() == (pd ? ROIType::PD : ROIType::OF);
                                 })
       << " PDQ=" << PDWaiting_.size()
       << " OFQ=" << OFWaiting_.size()
       << " RQ=" << std::accumulate(OFProcessed_.begin(), OFProcessed_.end(), 0,
                                    [](int sum, const auto& pair) {
                                      return sum + pair.second.size();
                                    })
       << " total=" << endTime - startTime
       << " getting=" << gettingTime - startTime
       << " processing=" << processingTime - gettingTime
       << " locking=" << lockingTime - processingTime
       << " putting=" << endTime - lockingTime;
    if (!pd) {
      ss << " #Features=" << frame->numFeaturePoints
         << " ext=" << frame->opticalFlowROIProcessEndTime - frame->opticalFlowROIProcessStartTime
         << " resize=" << frame->resizeEndTime - frame->resizeStartTime
         << " merge=" << frame->mergeROIEndTime - frame->mergeROIStartTime;
    }
    LOGD("%s", ss.str().c_str());

    lock.unlock();
    cv_.notify_one();
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
    if (prevFrame->fid == 0) break;
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
          /*oid=*/INVALID_OID,
          /*frame=*/currFrame,
          /*origLoc=*/pdRect,
          /*type=*/ROIType::PD,
          /*origin=*/Origin::PD,
          /*label=*/-1,
          /*ofFeatures=*/OFFeatures(),
          /*confidence=*/ROI::INVALID_CONF,
          /*padding=*/0));
    }
  }
  currFrame->pixelDiffROIProcessEndTime = NowMicros();
}

void ROIExtractor::processOF(Frame* currFrame) const {
  assert(std::all_of(currFrame->rois.begin(), currFrame->rois.end(),
                     [](auto& roi) { return roi->type() == ROIType::PD; }));
  currFrame->opticalFlowROIProcessStartTime = NowMicros();

  Rect imageSize(0.0f, 0.0f, float(currFrame->width()), float(currFrame->height()));
  float wRatio = (float) currFrame->resizedGrayMat.cols / (float) currFrame->width();
  float hRatio = (float) currFrame->resizedGrayMat.rows / (float) currFrame->height();

  // Prepare prevBoxes to track
  std::vector<BoundingBox> prevBoxes;
  const Frame* prevFrame = currFrame->prevFrame;
  if (prevFrame->useInferenceResultForOF) {
    for (const auto& box : prevFrame->boxes) {
      if (box->confidence < config_.OF_CONF_THRES) continue;
      Rect clippedLoc = box->loc.clip(imageSize);
      if (clippedLoc.minWH < 1) continue;
      BoundingBox prevBox(
          /*oid=*/box->oid,
          /*pid=*/-1,
          /*loc=*/clippedLoc,
          /*confidence=*/box->confidence,
          /*label=*/box->label,
          /*origin=*/Origin::FULL_FRAME);
      prevBox.setSrcROI(box->srcROI());
      prevBoxes.push_back(prevBox);
    }
  } else {
    for (auto& roi : currFrame->prevFrame->rois) {
      BoundingBox prevBox(
          /*oid=*/roi->oid,
          /*pid=*/-1,
          /*loc=*/roi->origLoc,
          /*confidence=*/1,
          /*label=*/roi->label(),
          /*origin=*/roi->origin());
      prevBox.setSrcROI(roi.get());
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
  for (const auto& scaledRect : scaledPrevRects) {
    assert(0 <= scaledRect.l && scaledRect.l <= scaledRect.r &&
        scaledRect.r <= currFrame->resizedGrayMat.cols &&
        0 <= scaledRect.t && scaledRect.t <= scaledRect.b &&
        scaledRect.b <= currFrame->resizedGrayMat.rows);
  }

  // OF ROI Extraction with Scaled Image
  std::vector<RectTrackingResult> trackingResults = extractOF(
      prevFrame->resizedGrayMat,
      currFrame->resizedGrayMat,
      scaledPrevRects,
      &currFrame->numFeaturePoints);
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
        /*oid=*/box.oid,
        /*frame=*/currFrame,
        /*origLoc=*/currLoc,
        /*type=*/ROIType::OF,
        /*origin=*/box.origin,
        /*label=*/box.label,
        /*ofFeatures=*/ofFeatures,
        /*confidence=*/box.confidence,
        /*padding=*/config_.OF_ROI_PADDING));
  }
  currFrame->opticalFlowROIProcessEndTime = NowMicros();

  currFrame->eatPDROIs(config_.PD_EAT_OVERLAP_THRES);
  currFrame->filterPDROIs(config_.PD_FILTER_OVERLAP_THRES);
  currFrame->assignPDROIIDs();

  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeROIs(ROIResizer_, executionType_, roiSize_);
  currFrame->resizeEndTime = NowMicros();

  currFrame->mergeROIStartTime = NowMicros();
  currFrame->resetMergedROIs();
  if (config_.MERGE) {
    currFrame->mergeMergedROIs(config_.MAX_MERGE_SIZE);
  }
  currFrame->sortMergedROIs();
  currFrame->mergeROIEndTime = NowMicros();
}

} // namespace md
