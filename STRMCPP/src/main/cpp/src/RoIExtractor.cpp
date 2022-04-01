#include "strm/RoIExtractor.hpp"

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"

namespace rm {

bool RoIExtractor::useOpticalFlowRoIs() const {
  return mConfig.OF_ROI;
}

void
RoIExtractor::process(const std::pair<std::pair<Frame*, Frame*>, std::vector<BoundingBox>>& item) {
  LOGD("RoIExtractor::process");
  Frame* prevFrame = item.first.first;
  Frame* currFrame = item.first.second;

  std::vector<RoI> rois;
  if (mConfig.OF_ROI) {
    std::vector<BoundingBox> prevResults;
    for (const BoundingBox& bbx : item.second) {
      if (bbx.confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        prevResults.push_back(bbx);
      }
    }
    std::vector<RoI> opticalFlowRoIs = getOpticalFlowRoIs(prevFrame, currFrame,
                                                          prevResults, mTargetSize);
    rois.insert(rois.end(), opticalFlowRoIs.begin(), opticalFlowRoIs.end());
    currFrame->opticalFlowRoIs = opticalFlowRoIs;
  }
  if (mConfig.PD_ROI) {
    std::vector<RoI> pixelDiffRoIs = getPixelDiffRoIs(prevFrame, currFrame, mTargetSize);
    rois.insert(rois.end(), pixelDiffRoIs.begin(), pixelDiffRoIs.end());
  }
  if (mConfig.MERGE_ROI) {
    mergeSingleFrameRoIs(currFrame, rois, mConfig.MERGE_THRESHOLD);
  }
  currFrame->rois = rois;
}

void RoIExtractor::mergeSingleFrameRoIs(
        const Frame* frame, std::vector<RoI> rois, float mergeThreshold) {
  while (true) {
    bool updated = false;
    for (auto it0 = rois.begin(); it0 != rois.end(); it0++) {
      for (auto it1 = it0 + 1; it1 != rois.end(); it1++) {
        const RoI& roi0 = *it0;
        const RoI& roi1 = *it1;
        float intersection = 0.0; // TODO: add STRMUtils.box_intersection(roi0.location, roi1.location);
        if (intersection / (float) roi0.getArea() < mergeThreshold
            && intersection / (float) roi1.getArea() < mergeThreshold) {
          continue;
        }
        int newLeft = std::min(roi0.location.left, roi1.location.left);
        int newTop = std::min(roi0.location.top, roi1.location.top);
        int newRight = std::max(roi0.location.right, roi1.location.right);
        int newBottom = std::max(roi0.location.bottom, roi1.location.bottom);
        if (newLeft >= newRight || newTop >= newBottom) {
          continue;
        }
        Rect newLocation(newLeft, newTop, newRight, newBottom);
        RoI::Type roiType = roi0.type == RoI::Type::OF || roi1.type == RoI::Type::OF
                            ? RoI::Type::OF
                            : RoI::Type::PD;
        std::string roiLabel = roi0.labelName.empty() || roi1.labelName.empty()
                               || roi0.labelName != roi1.labelName
                               ? "" : roi0.labelName;
        rois.erase(it0);
        rois.erase(it1);
        rois.emplace_back(frame, newLocation, roiType, roiLabel,
                          std::min(std::max(roi0.location.width(), roi0.location.height()),
                                   std::max(roi1.location.width(), roi1.location.height())));
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
  }
}

std::vector<RoI> RoIExtractor::getOpticalFlowRoIs(
        const Frame* prevFrame, const Frame* currFrame,
        const std::vector<BoundingBox>& boundingBoxes, const cv::Size& targetSize) {
  int width = currFrame->mat->cols;
  int height = currFrame->mat->rows;

  std::vector<Rect> boundingRects;
  boundingRects.reserve(boundingBoxes.size());
  for (const auto& bbx : boundingBoxes) {
    boundingRects.emplace_back(bbx.location);
  }

  std::vector<RoI> opticalFlowRoIs;
  if (!boundingBoxes.empty()) {
    const std::vector<std::pair<int, int>>& shifts = getBoundingBoxShifts(
            prevFrame->mat, currFrame->mat, boundingRects, targetSize);
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
        const cv::Mat* prevImage, const cv::Mat* currImage,
        const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(prevImage != nullptr && currImage != nullptr);
  cv::Mat prevMat = prevImage->clone();
  cv::Mat currMat = currImage->clone();

  std::vector<cv::Point2f> p0;
  std::vector<cv::Point2f> p1;

  std::vector<uchar> status;
  std::vector<float> err;

  cv::cvtColor(prevMat, prevMat, cv::COLOR_BGR2GRAY);
  cv::cvtColor(currMat, currMat, cv::COLOR_BGR2GRAY);

  cv::resize(prevMat, prevMat, targetSize);
  cv::resize(currMat, currMat, targetSize);

  std::vector<cv::Point> centroids;
  for (const Rect& bbx : boundingBoxes) {
    int bbxCenterX = bbx.left + bbx.width() / 2;
    int bbxCenterY = bbx.top + bbx.height() / 2;
    cv::Point bbxCentroidPoints(bbxCenterX * targetSize.width / currImage->cols,
                                bbxCenterY * targetSize.height / currImage->rows);
    centroids.push_back(bbxCentroidPoints);
    // might not work... replaces p0.fromList(centroids); p0 is Point2f,
    p0.push_back(bbxCentroidPoints);
  }
  cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);
  cv::calcOpticalFlowPyrLK(prevMat, currMat, p0, p1, status, err, cv::Size(15, 15), 2, criteria);

  uchar StatusArr[status.size()];
  std::copy(status.begin(), status.end(), StatusArr);

  cv::Point p1Arr[p1.size()];
  std::copy(p1.begin(), p1.end(), p1Arr);

  std::vector<std::pair<int, int>> shifts;
  for (int pointIdx = 0; pointIdx < centroids.size(); pointIdx++) {
    if (StatusArr[pointIdx] == 1) {
      shifts.emplace_back((int) ((p1Arr[pointIdx].x - centroids.at(pointIdx).x) * currImage->cols / targetSize.width),
                          (int) ((p1Arr[pointIdx].y - centroids.at(pointIdx).y) * currImage->rows / targetSize.height));
    } else {
      shifts.emplace_back(0, 0);
    }
  }
  return shifts;
}

std::vector<RoI> RoIExtractor::getPixelDiffRoIs(const Frame* prevFrame, const Frame* currFrame,
                                                const cv::Size& targetSize) {
  cv::Mat prevMat = prevFrame->mat->clone();
  cv::Mat currMat = currFrame->mat->clone();

  cv::cvtColor(prevMat, prevMat, cv::COLOR_BGR2GRAY);
  cv::cvtColor(currMat, prevMat, cv::COLOR_BGR2GRAY);

  cv::resize(prevMat, prevMat, targetSize);
  cv::resize(currMat, currMat, targetSize);

  cv::Mat mat;
  calculateDiffAndThreshold(prevMat, currMat, mat);
  cannyEdgeDetection(mat);

  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;

  cv::findContours(mat, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // replaces get boxes from contours.
  std::vector<Rect> boxes;
  for (const std::vector<cv::Point>& contour : contours) {
    cv::Rect box = cv::boundingRect(contour);
    boxes.emplace_back(box.x, box.y, box.x + box.width, box.y + box.height);
  }

  std::vector<RoI> rois;
  rois.reserve(boxes.size());
  for (Rect& box : boxes) {
    rois.emplace_back(currFrame, box, RoI::PD, "");
  }
  return rois;
}

void RoIExtractor::calculateDiffAndThreshold(
        const cv::Mat& frame0, const cv::Mat& frame1, cv::Mat& diff) {
  cv::absdiff(frame0, frame1, diff);
  for (int i = 0; i < 3; i++) {
    cv::dilate(diff, diff, cv::getStructuringElement(
            cv::MORPH_RECT, cv::Size(3, 3)), cv::Point(0, 0), i + 1);
  }
  cv::threshold(diff, diff, 30, 255, cv::THRESH_BINARY);
}

void RoIExtractor::cannyEdgeDetection(cv::Mat& mat) {
cv::GaussianBlur(mat, mat, cv::Size(3, 3),
0);
cv::Canny(mat, mat,
120, 255, 3, true);
cv::dilate(mat, mat, cv::getStructuringElement(
        cv::MORPH_RECT, cv::Size(
5,5)), cv::Point(0,0), 1);
}

} // namespace rm
