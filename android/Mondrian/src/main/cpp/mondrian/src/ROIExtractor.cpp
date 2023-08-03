#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

ROIExtractor::ROIExtractor(const ROIExtractorConfig& config, ROIResizer* roiResizer)
    : config_(config), ROIResizer_(roiResizer), stop_(false) {
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
  cv_.notify_one();
  LOGD("ROIExtractor::enqueue(fid=%d) "
       "// PDWaiting=%lu OFWaiting=%lu OFProcessed=%lu | OFWaiting.front()=%d",
       frame->frameIndex,
       PDWaiting_.size(),
       OFWaiting_.size(),
       OFProcessed_.size(),
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
}

Stream ROIExtractor::collectFrames(int scheduleID) {
  std::lock_guard<std::mutex> lock(mtx_);
  for (Frame* frame : OFProcessing_) {
    frame->reprocessOF = true;
    frame->useInferenceResultForOF = true;
  }
  for (Frame* frame : OFProcessed_) {
    frame->useInferenceResultForOF = true;
  }
  Stream collectedFrames = std::move(OFProcessed_);
  cv_.notify_all();
  LOGD("ROIExtractor::collectFrames(scheduleID=%d) "
       "// PDWaiting=%lu OFWaiting=%lu OFProcessed=%lu collectedFrames=%lu | OFWaiting.front()=%d",
       scheduleID,
       PDWaiting_.size(),
       OFWaiting_.size(),
       OFProcessed_.size(),
       collectedFrames.size(),
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
  return collectedFrames;
}

void ROIExtractor::work(int extractorId) {
  auto getPDJob = [this]() -> Frame* {
    if (!PDWaiting_.empty()) {
      return *PDWaiting_.begin();
    } else {
      return (Frame*) nullptr;
    }
  };

  auto getOFJob = [this]() -> Frame* {
    bool ofFrameExists = !OFWaiting_.empty();
    bool readyForOFExtraction = ofFrameExists && (*OFWaiting_.begin())->readyForOFExtraction();
    if (readyForOFExtraction) {
      return *OFWaiting_.begin();
    } else {
      if (ofFrameExists) {
        Frame* firstFrame = *OFWaiting_.begin();
        LOGD("XXX OF frame exists but not ready for OF extraction // fid=%d, useInfResult=%d",
             firstFrame->frameIndex, firstFrame->useInferenceResultForOF);
      } else {
        LOGD("XXX OF frame not exist");
      }
      return nullptr;
    }
  };

  while (true) {
    bool pd = false;
    Frame* frame = nullptr;

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this, &pd, &frame, &getPDJob, &getOFJob]() -> bool {
      if (stop_) {
        return true;
      }
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

    if (stop_) {
      return;
    }

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
        frame->isROIsReady = true;
        OFProcessed_.insert(frame);
      } else {
        // When scheduling is triggered while OF processing
        frame->resetOFROIExtraction();
        OFWaiting_.insert(frame);
      }
    }
    lock.unlock();
    cv_.notify_one();

    time_us end = NowMicros();
    LOGD("%s took %5lld us // vid=%d fid=%d numExtractedROIs=%lu",
         pd ? "PD" : "OF", end - start, frame->vid, frame->frameIndex,
         std::count_if(frame->rois.begin(), frame->rois.end(),
                       [pd](auto& roi) { return roi->type == (pd ? PD : OF); }));
  }
}

void ROIExtractor::processPD(Frame* currFrame) const {
  assert(currFrame->rois.empty());
  currFrame->prepareResizedGrayMat(config_.EXTRACTION_SIZE);

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
          /*confidence=*/ROI::INVALID_CONF,
          /*padding=*/0));
    }
  }
  currFrame->pixelDiffROIProcessEndTime = NowMicros();
}

void ROIExtractor::processOF(Frame* currFrame) {
  assert(std::all_of(currFrame->rois.begin(), currFrame->rois.end(),
                     [](auto& roi) { return roi->type == PD; }));
  currFrame->opticalFlowROIProcessStartTime = NowMicros();
  Rect imageSize(0, 0, (float) currFrame->width(), (float) currFrame->height());
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
    auto [x, y] = ofFeatures.shiftAvg;
    Rect currLoc = Rect(box.loc.l + x,
                        box.loc.t + y,
                        box.loc.r + x,
                        box.loc.b + y).clip(imageSize);
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
        /*confidence=*/box.confidence,
        /*padding=*/config_.OF_ROI_PADDING));
  }
  currFrame->opticalFlowROIProcessEndTime = NowMicros();

  currFrame->eatPDROIs(config_.PD_EAT_OVERLAP_THRES);
  currFrame->filterPDROIs(config_.PD_FILTER_OVERLAP_THRES);
  currFrame->assignPDROIIDs();

  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeROIs(ROIResizer_);
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
