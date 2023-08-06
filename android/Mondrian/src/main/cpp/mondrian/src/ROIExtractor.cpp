#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "mondrian/Log.hpp"

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
  std::lock_guard<std::mutex> queueLock(mtx_);
  PDWaiting_.insert(frame);
  cv_.notify_all();
  LOGD("[ROIExtractor] enqueue "
       "// OFWaiting=%lu OFProcessing=%lu OFProcessed=%d | OFWaiting.front()=%d",
       OFWaiting_.size(), OFProcessing_.size(),
       std::accumulate(OFProcessed_.begin(), OFProcessed_.end(), 0,
                       [](int sum, const auto& pair) { return sum + pair.second.size(); }),
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
}

void ROIExtractor::notify() {
  cv_.notify_all();
}

MultiStream ROIExtractor::collectFrames(int currID) {
  std::unique_lock<std::mutex> queueLock(mtx_);
  cv_.wait(queueLock, [this]() { return OFProcessing_.empty(); });

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
  auto getOFJob = [this]() {
    bool ofFrameExists = !OFWaiting_.empty();
    bool readyForOFExtraction = ofFrameExists && (*OFWaiting_.begin())->readyForOFExtraction();
    if (ofFrameExists && readyForOFExtraction) {
      return *OFWaiting_.begin();
    } else {
//      LOGD("XXX %s %s %s",
//           ofFrameExists
//           ? "ofFrameExists"
//           : "no of frames ",
//           notFullyPacked
//           ? "notFullyPacked"
//           : "fullyPacked   ",
//           readyForOFExtraction
//           ? "readyForOFExtraction    "
//           : "not readyForOFExtraction");
//      if (ofFrameExists && !readyForOFExtraction) {
//        Frame* frame = *OFWaiting_.begin();
//        LOGD("XXX Frame %d, %s %s",
//             frame->frameIndex,
//             frame->useInferenceResultForOF
//             ? "useInferenceResultForOF"
//             : "useOFResult            ",
//             frame->readyForOFExtraction()
//             ? "readyForOFExtraction    "
//             : "not readyForOFExtraction");
//      }
      return (Frame*) nullptr;
    }
  };

  while (true) {
    bool isOF = false;
    Frame* frame = nullptr;

    std::unique_lock<std::mutex> queueLock(mtx_);
    cv_.wait(queueLock, [this, &isOF, &frame, &getPDJob, &getOFJob]() {
      if (stop_) return true;
      frame = getOFJob();
      if (frame != nullptr) {
        isOF = true;
        return true;
      }
      frame = getPDJob();
      if (frame != nullptr) {
        isOF = false;
        return true;
      }
      return false;
    });
    time_us start = NowMicros();

    if (stop_) {
      cv_.notify_all();
      return;
    }

    if (isOF) {
      frame->OFExtractorID = extractorId;
      OFWaiting_.erase(frame);
      OFProcessing_.insert(frame);
    } else {
      frame->PDExtractorID = extractorId;
      PDWaiting_.erase(frame);
      PDProcessing_.insert(frame);
    }
    queueLock.unlock();
    cv_.notify_all();

    if (isOF) {
      processOF(frame);
    } else {
      processPD(frame);
    }

    if (isOF) {
      postprocessOF(frame);
    } else {
      queueLock.lock();
      PDProcessing_.erase(frame);
      OFWaiting_.insert(frame);
      queueLock.unlock();
    }
    time_us end = NowMicros();
    if (!isOF) {
      LOGD("[ROIExtractor] PDTime=%lld // vid=%d fid=%d #PDROIs=%lu",
           end - start, frame->vid, frame->frameIndex,
           std::count_if(frame->rois.begin(), frame->rois.end(),
                         [](auto& roi) { return roi->type == PD; }));
    } else {
      LOGD("[ROIExtractor] OFTime=%lld // vid=%d fid=%d #OFROIs=%lu resizeTime=%lld mergeTime=%lld",
           end - start, frame->vid, frame->frameIndex,
           std::count_if(frame->rois.begin(), frame->rois.end(),
                         [](auto& roi) { return roi->type == OF; }),
           frame->resizeEndTime - frame->resizeStartTime,
           frame->mergeROIEndTime - frame->mergeROIStartTime);
    }

    cv_.notify_all();
  }
}

