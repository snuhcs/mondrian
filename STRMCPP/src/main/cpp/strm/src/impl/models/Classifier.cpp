#include "strm/impl/models/Classifier.hpp"

#include <set>

#include "strm/Utils.hpp"

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
    if (1 <= maxLabel && maxLabel <= 8 && maxConfidence > confidenceThreshold) {
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

} // namespace rm
