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
  auto outputs = inference(mat).first;  // OUTPUT_WIDTH * (xywh, conf, cls confs) => 85

  std::vector<BoundingBox> detections;
  for (int i = 0; i < outputSize; i++) {
    float maxConfidence = 0;
    int maxLabel = -1;
    for (int label = 0; label < numLabels; label++) {
      float confidence = outputs[i * (5 + numLabels) + 5 + label];
      if (maxConfidence < confidence) {
        maxLabel = label;
        maxConfidence = confidence;
      }
    }
    if (maxLabel == 0 && maxConfidence > confidenceThreshold) {
      float xPos = outputs[i * 85 + 0];
      float yPos = outputs[i * 85 + 1];
      float w = outputs[i * 85 + 2];
      float h = outputs[i * 85 + 3];
      detections.emplace_back(Rect(
          std::max(0, (int) ((xPos - w / 2) * (float) originalWidth)),
          std::max(0, (int) ((yPos - h / 2) * (float) originalHeight)),
          std::min(originalWidth, (int) ((xPos + w / 2) * (float) originalWidth)),
          std::min(originalHeight, (int) ((yPos + h / 2) * (float) originalHeight))),
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
