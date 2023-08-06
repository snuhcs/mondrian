#include "mondrian/Utils.hpp"

#include <map>
#include <set>

#include "opencv2/video/tracking.hpp"

#include "mondrian/DataType.hpp"

namespace md {

std::vector<Rect> extractPD(const cv::Mat& prevGrayMat, const cv::Mat& nextGrayMat) {
  assert(prevGrayMat.size() == nextGrayMat.size());
  cv::Mat mat;
  cv::absdiff(prevGrayMat, nextGrayMat, mat);
  cv::dilate(mat, mat,
      /*kernel=*/cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
      /*anchor=*/cv::Point(-1, -1),
      /*iterations==*/2);
  cv::threshold(mat, mat,
      /*thresh=*/35,
      /*maxval=*/255,
      /*type=*/cv::THRESH_BINARY);
  cv::Canny(mat, mat,
      /*threshold1==*/120,
      /*threshold2==*/255,
      /*apertureSize=*/3,
      /*L2gradient=*/false);
  cv::dilate(mat, mat,
      /*kernel=*/cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
      /*anchor=*/cv::Point(-1, -1),
      /*iterations==*/2);

  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;
  cv::findContours(mat, contours, hierarchy,
      /*mode=*/cv::RETR_EXTERNAL,
      /*method=*/cv::CHAIN_APPROX_SIMPLE);

  std::vector<Rect> boxes;
  for (const std::vector<cv::Point>& contour : contours) {
    std::vector<cv::Point> approxCurve;
    cv::approxPolyDP(contour, approxCurve,
        /*epsilon=*/cv::arcLength(contour, true) * 0.02,
        /*closed=*/true);
    cv::Rect2f box = cv::boundingRect(approxCurve);
    assert(box.width > 0 && box.height > 0);
    boxes.emplace_back(
        box.x,
        box.y,
        box.x + box.width,
        box.y + box.height);
  }

  for (const auto& box : boxes) {
    assert(0 <= box.l
               && 0 <= box.t
               && box.r <= prevGrayMat.cols
               && box.b <= prevGrayMat.rows);
  }
  return boxes;
}

std::vector<RectTrackingResult> extractOF(const cv::Mat& prevGrayMat,
                                          const cv::Mat& nextGrayMat,
                                          const std::vector<Rect>& prevRects) {
  assert(prevGrayMat.size() == nextGrayMat.size());

  std::vector<int> startEndIndices = {0};
  std::vector<cv::Point2f> prevPoints;
  for (const Rect& prevRect : prevRects) {
    int l = std::max((int) prevRect.l, 0);
    int t = std::max((int) prevRect.t, 0);
    int r = std::min((int) prevRect.r, prevGrayMat.cols);
    int b = std::min((int) prevRect.b, prevGrayMat.rows);
    std::vector<cv::Point2f> aRectPoints;
    cv::goodFeaturesToTrack(prevGrayMat(cv::Rect(l, t, r - l, b - t)), aRectPoints,
        /*maxCorners=*/50,
        /*qualityLevel=*/0.01,
        /*minDistance=*/5.0,
        /*mask=*/cv::Mat(),
        /*blockSize=*/3,
        /*useHarrisDetector=*/false,
        /*k=*/0.03);
    if (!aRectPoints.empty()) { // Add offset to corner prevPoints.
      std::for_each(aRectPoints.begin(), aRectPoints.end(), [l, t](cv::Point2f& p) {
        p.x += (float) l;
        p.y += (float) t;
      });
    } else { // Use center point if no corners are found.
      aRectPoints.emplace_back((prevRect.l + prevRect.r) / 2,
                               (prevRect.t + prevRect.b) / 2);
    }
    startEndIndices.push_back(startEndIndices.back() + int(aRectPoints.size()));
    prevPoints.insert(prevPoints.end(), aRectPoints.begin(), aRectPoints.end());
  }
  assert(startEndIndices.back() == prevPoints.size());

  std::vector<cv::Point2f> nextPoints;
  std::vector<uchar> statuses;
  std::vector<float> errors;
  cv::calcOpticalFlowPyrLK(prevGrayMat, nextGrayMat, prevPoints, nextPoints, statuses, errors,
      /*winSize=*/cv::Size(15, 15),
      /*maxLevel=*/2,
      /*criteria=*/cv::TermCriteria(
          /*type=*/cv::TermCriteria::COUNT + cv::TermCriteria::EPS,
          /*maxCount=*/10,
          /*epsilon=*/0.03));
  assert(prevPoints.size() == nextPoints.size());
  assert(prevPoints.size() == statuses.size());
  assert(prevPoints.size() == errors.size());

  std::vector<RectTrackingResult> trackingResults;
  for (int i = 0; i < startEndIndices.size() - 1; i++) {
    int startIndex = startEndIndices[i];
    int endIndex = startEndIndices[i + 1];
    RectTrackingResult trackingResult;
    for (int j = startIndex; j < endIndex; j++) {
      trackingResult.prevPoints.push_back(prevPoints[j]);
      trackingResult.nextPoints.push_back(nextPoints[j]);
      trackingResult.statuses.push_back((int) statuses[j]);
      trackingResult.errors.push_back(errors[j]);
    }
    trackingResults.push_back(trackingResult);
  };
  return trackingResults;
}

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold) {
  std::vector<BoundingBox> nmsList;

  auto comp = [](const BoundingBox& l, const BoundingBox& r) -> bool {
    return l.confidence > r.confidence;
  };
  for (int k = 0; k < numLabels; k++) {
    if (k != 0) {
      continue;
    }
    std::set<BoundingBox, decltype(comp)> sortedBoxes(comp);

    for (const BoundingBox& box : boxes) {
      if (box.label == k) {
        sortedBoxes.insert(box);
      }
    }

    while (!sortedBoxes.empty()) {
      auto startIt = sortedBoxes.begin();
      const BoundingBox& max = *startIt;
      nmsList.push_back(max);
      sortedBoxes.erase(startIt);

      for (auto it = sortedBoxes.begin(); it != sortedBoxes.end();) {
        if (max.loc.iou(it->loc) >= iouThreshold) {
          it = sortedBoxes.erase(it);
        } else {
          it++;
        }
      }
    }
  }
  return nmsList;
}

void nms(std::vector<std::unique_ptr<BoundingBox>>& boxes,
         const int numLabels, const float iouThreshold) {
  std::set<int> nmsIndices;

  auto comp = [&boxes](int l, int r) -> bool {
    return boxes[l]->confidence > boxes[r]->confidence;
  };
  for (int k = 0; k < numLabels; k++) {
    std::set<int, decltype(comp)> sortedBoxes(comp);

    for (int i = 0; i < boxes.size(); i++) {
      if (boxes[i]->label == k) {
        sortedBoxes.insert(i);
      }
    }

    while (!sortedBoxes.empty()) {
      auto startIt = sortedBoxes.begin();
      int max = *startIt;
      nmsIndices.insert(max);
      sortedBoxes.erase(startIt);

      for (auto it = sortedBoxes.begin(); it != sortedBoxes.end();) {
        if (boxes[max]->loc.iou(boxes[*it]->loc) >= iouThreshold) {
          it = sortedBoxes.erase(it);
        } else {
          it++;
        }
      }
    }
  }
  for (int i = (int) boxes.size() - 1; i >= 0; i--) {
    if (nmsIndices.find(i) == nmsIndices.end()) {
      boxes.erase(boxes.begin() + i);
    }
  }
}

} // namespace md
