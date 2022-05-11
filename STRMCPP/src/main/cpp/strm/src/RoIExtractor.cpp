#include "strm/RoIExtractor.hpp"

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"

namespace rm {

RoIExtractor::RoIExtractor(RoIExtractorConfig config)
    : mConfig(config),
      mTargetSize(cv::Size(mConfig.EXTRACTION_RESIZE_WIDTH,
                           mConfig.EXTRACTION_RESIZE_HEIGHT)) {
  LOGD("RoIExtractor()");
}

bool RoIExtractor::useOpticalFlowRoIs() const {
  return mConfig.OF_ROI;
}

void RoIExtractor::process(
    const std::pair<std::pair<std::shared_ptr<Frame>, std::shared_ptr<Frame>>, std::vector<BoundingBox>>& item) const {
  Frame* prevFrame = item.first.first.get();
  Frame* currFrame = item.first.second.get();
  LOGD("RoIExtractor::process((%s, %d), (%s, %d), %d)", prevFrame->key.c_str(),
       prevFrame->frameIndex,
       currFrame->key.c_str(), currFrame->frameIndex, (int) item.second.size());

  std::vector<RoI> rois;
  if (mConfig.OF_ROI) {
    std::vector<BoundingBox> prevResults;
    for (const BoundingBox& bbx : item.second) {
      if (bbx.confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        prevResults.push_back(bbx);
      }
    }
    currFrame->opticalFlowRoIProcessStartTime = NowMicros();
    std::vector<RoI> opticalFlowRoIs = getOpticalFlowRoIs(prevFrame, currFrame,
                                                          prevResults, mTargetSize);
    currFrame->opticalFlowRoIProcessEndTime = NowMicros();
    rois.insert(rois.end(), opticalFlowRoIs.begin(), opticalFlowRoIs.end());
    currFrame->opticalFlowRoIs = opticalFlowRoIs;
  }
  if (mConfig.PD_ROI) {
    currFrame->pixelDiffRoIProcessStartTime = NowMicros();
    std::vector<RoI> pixelDiffRoIs = getPixelDiffRoIs(prevFrame, currFrame,
                                                      mTargetSize, mConfig.MIN_ROI_AREA);
    currFrame->pixelDiffRoIProcessEndTime = NowMicros();
    rois.insert(rois.end(), pixelDiffRoIs.begin(), pixelDiffRoIs.end());
  }
  LOGD("Before Merge: %lu", rois.size());
  if (mConfig.MERGE_ROI) {
    currFrame->mergeRoIStartTime = NowMicros();
    mergeSingleFrameRoIs(rois, currFrame, mConfig.MERGE_THRESHOLD, mConfig.MAX_MERGED_ROI_SIZE);
    currFrame->mergeRoIEndTime = NowMicros();
  }
  LOGD("After  Merge: %lu", rois.size());
  currFrame->rois = rois;
}

void RoIExtractor::mergeSingleFrameRoIs(std::vector<RoI>& rois, const Frame* frame,
                                        const float mergeThreshold, const int maxMergedRoISize) {
  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < rois.size(); i++) {
      for (j = i + 1; j < rois.size(); j++) {
        const RoI& roi0 = rois[i];
        const RoI& roi1 = rois[j];
        int intersection = roi0.location.intersection(roi1.location);
        bool isNotOverlap = (float) intersection / (float) roi0.getArea() < mergeThreshold
                            && (float) intersection / (float) roi1.getArea() < mergeThreshold;
        bool isTooLarge = (std::max(roi0.location.right, roi1.location.right) -
                           std::min(roi0.location.left, roi1.location.left) > maxMergedRoISize) ||
                          (std::max(roi0.location.bottom, roi1.location.bottom) -
                           std::min(roi0.location.top, roi1.location.top) > maxMergedRoISize);
        if (isNotOverlap || isTooLarge) {
          continue;
        }
        updated = true;
        break;
      }
      if (updated) {
        break;
      }
    }
    if (!updated) {
      break;
    }
    const RoI& roi0 = rois[i];
    const RoI& roi1 = rois[j];
    int newLeft = std::min(roi0.location.left, roi1.location.left);
    int newTop = std::min(roi0.location.top, roi1.location.top);
    int newRight = std::max(roi0.location.right, roi1.location.right);
    int newBottom = std::max(roi0.location.bottom, roi1.location.bottom);
    assert(newLeft < newRight && newTop < newBottom);
    Rect newLocation(newLeft, newTop, newRight, newBottom);
    RoI::Type roiType = roi0.type == RoI::Type::OF || roi1.type == RoI::Type::OF
                        ? RoI::Type::OF
                        : RoI::Type::PD;
    std::string roiLabel = roi0.labelName.empty() || roi1.labelName.empty()
                           || roi0.labelName != roi1.labelName
                           ? "" : roi0.labelName;
    rois.emplace_back(frame, newLocation, roiType, roiLabel,
                      std::min(std::max(roi0.location.width(), roi0.location.height()),
                               std::max(roi1.location.width(), roi1.location.height())));
    rois.erase(rois.begin() + j);
    rois.erase(rois.begin() + i);
  }
}

