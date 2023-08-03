#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

const cv::TermCriteria ROIExtractor::CRITERIA = cv::TermCriteria(
    /*type=*/cv::TermCriteria::COUNT + cv::TermCriteria::EPS,
    /*maxCount=*/10,
    /*epsilon=*/0.03);

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
  LOGD("ROIExtractor::enqueue(%4d) "
       "// PDWaiting=%lu OFWaiting=%lu OFProcessed=%lu | OFWaiting.front()=%d",
       frame->frameIndex,
       PDWaiting_.size(),
       OFWaiting_.size(),
       OFProcessed_.size(),
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
}

std::condition_variable& ROIExtractor::cv() {
  return cv_;
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
  LOGD("ROIExtractor::collectFrames(%4d) "
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
        OFProcessed_.insert(frame);
        frame->isROIsReady = true;
      } else {
        // When scheduling is triggered while OF processing
        frame->rois.erase(std::remove_if(
            frame->rois.begin(), frame->rois.end(),
            [](const std::unique_ptr<ROI>& roi) {
              return roi->type == OF;
            }), frame->rois.end());
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

void ROIExtractor::processPD(Frame* currFrame) {
  assert(currFrame->rois.empty());
  currFrame->pixelDiffROIProcessStartTime = NowMicros();
  const Frame* prevFrame = currFrame;
  for (int i = 0; i < config_.PD_INTERVAL; i++) {
    if (prevFrame->frameIndex == 0) break;
    prevFrame = prevFrame->prevFrame;
  }
  assert(prevFrame != nullptr && prevFrame != currFrame);
  std::vector<Rect> resizedPDRects = extractPD(prevFrame->resizedGrayMat, currFrame->resizedGrayMat);
  std::vector<Rect> pdRects;
  std::transform(
      resizedPDRects.begin(), resizedPDRects.end(), std::back_inserter(pdRects),
      [currFrame](Rect& resizedPDRect) -> Rect {
        float wRatio = (float) currFrame->resizedGrayMat.cols / (float) currFrame->width();
        float hRatio = (float) currFrame->resizedGrayMat.rows / (float) currFrame->height();
        return {resizedPDRect.l / wRatio, resizedPDRect.t / hRatio,
                resizedPDRect.r / wRatio, resizedPDRect.b / hRatio};
      });
  std::for_each(pdRects.begin(), pdRects.end(),
                [currFrame](Rect& pdRect){
    float widthRatio = float(currFrame->resizedGrayMat.cols) / float(currFrame->width());
    float heightRatio = float(currFrame->resizedGrayMat.rows) / float(currFrame->height());
    pdRect.x *= widthRatio;
    pdRect.y *= heightRatio;
    pdRect.width *= widthRatio;
    pdRect.height *= heightRatio;
  });
  getPixelDiffROIs(currFrame, targetSize_,
                   config_.MAX_PD_ROI_SIZE, config_.MIN_PD_ROI_SIZE,
                   currFrame->rois);
  currFrame->pixelDiffROIProcessEndTime = NowMicros();
}

void ROIExtractor::getPixelDiffROIs(Frame* currFrame, const cv::Size& targetSize,
                                    const float maxPDROISize, const float minPDROISize,
                                    std::vector<std::unique_ptr<ROI>>& outChildROIs) const {

  // Find {PD_INTERVAL}th previous frame. If not available, use farthest frame.


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

void ROIExtractor::processOF(Frame* currFrame) {
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
