#include "mondrian/ROIExtractor.hpp"

#include <sched.h>

#include <memory>
#include <numeric>
#include <set>
#include <utility>
#include <fstream>
#include <string>

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"
#include "chrome_tracer/tracer.h"

namespace md {

ROIExtractor::ROIExtractor(const ROIExtractorConfig& config,
                           const ExecutionType executionType,
                           const int maxMergeSize,
                           const int roiSize,
                           ROIResizer* roiResizer,
                           chrome_tracer::ChromeTracer* tracer)
    : config_(config),
      executionType_(executionType),
      maxMergeSize_(maxMergeSize),
      roiSize_(roiSize),
      ROIResizer_(roiResizer),
      tracer_(tracer),
      stop_(false),
      isOFProcessing_(false),
      collecting_(false) {
  PDThread_ = std::thread([this]() {
    assert(sched_setaffinity_big_or_primary());
    workPD();
  });
  OFThread_ = std::thread([this]() {
    assert(sched_setaffinity_big_or_primary());
    workOF();
  });
  PostprocessThread_ = std::thread([this]() {
    assert(sched_setaffinity_big_or_primary());
    workPostprocess();
  });
}

ROIExtractor::~ROIExtractor() {
  stop_ = true;
  OFCv_.notify_all();
  PostprocessThread_.join();
  OFThread_.join();
  PDCv_.notify_all();
  PDThread_.join();
}

void ROIExtractor::enqueue(Frame* frame) {
  std::lock_guard<std::mutex> lock(PDMtx_);
  PDWaiting_.push_back(frame);
  PDCv_.notify_one();
}

void ROIExtractor::notify() {
  OFCv_.notify_all();
}

std::list<Frame*> ROIExtractor::collectFrames(int currID) {
  collecting_ = true;

  std::unique_lock<std::mutex> OFLock(OFMtx_);
  OFCv_.wait(OFLock, [this]() {
    return !isOFProcessing_ && !isPostprocessing_ && PostprocessWaiting_.empty();
  });

  time_us scheduledTime = NowMicros();
  for (auto& frame : Processed_) {
    frame->scheduledTime = scheduledTime;
    frame->scheduleID = currID;
    frame->useInferenceResultForOF = true;
  }

  std::list<Frame*> stream = std::move(Processed_);
  while (!Processed_.empty()) {
    Processed_.pop_front();
  }
  collecting_ = false;
  OFCv_.notify_all();
  return stream;
}

void ROIExtractor::workPD() {
  while (!stop_) {
    std::unique_lock<std::mutex> PDLock(PDMtx_);
    PDCv_.wait(PDLock, [this]() { return stop_ || !PDWaiting_.empty(); });
    if (stop_) return;
    Frame* frame = PDWaiting_.front();
    PDWaiting_.pop_front();
    PDLock.unlock();

    int32_t handle = tracer_->BeginEvent(ROIExtractorPDTag_,
                                         "PD" + std::to_string(frame->fid));
    processPD(frame);
    tracer_->EndEvent(ROIExtractorPDTag_, handle);

    std::unique_lock<std::mutex> OFLock(OFMtx_);
    OFWaiting_.push_back(frame);
    OFLock.unlock();
    OFCv_.notify_all();
  }
}

void ROIExtractor::workOF() {
  while (!stop_) {
    std::unique_lock<std::mutex> OFLock(OFMtx_);
    OFCv_.wait(OFLock, [this]() {
      if (stop_) return true;
      return !collecting_
          && !OFWaiting_.empty()
          && OFWaiting_.front()->readyForOFExtraction();
    });
    if (stop_) return;
    std::list<Frame*> frames = std::move(OFWaiting_);
    OFWaiting_.clear();
    isOFProcessing_ = true;
    OFLock.unlock();

    for (auto& frame : frames) {
      int32_t handle = tracer_->BeginEvent(ROIExtractorOFTag_,
                                           "OF" + std::to_string(frame->fid));
      processOF(frame);
      tracer_->EndEvent(ROIExtractorOFTag_, handle);

      OFLock.lock();
      frame->isROIsReady = true;
      PostprocessWaiting_.push_back(frame);
      size_t numOFWaiting = OFWaiting_.size() + frames.back()->fid - frame->fid;
      OFLock.unlock();
      OFCv_.notify_all();

      std::stringstream ss;
      ss << "[ROIExtractor]"
         << " OF         "
         << " [" << frame->vid << ", " << frame->fid << "]"
         << " OFQ=" << numOFWaiting
         << " Track=" << frame->opticalFlowROIProcessEndTime - frame->opticalFlowROIProcessStartTime
         << " Filter=" << frame->filterEndTime - frame->filterStartTime;
      LOGD("%s", ss.str().c_str());
    }

    isOFProcessing_ = false;
    OFCv_.notify_all();
  }
}

void ROIExtractor::workPostprocess() {
  while (!stop_) {
    std::unique_lock<std::mutex> OFLock(OFMtx_);
    OFCv_.wait(OFLock, [this]() { return stop_ || !PostprocessWaiting_.empty(); });
    if (stop_) return;
    Frame* frame = PostprocessWaiting_.front();
    PostprocessWaiting_.pop_front();
    isPostprocessing_ = true;
    OFLock.unlock();

    int32_t handle = tracer_->BeginEvent(ROIExtractorPostprocessTag_,
                                         "ROIPostprocess" + std::to_string(frame->fid));
    postprocess(frame);
    tracer_->EndEvent(ROIExtractorPostprocessTag_, handle);

    OFLock.lock();
    Processed_.push_back(frame);
    isPostprocessing_ = false;
    OFLock.unlock();
    OFCv_.notify_all();

    std::stringstream ss;
    ss << "[ROIExtractor]"
       << " Postprocess"
       << " [" << frame->vid << ", " << frame->fid << "]"
       << "       "
       << " #ROIs=" << std::count_if(frame->rois.begin(), frame->rois.end(),
                                     [](auto& roi) { return roi->type() == ROIType::OF; })
       << " #Features=" << frame->numFeaturePoints
       << " Resize=" << frame->resizeEndTime - frame->resizeStartTime
       << " Merge=" << frame->mergeROIEndTime - frame->mergeROIStartTime;
    LOGD("%s", ss.str().c_str());
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
      config_.OF_TRACK_CENTER,
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

  currFrame->filterStartTime = NowMicros();
  currFrame->eatPDROIs(config_.PD_EAT_OVERLAP_THRES);
  currFrame->filterPDROIs(config_.PD_FILTER_OVERLAP_THRES);
  currFrame->assignPDROIIDs();
  currFrame->filterEndTime = NowMicros();
}

void ROIExtractor::postprocess(md::Frame* currFrame) const {
  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeROIs(ROIResizer_, executionType_, roiSize_);
  currFrame->resizeEndTime = NowMicros();

  currFrame->mergeROIStartTime = NowMicros();
  currFrame->resetMergedROIs();
  if (config_.MERGE) {
    currFrame->mergeMergedROIs(maxMergeSize_);
  }
  currFrame->sortMergedROIs();
  currFrame->mergeROIEndTime = NowMicros();
}

} // namespace md