std::vector<RoI> RoIExtractor::getOpticalFlowRoIs(
        const Frame* prevFrame, Frame *currFrame,
        const std::vector<BoundingBox>& boundingBoxes, const cv::Size& targetSize) {
  int width = currFrame->mat.cols;
  int height = currFrame->mat.rows;

  std::vector<Rect> boundingRects;
  boundingRects.reserve(boundingBoxes.size());
  for (const auto& bbx : boundingBoxes) {
    boundingRects.emplace_back(bbx.location);
  }

  std::vector<RoI> opticalFlowRoIs;
  if (!boundingBoxes.empty()) {
    const std::vector<std::pair<int, int>>& shifts = getBoundingBoxShifts(
            prevFrame->mat, currFrame->mat, boundingRects, targetSize, currFrame);
    for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
      const std::pair<int, int>& shift = shifts.at(boxIndex);
      const BoundingBox& box = boundingBoxes.at(boxIndex);
      const Rect& loc = box.location;
      int newLeft = std::max(0, loc.left + shift.first);
      int newTop = std::max(0, loc.top + shift.second);
      int newRight = std::min(width, loc.right + shift.first);
      int newBottom = std::min(height, loc.bottom + shift.second);
      if (newLeft < newRight && newTop < newBottom) {
        opticalFlowRoIs.emplace_back(
            currFrame, Rect(newLeft, newTop, newRight, newBottom),
            RoI::Type::OF, box.labelName);
      }
    }
  }
  return opticalFlowRoIs;
}

std::vector<std::pair<int, int>>
RoIExtractor::getBoundingBoxShifts(const cv::Mat &prevImage, const cv::Mat &currImage,
                                   const std::vector<Rect> &boundingBoxes,
                                   const cv::Size &targetSize, Frame *currFrame) {
  assert(!prevImage.empty() && !currImage.empty());

  const time_us t0 = NowMicros();
  cv::Mat prevMat = prevImage.clone();
  cv::Mat currMat = currImage.clone();

  std::vector<cv::Point2f> p0;
  std::vector<cv::Point2f> p1;

  std::vector<uchar> status;
  std::vector<float> err;

  const time_us t1 = NowMicros();
  cv::cvtColor(prevMat, prevMat, cv::COLOR_BGR2GRAY);
  cv::cvtColor(currMat, currMat, cv::COLOR_BGR2GRAY);

  const time_us t2 = NowMicros();
  cv::resize(prevMat, prevMat, targetSize);
  cv::resize(currMat, currMat, targetSize);

  const time_us t3 = NowMicros(); // < 50us
  std::vector<cv::Point> centroids;
  for (const Rect& bbx : boundingBoxes) {
    int bbxCenterX = bbx.left + bbx.width() / 2;
    int bbxCenterY = bbx.top + bbx.height() / 2;
    cv::Point bbxCentroidPoints(bbxCenterX * targetSize.width / currImage.cols,
                                bbxCenterY * targetSize.height / currImage.rows);
    centroids.push_back(bbxCentroidPoints);
    // might not work... replaces p0.fromList(centroids); p0 is Point2f,
    p0.push_back(bbxCentroidPoints);
  }

  const time_us t4 = NowMicros(); // actually only calcOpticalFlowPyrLK
  cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);
  cv::calcOpticalFlowPyrLK(prevMat, currMat, p0, p1, status, err, cv::Size(15, 15), 2, criteria);

  const time_us t5 = NowMicros(); // < 50us
  uchar StatusArr[status.size()];
  std::copy(status.begin(), status.end(), StatusArr);

  cv::Point p1Arr[p1.size()];
  std::copy(p1.begin(), p1.end(), p1Arr);

  std::vector<std::pair<int, int>> shifts;
  for (int pointIdx = 0; pointIdx < centroids.size(); pointIdx++) {
    if (StatusArr[pointIdx] == 1) {
      shifts.emplace_back((int) ((p1Arr[pointIdx].x - centroids.at(pointIdx).x)
                                 * currImage.cols / targetSize.width),
                          (int) ((p1Arr[pointIdx].y - centroids.at(pointIdx).y)
                                 * currImage.rows / targetSize.height));
    } else {
      shifts.emplace_back(0, 0);
    }
  }
  const time_us t6 = NowMicros();

  currFrame->of0 = t0;
  currFrame->of1 = t1;
  currFrame->of2 = t2;
  currFrame->of3 = t3;
  currFrame->of4 = t4;
  currFrame->of5 = t5;
  currFrame->of6 = t6;

  return shifts;
}

