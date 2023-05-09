#include "mondrian/Utils.hpp"

#include <map>
#include <set>

#include "mondrian/DataType.hpp"

namespace md {

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

std::set<int> range(int startIndex, int endIndex) {
  std::set<int> range;
  for (int i = startIndex; i < endIndex; i++) {
    range.insert(i);
  }
  return range;
}

const char* COCO_LABELS[] = {
    "person",
    "bicycle",
    "car",
    "motorbike",
    "aeroplane",
    "bus",
    "train",
    "truck",
    "boat",
    "traffic light",
    "fire hydrant",
    "stop sign",
    "parking meter",
    "bench",
    "bird",
    "cat",
    "dog",
    "horse",
    "sheep",
    "cow",
    "elephant",
    "bear",
    "zebra",
    "giraffe",
    "backpack",
    "umbrella",
    "handbag",
    "tie",
    "suitcase",
    "frisbee",
    "skis",
    "snowboard",
    "sports ball",
    "kite",
    "baseball bat",
    "baseball glove",
    "skateboard",
    "surfboard",
    "tennis racket",
    "bottle",
    "wine glass",
    "cup",
    "fork",
    "knife",
    "spoon",
    "bowl",
    "banana",
    "apple",
    "sandwich",
    "orange",
    "broccoli",
    "carrot",
    "hot dog",
    "pizza",
    "donut",
    "cake",
    "chair",
    "sofa",
    "potted plant",
    "bed",
    "dining table",
    "toilet",
    "tvmonitor",
    "laptop",
    "mouse",
    "remote",
    "keyboard",
    "cell phone",
    "microwave",
    "oven",
    "toaster",
    "sink",
    "refrigerator",
    "book",
    "clock",
    "vase",
    "scissors",
    "teddy bear",
    "hair drier",
    "toothbrush"
};

} // namespace md
