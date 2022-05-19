#include "strm/RoIExtractor.hpp"

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"

namespace rm {

RoIExtractor::RoIExtractor(RoIExtractorConfig config, const ResizeProfile* resizeProfile,
                           const RoIPrioritizer* roIPrioritizer)
    : mConfig(config),
      mTargetSize(cv::Size(mConfig.EXTRACTION_RESIZE_WIDTH, mConfig.EXTRACTION_RESIZE_HEIGHT)),
      mResizeProfile(resizeProfile), mRoIPrioritizer(roIPrioritizer) {
  LOGD("RoIExtractor()");
}

bool RoIExtractor::useOpticalFlowRoIs() const {
  return mConfig.OF_ROI;
}

std::vector<RoI> RoIExtractor::process(Frame* prevFrame, Frame* currFrame,
                                       const std::vector<BoundingBox>& prevResults) const {
  LOGD("RoIExtractor::process((%s, %d), (%s, %d), %d)", prevFrame->key.c_str(),
       prevFrame->frameIndex, currFrame->key.c_str(), currFrame->frameIndex,
       (int) prevResults.size());

  std::vector<RoI> rois;

  // Preprocess matrices
  if (mConfig.OF_ROI || mConfig.PD_ROI) {
    if (prevFrame->preProcessedMat.empty()) {
      cv::Mat mat = prevFrame->mat;
      cv::resize(mat, mat, mTargetSize);
      cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
      prevFrame->preProcessedMat = mat;
    }
    if (currFrame->preProcessedMat.empty()) {
      cv::Mat mat = currFrame->mat;
      cv::resize(mat, mat, mTargetSize);
      cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
      currFrame->preProcessedMat = mat;
    }
  }

  if (mConfig.OF_ROI) {
    std::vector<BoundingBox> filteredPrevResults;
    for (const BoundingBox& bbx : prevResults) {
      if (bbx.confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        filteredPrevResults.push_back(bbx);
      }
    }
    currFrame->opticalFlowRoIProcessStartTime = NowMicros();
    std::vector<RoI> opticalFlowRoIs = getOpticalFlowRoIs(prevFrame, currFrame,
                                                          filteredPrevResults, mTargetSize);
    currFrame->opticalFlowRoIProcessEndTime = NowMicros();
    currFrame->opticalFlowRoIs = opticalFlowRoIs;
    rois.insert(rois.end(), opticalFlowRoIs.begin(), opticalFlowRoIs.end());
  }

  if (mConfig.PD_ROI) {
    currFrame->pixelDiffRoIProcessStartTime = NowMicros();
    std::vector<RoI> pixelDiffRoIs = getPixelDiffRoIs(prevFrame, currFrame,
                                                      mTargetSize, mConfig.MIN_ROI_AREA);
    currFrame->pixelDiffRoIProcessEndTime = NowMicros();
    rois.insert(rois.end(), pixelDiffRoIs.begin(), pixelDiffRoIs.end());
  }
  prevFrame->preProcessedMat.release();

  currFrame->resizeRoIStartTime = NowMicros();
  for (auto& roi : rois) {
    roi.scale = mResizeProfile->getScale(roi.labelName,
                                         roi.location.width(), roi.location.height());
  }
  currFrame->resizeRoIEndTime = NowMicros();

  LOGD("Before Merge: %lu", rois.size());
  if (mConfig.MERGE_ROI) {
    currFrame->mergeRoIStartTime = NowMicros();
    mergeSingleFrameRoIs(rois, currFrame, mConfig.MERGE_THRESHOLD, mConfig.MAX_MERGED_ROI_SIZE);
    currFrame->mergeRoIEndTime = NowMicros();
  }
  LOGD("After  Merge: %lu", rois.size());

  std::sort(rois.begin(), rois.end(),
            [this](const RoI& lhs, const RoI& rhs) -> bool {
              return mRoIPrioritizer->priority(lhs) < mRoIPrioritizer->priority(rhs);
            });
  return rois;
}

void RoIExtractor::mergeSingleFrameRoIs(std::vector<RoI>& rois, const Frame* frame,
                                        const float mergeThreshold, const int maxMergedRoISize) {
  while (true) {
    bool updated = false;
    int i, j;
    int newLeft, newTop, newRight, newBottom;
    for (i = 0; i < rois.size(); i++) {
      for (j = i + 1; j < rois.size(); j++) {
        const RoI& roi0 = rois[i];
        const RoI& roi1 = rois[j];
        int intersection = roi0.location.intersection(roi1.location);
        if ((float) intersection / (float) roi0.getArea() < mergeThreshold &&
            (float) intersection / (float) roi1.getArea() < mergeThreshold) {
          continue;
        }
        newLeft = std::min(roi0.location.left, roi1.location.left);
        newTop = std::min(roi0.location.top, roi1.location.top);
        newRight = std::max(roi0.location.right, roi1.location.right);
        newBottom = std::max(roi0.location.bottom, roi1.location.bottom);
        if (newRight - newLeft > maxMergedRoISize || newBottom - newTop > maxMergedRoISize) {
          continue;
        }
        float newArea = (float) ((newRight - newLeft) * (newBottom - newLeft)) *
                        std::max(roi0.scale, roi1.scale);
        float originalArea = (float) roi0.location.area() * roi0.scale
                             + (float) roi1.location.area() * roi1.scale;
        if (newArea >= originalArea) {
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
    assert(newLeft < newRight && newTop < newBottom);
    Rect newLocation(newLeft, newTop, newRight, newBottom);
    RoI::Type roiType = roi0.type == RoI::Type::OF || roi1.type == RoI::Type::OF
                        ? RoI::Type::OF
                        : RoI::Type::PD;
    std::string roiLabel = roi0.labelName.empty() || roi1.labelName.empty()
                           || roi0.labelName != roi1.labelName
                           ? "" : roi0.labelName;
    rois.emplace_back(frame, newLocation, roiType, roiLabel, std::max(roi0.scale, roi1.scale));
    assert(j > i);
    rois.erase(rois.begin() + j);
    rois.erase(rois.begin() + i);
  }
}

std::vector<RoI> RoIExtractor::getOpticalFlowRoIs(
    const Frame* prevFrame, const Frame* currFrame,
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
        prevFrame, currFrame, boundingRects, targetSize);
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

std::vector<std::pair<int, int>> RoIExtractor::getBoundingBoxShifts(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->preProcessedMat.empty() && !currFrame->preProcessedMat.empty());

  const cv::Mat& prevImage = prevFrame->preProcessedMat;
  const cv::Mat& currImage = currFrame->preProcessedMat;

  std::vector<cv::Point2f> p0;
  std::vector<cv::Point2f> p1;

  std::vector<uchar> status;
  std::vector<float> err;

  std::vector<cv::Point> centroids;
  for (const Rect& bbx : boundingBoxes) {
    int bbxCenterX = bbx.left + bbx.width() / 2;
    int bbxCenterY = bbx.top + bbx.height() / 2;
    cv::Point bbxCentroidPoints(bbxCenterX * targetSize.width / currFrame->mat.cols,
                                bbxCenterY * targetSize.height / currFrame->mat.rows);
    centroids.push_back(bbxCentroidPoints);
    // might not work... replaces p0.fromList(centroids); p0 is Point2f,
    p0.push_back(bbxCentroidPoints);
  }
  cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);
  cv::calcOpticalFlowPyrLK(prevImage, currImage, p0, p1, status, err, cv::Size(15, 15), 2,
                           criteria);

  uchar StatusArr[status.size()];
  std::copy(status.begin(), status.end(), StatusArr);

  cv::Point p1Arr[p1.size()];
  std::copy(p1.begin(), p1.end(), p1Arr);

  std::vector<std::pair<int, int>> shifts;
  for (int pointIdx = 0; pointIdx < centroids.size(); pointIdx++) {
    if (StatusArr[pointIdx] == 1) {
      shifts.emplace_back((int) ((p1Arr[pointIdx].x - centroids.at(pointIdx).x)
                                 * currFrame->mat.cols / targetSize.width),
                          (int) ((p1Arr[pointIdx].y - centroids.at(pointIdx).y)
                                 * currFrame->mat.rows / targetSize.height));
    } else {
      shifts.emplace_back(0, 0);
    }
  }
  return shifts;
}

std::vector<RoI> RoIExtractor::getPixelDiffRoIs(const Frame* prevFrame, const Frame* currFrame,
                                                const cv::Size& targetSize, const int mixRoIArea) {
  cv::Mat mat = calculateDiffAndThreshold(prevFrame->preProcessedMat, currFrame->preProcessedMat);
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
    cv::Rect box = cv::boundingRect(approxCurve);
    if (box.area() >= mixRoIArea) {
      boxes.emplace_back(box.x * currFrame->mat.cols / targetSize.width,
                         box.y * currFrame->mat.rows / targetSize.height,
                         (box.x + box.width) * currFrame->mat.cols / targetSize.width,
                         (box.y + box.height) * currFrame->mat.rows / targetSize.height);
    }
  }

  std::vector<RoI> rois;
  rois.reserve(boxes.size());
  for (Rect& box : boxes) {
    rois.emplace_back(currFrame, box, RoI::PD, "");
  }
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
