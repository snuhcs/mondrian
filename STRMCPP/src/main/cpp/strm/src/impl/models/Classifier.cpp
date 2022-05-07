#include "strm/impl/models/Classifier.hpp"

#include <set>

namespace rm {

Classifier::Classifier(const int numLabels, const int inputSize, const int outputSize,
                       const float confidenceThreshold, const float iouThreshold)
    : numLabels(numLabels), inputSize(inputSize), outputSize(outputSize),
      confidenceThreshold(confidenceThreshold), iouThreshold(iouThreshold) {}

std::vector<BoundingBox>
Classifier::recognizeImage(const cv::Mat& mat, int originalWidth, int originalHeight) {
  auto[bboxes, confidences] = inference(mat);

  std::vector<BoundingBox> detections;
  for (int i = 0; i < outputSize; i++) {
    float maxConfidence = 0;
    int maxLabel = -1;
    for (int label = 0; label < numLabels; label++) {
      float confidence = confidences[i * numLabels + label];
      if (maxConfidence < confidence) {
        maxLabel = label;
        maxConfidence = confidence;
      }
    }
    if (maxLabel == 0 && maxConfidence > confidenceThreshold) {
      float xPos = bboxes[i * 4 + 0];
      float yPos = bboxes[i * 4 + 1];
      float w = bboxes[i * 4 + 2];
      float h = bboxes[i * 4 + 3];
      detections.emplace_back(Rect(
          std::max(0, (int) ((xPos - w / 2) * (float) originalWidth / (float) inputSize)),
          std::max(0, (int) ((yPos - h / 2) * (float) originalHeight / (float) inputSize)),
          std::min(originalWidth,
                   (int) ((xPos + w / 2) * (float) originalWidth / (float) inputSize)),
          std::min(originalHeight,
                   (int) ((yPos + h / 2) * (float) originalHeight / (float) inputSize))),
                              maxConfidence, "person");
    }
  }
  return nms(detections, numLabels, iouThreshold);
}

int Classifier::getInputSize() const {
  return inputSize;
}

long long Classifier::getInferenceTimeMs() const {
  return inferenceTimeMs;
}

std::vector<BoundingBox> Classifier::nms(const std::vector<BoundingBox>& boxes,
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
//      if (box.labelName == <labelName for k>) {
      if (box.labelName == "person") {
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

} // namespace rm