void ROIExtractor::postprocessOF(Frame* currFrame) {
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

  if (executionType_ == EMULATED_BATCH) {
    assert(std::all_of(currFrame->boxesIfLast.begin(), currFrame->boxesIfLast.end(),
                       [this](const auto& box) {
                         return std::max(box.first, box.second) <= ROISize_;
                       }));
  }

  std::lock_guard<std::mutex> queueLock(mtx_);
  OFProcessing_.erase(currFrame);
  if (currFrame->extractOFAgain) {
    OFWaiting_.insert(currFrame);
  } else {
    OFProcessed_[currFrame->vid].insert(currFrame);
    currFrame->isROIsReady = true;
  }
}

void ROIExtractor::processPD(Frame* currFrame) {
  currFrame->pixelDiffROIProcessStartTime = NowMicros();
  getPixelDiffROIs(currFrame, targetSize_,
                   config_.MAX_PD_ROI_SIZE, config_.MIN_PD_ROI_SIZE,
                   currFrame->rois);
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

void ROIExtractor::getPixelDiffROIs(Frame* currFrame, const cv::Size& targetSize,
                                    const float maxPDROISize, const float minPDROISize,
                                    std::vector<std::unique_ptr<ROI>>& outChildROIs) const {

  // Find {PD_INTERVAL}th previous frame. If not available, use farthest frame.
  const Frame* prevFrame = currFrame;
  for (int i = 0; i < config_.PD_INTERVAL; i++) {
    assert(prevFrame != nullptr);
    if (prevFrame->prevFrame == nullptr) {
      break;
    }
    prevFrame = prevFrame->prevFrame;
  }

  float widthRatio = float(targetSize.width) / float(prevFrame->width());
  float heightRatio = float(targetSize.height) / float(prevFrame->height());
//  LOGD("XXX %d %d => %d %d | %f %f",
//       prevFrame->width(), prevFrame->height(),
//       targetSize.width, targetSize.height,
//       widthRatio, heightRatio);

  const cv::Mat& prevImage = prevFrame->resizedGrayMat;
  const cv::Mat& currImage = currFrame->resizedGrayMat;

  cv::Mat mat = calculateDiffAndThreshold(prevImage, currImage);
  cannyEdgeDetection(mat);

  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;

  cv::findContours(mat, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // replaces get boxes from contours.
  std::vector<Rect> boxes;
  for (const std::vector<cv::Point>& contour : contours) {
    double approxDistance = cv::arcLength(contour, true) * 0.02;
    std::vector<cv::Point> approxCurve;
    cv::approxPolyDP(contour, approxCurve, approxDistance, true);
    cv::Rect2f box = cv::boundingRect(approxCurve);
    assert(box.width > 0 && box.height > 0);
    if (minPDROISize <= std::min(box.width, box.height)
        && std::max(box.width, box.height) <= maxPDROISize) {
      Rect _box(box.x / widthRatio,
                box.y / heightRatio,
                (box.x + box.width) / widthRatio,
                (box.y + box.height) / heightRatio);
//      LOGD("XXX: %f %f %f %f | %f %f | %f %f %f %f",
//           box.x, box.y, box.x + box.width, box.y + box.height,
//           widthRatio, heightRatio,
//           _box.l, _box.t, _box.r, _box.b);
      assert(0 <= _box.l && 0 <= _box.t
                 && _box.r <= prevFrame->width() && _box.b <= prevFrame->height());
      boxes.push_back(_box);
    }
  }

  for (const Rect& box : boxes) {
    if (std::min(box.w, box.h) >= 1.0f) {
      outChildROIs.emplace_back(new ROI(
          nullptr,
          INVALID_ID,
          currFrame,
          box,
          PD,
          O_PD,
          -1,
          OFFeatures(),
          ROI::INVALID_CONF));
    }
  }
}

cv::Mat ROIExtractor::calculateDiffAndThreshold(
    const cv::Mat& prevMat, const cv::Mat& currMat) {
  cv::Mat diff;
  cv::absdiff(prevMat, currMat, diff);
  cv::dilate(diff, diff,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(-1, -1),
             2);
  cv::threshold(diff, diff, 35, 255, cv::THRESH_BINARY);
  return diff;
}

void ROIExtractor::cannyEdgeDetection(cv::Mat mat) {
  cv::Canny(mat, mat, 120, 255, 3, false);
  cv::dilate(mat, mat,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(-1, -1),
             2);
}

} // namespace md
