#include "mondrian/Utils.hpp"

#include <sched.h>

#include <map>
#include <set>

#include "opencv2/video/tracking.hpp"

#include "mondrian/DataType.hpp"

namespace md {

cv::Vec3b yuv2rgb(uchar Y, uchar U, uchar V) {
  int C = Y - 16;
  int D = U - 128;
  int E = V - 128;

  uchar R = cv::saturate_cast<uchar>((298 * C + 409 * E + 128) >> 8);
  uchar G = cv::saturate_cast<uchar>((298 * C - 100 * D - 208 * E + 128) >> 8);
  uchar B = cv::saturate_cast<uchar>((298 * C + 516 * D + 128) >> 8);

  return {R, G, B};
}

cv::Mat extractRgbROIFromYuvMat(cv::Mat yuvMat, int l, int t, int r, int b) {
  assert(yuvMat.rows % 3 == 0);
  const int height = yuvMat.rows * 2 / 3;

  cv::Mat rgbROI(b - t, r - l, CV_8UC3);
  for (int y = t; y < b; y++) {
    for (int x = l; x < r; x++) {
      uchar Y = yuvMat.at<uchar>(y, x);
      uchar U = yuvMat.at<uchar>(height + y / 2, x - x % 2);
      uchar V = yuvMat.at<uchar>(height + y / 2, x - x % 2 + 1);
      rgbROI.at<cv::Vec3b>(y - t, x - l) = yuv2rgb(Y, U, V);
    }
  }
  return rgbROI;
}

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
                                          const std::vector<Rect>& prevRects,
                                          bool useCenter,
                                          int* numFeaturePoints) {
  assert(prevGrayMat.size() == nextGrayMat.size());

  std::vector<int> startEndIndices = {0};
  std::vector<cv::Point2f> prevPoints;
  for (const Rect& prevRect : prevRects) {
    int l = std::max((int) prevRect.l, 0);
    int t = std::max((int) prevRect.t, 0);
    int r = std::min((int) prevRect.r, prevGrayMat.cols);
    int b = std::min((int) prevRect.b, prevGrayMat.rows);
    std::vector<cv::Point2f> aRectPoints;
    if (!useCenter) { // Add offset to corner prevPoints.
      cv::goodFeaturesToTrack(prevGrayMat(cv::Rect(l, t, r - l, b - t)), aRectPoints,
          /*maxCorners=*/50,
          /*qualityLevel=*/0.01,
          /*minDistance=*/5.0,
          /*mask=*/cv::Mat(),
          /*blockSize=*/3,
          /*useHarrisDetector=*/false,
          /*k=*/0.03);
      std::for_each(aRectPoints.begin(), aRectPoints.end(), [l, t](cv::Point2f& p) {
        p.x += (float) l;
        p.y += (float) t;
      });
    }
    if (aRectPoints.empty()) { // Use center point if no corners are found.
      aRectPoints.emplace_back((prevRect.l + prevRect.r) / 2,
                               (prevRect.t + prevRect.b) / 2);
    }
    startEndIndices.push_back(startEndIndices.back() + int(aRectPoints.size()));
    prevPoints.insert(prevPoints.end(), aRectPoints.begin(), aRectPoints.end());
  }
  assert(startEndIndices.back() == prevPoints.size());

  if (numFeaturePoints != nullptr) {
    *numFeaturePoints = prevPoints.size();
  }

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
                             const int numLabels, const float iouThres) {
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
        if (max.loc.iou(it->loc) >= iouThres) {
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
         const int numLabels, const float iouThres) {
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
        if (boxes[max]->loc.iou(boxes[*it]->loc) >= iouThres) {
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

bool sched_setaffinity_big() {
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(6, &set);
  CPU_SET(5, &set);
  CPU_SET(4, &set);
  int return_code = sched_setaffinity(0, sizeof(cpu_set_t), &set);
  return return_code == 0;
}

bool sched_setaffinity_big_or_primary() {
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(7, &set);
  CPU_SET(6, &set);
  CPU_SET(5, &set);
  CPU_SET(4, &set);
  int return_code = sched_setaffinity(0, sizeof(cpu_set_t), &set);
  return return_code == 0;
}

bool sched_setaffinity_little() {
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(3, &set);
  CPU_SET(2, &set);
  CPU_SET(1, &set);
  CPU_SET(0, &set);
  int return_code = sched_setaffinity(0, sizeof(cpu_set_t), &set);
  return return_code == 0;
}

} // namespace md
