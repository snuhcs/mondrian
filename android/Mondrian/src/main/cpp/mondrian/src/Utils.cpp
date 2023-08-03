#include "mondrian/Utils.hpp"

#include <map>
#include <set>

#include "opencv2/video/tracking.hpp"

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