std::vector<RoI> RoIExtractor::getPixelDiffRoIs(const Frame* prevFrame, Frame *currFrame,
                                                const cv::Size& targetSize, const int mixRoIArea) {
  const time_us t0 = NowMicros();
  cv::Mat prevMat = prevFrame->mat.clone();
  cv::Mat currMat = currFrame->mat.clone();

  const time_us t1 = NowMicros();
  cv::cvtColor(prevMat, prevMat, cv::COLOR_BGR2GRAY);
  cv::cvtColor(currMat, currMat, cv::COLOR_BGR2GRAY);

  const time_us t2 = NowMicros();
  cv::resize(prevMat, prevMat, targetSize);
  cv::resize(currMat, currMat, targetSize);

  const time_us t3 = NowMicros();
  cv::Mat mat = calculateDiffAndThreshold(prevMat, currMat);

  const time_us t4 = NowMicros();
  cannyEdgeDetection(mat);

  const time_us t5 = NowMicros();
  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;

  const time_us t6 = NowMicros();
  cv::findContours(mat, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  const time_us t7 = NowMicros();
  // replaces get boxes from contours.
  std::vector<Rect> boxes;
  for (const std::vector<cv::Point>& contour : contours) {
    double approxDistance = cv::arcLength(contour, true) * 0.02;
    std::vector<cv::Point> approxCurve;
    cv::approxPolyDP(contour, approxCurve, approxDistance, true);
    cv::Rect box = cv::boundingRect(approxCurve);
    if (box.area() >= mixRoIArea) {
      boxes.emplace_back(box.x * currFrame->mat.cols / targetSize.width,
                         box.y * currFrame->mat.rows / targetSize.height,
                         (box.x + box.width) * currFrame->mat.cols / targetSize.width,
                         (box.y + box.height) * currFrame->mat.rows / targetSize.height);
    }
  }

  const time_us t8 = NowMicros();
  std::vector<RoI> rois;
  rois.reserve(boxes.size());
  for (Rect& box : boxes) {
    rois.emplace_back(currFrame, box, RoI::PD, "");
  }

  const time_us t9 = NowMicros();

  currFrame->pd0 = t0;
  currFrame->pd1 = t1;
  currFrame->pd2 = t2;
  currFrame->pd3 = t3;
  currFrame->pd4 = t4;
  currFrame->pd5 = t5;
  currFrame->pd6 = t6;
  currFrame->pd7 = t7;
  currFrame->pd8 = t8;
  currFrame->pd9 = t9;
  return rois;
}

cv::Mat RoIExtractor::calculateDiffAndThreshold(
    const cv::Mat& prevMat, const cv::Mat& currMat) {
  cv::Mat diff;
  cv::absdiff(prevMat, currMat, diff);
  for (int i = 0; i < 3; i++) {
    cv::dilate(diff, diff, cv::getStructuringElement(
        cv::MORPH_RECT, cv::Size(3, 3)), cv::Point(0, 0), i + 1);
  }
  cv::threshold(diff, diff, 30, 255, cv::THRESH_BINARY);
  return diff;
}

void RoIExtractor::cannyEdgeDetection(cv::Mat mat) {
  cv::GaussianBlur(mat, mat, cv::Size(3, 3), 0);
  cv::Canny(mat, mat, 120, 255, 3, true);
  cv::dilate(mat, mat, cv::getStructuringElement(
      cv::MORPH_RECT, cv::Size(5, 5)), cv::Point(0, 0), 1);
}

} // namespace rm
