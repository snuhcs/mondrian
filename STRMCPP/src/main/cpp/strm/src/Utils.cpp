#include "strm/Utils.hpp"

#include <map>
#include <set>

namespace rm {

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold) {
  std::vector<BoundingBox> nmsList;

  auto comp = [](const BoundingBox& l, const BoundingBox& r) -> bool {
    return l.confidence > r.confidence;
  };
  for (int k = 0; k < numLabels; k++) {
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
        if (max.location.iou(it->location) >= iouThreshold) {
          it = sortedBoxes.erase(it);
        } else {
          it++;
        }
      }
    }
  }
  return nmsList;
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

} // namespace rm
