#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

const cv::TermCriteria ROIExtractor::CRITERIA = cv::TermCriteria(
    cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);

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
      targetSize_(cv::Size(int(config.EXTRACTION_RESIZE_WIDTH),
                           int(config.EXTRACTION_RESIZE_HEIGHT))),
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

void ROIExtractor::processOF(Frame* currFrame) {
  currFrame->rois.erase(std::remove_if(
      currFrame->rois.begin(), currFrame->rois.end(),
      [](const std::unique_ptr<ROI>& roi) {
        return roi->type == OF;
      }), currFrame->rois.end());
  const Frame* prevFrame = currFrame->prevFrame;
  Rect imageSize(0.0f, 0.0f, float(currFrame->width()), float(currFrame->height()));
  std::vector<BoundingBox> reliablePrevBoxes;
  if (prevFrame->useInferenceResultForOF) {
    for (const std::unique_ptr<BoundingBox>& box : prevFrame->boxes) {
      if (box->confidence > config_.OF_CONF_THRESHOLD) {
        BoundingBox reliableBox(
            box->id,
            box->loc.clip(imageSize),
            box->confidence,
            box->label,
            /*origin=*/O_PACKED_CANVAS);
        reliableBox.srcROI = box->srcROI;
        reliablePrevBoxes.push_back(reliableBox);
      }
    }
  } else {
    for (auto& roi : currFrame->prevFrame->rois) {
      BoundingBox reliableBox(
          roi->id,
          roi->origLoc,
          /*confidence=*/1,
          roi->label,
          roi->origin);
      reliableBox.srcROI = roi.get();
      reliablePrevBoxes.push_back(reliableBox);
    }
  }
  currFrame->opticalFlowROIProcessStartTime = NowMicros();
  getOpticalFlowROIs(prevFrame, currFrame, reliablePrevBoxes, targetSize_, currFrame->rois);
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

void ROIExtractor::getOpticalFlowROIs(const Frame* prevFrame, Frame* currFrame,
                                      const std::vector<BoundingBox>& prevBoxes,
                                      const cv::Size& targetSize,
                                      std::vector<std::unique_ptr<ROI>>& outChildROIs) {
  std::vector<Rect> prevRects;
  prevRects.reserve(prevBoxes.size());
  for (const auto& bbx : prevBoxes) {
    prevRects.emplace_back(bbx.loc);
  }

  Rect imageSize(0.0f, 0.0f, float(currFrame->width()), float(currFrame->height()));

  if (!prevBoxes.empty()) {
    const std::vector<OFFeatures>& ofFeatures = opticalFlowTracking(
        prevFrame, currFrame, prevRects, targetSize);
    assert(ofFeatures.size() == prevBoxes.size());
    for (int boxIndex = 0; boxIndex < prevBoxes.size(); boxIndex++) {
      const BoundingBox& box = prevBoxes[boxIndex];
      const Rect& loc = box.loc;
      const OFFeatures& of = ofFeatures[boxIndex];
      float x = of.shiftAvg.first;
      float y = of.shiftAvg.second;
      Rect newLoc(loc.l + x, loc.t + y, loc.r + x, loc.b + y);
      outChildROIs.emplace_back(new ROI(
          box.srcROI, box.id, currFrame, newLoc.clip(imageSize),
          OF, box.origin, box.label, of, box.confidence));
    }
  }
}

std::vector<OFFeatures> ROIExtractor::opticalFlowTracking(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->resizedGrayMat.empty());
  assert(!currFrame->resizedGrayMat.empty());
  assert(prevFrame->resizedGrayMat.channels() == currFrame->resizedGrayMat.channels());

  float widthRatio = float(targetSize.width) / float(prevFrame->width());
  float heightRatio = float(targetSize.height) / float(prevFrame->height());

  const cv::Mat& prevImage = prevFrame->resizedGrayMat;
  const cv::Mat& currImage = currFrame->resizedGrayMat;

  Rect target(0.0f, 0.0f, float(targetSize.width), float(targetSize.height));

  std::vector<int> startEndIndices = {0};
  std::vector<cv::Point2f> inputPoints;
  for (const Rect& bbx : boundingBoxes) {
    Rect roi(bbx.l * widthRatio, bbx.t * heightRatio,
             bbx.r * widthRatio, bbx.b * heightRatio);
    roi = roi.clip(target);

    std::vector<cv::Point2f> points;
    cv::Rect roiBbx = cv::Rect(int(roi.l), int(roi.t), int(roi.w), int(roi.h));
    cv::goodFeaturesToTrack(prevImage(roiBbx), points, 50, 0.01, 5, cv::Mat(), 3, false, 0.03);
    for (cv::Point2f& p : points) {
      p.x += roi.l;
      p.y += roi.t;
    }
    if (points.empty()) {
      points.emplace_back(float(bbx.l + bbx.r) / 2 * widthRatio,
                          float(bbx.t + bbx.b) / 2 * heightRatio);
    }
    startEndIndices.push_back(startEndIndices.back() + int(points.size()));
    inputPoints.insert(inputPoints.end(), points.begin(), points.end());
  }
  assert(startEndIndices.back() == inputPoints.size());

  std::vector<cv::Point2f> outputPoints;
  std::vector<uchar> statuses;
  std::vector<float> errs;
  cv::calcOpticalFlowPyrLK(prevImage, currImage, inputPoints, outputPoints, statuses, errs,
                           cv::Size(15, 15), 2, CRITERIA);
  assert(inputPoints.size() == outputPoints.size());
  assert(inputPoints.size() == statuses.size());
  assert(inputPoints.size() == errs.size());

  std::vector<OFFeatures> ofFeatures;
  for (int i = 0; i < startEndIndices.size() - 1; i++) {
    int startIndex = startEndIndices[i];
    int endIndex = startEndIndices[i + 1];
    std::vector<std::pair<float, float>> _shifts;
    std::vector<int> _statuses;
    std::vector<float> _errs;
    for (int j = startIndex; j < endIndex; j++) {
      float x = (outputPoints[j].x - inputPoints[j].x) / widthRatio;
      float y = (outputPoints[j].y - inputPoints[j].y) / heightRatio;
      _shifts.emplace_back(x, y);
      _statuses.push_back(int(statuses[j]));
      _errs.push_back(errs[j]);
    }
    ofFeatures.emplace_back(_shifts, _statuses, _errs);
  }
  return ofFeatures;
}

} // namespace md
