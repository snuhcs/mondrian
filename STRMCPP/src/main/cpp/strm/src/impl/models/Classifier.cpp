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
  inference(mat);

  float widthRatio = (float) originalWidth / (float) inputSize;
  float heightRatio = (float) originalHeight / (float) inputSize;

  std::vector<BoundingBox> detections;
  for (int i = 0; i < outputSize; i++) {
    const float* boxes = getBoxes(i);
    const float* confidences = getConfidences(i);
    float maxConfidence = 0;
    int maxLabel = -1;
    for (int label = 0; label < numLabels; label++) {
      if (maxConfidence < confidences[label]) {
        maxLabel = label;
        maxConfidence = confidences[label];
      }
    }
    if (maxLabel == 0 && maxConfidence > confidenceThreshold) {
      float x = boxes[0];
      float y = boxes[1];
      float w = boxes[2];
      float h = boxes[3];
      detections.emplace_back(Rect(
          std::max(0, (int) ((x - w / 2) * widthRatio)),
          std::max(0, (int) ((y - h / 2) * heightRatio)),
          std::min(originalWidth, (int) ((x + w / 2) * widthRatio)),
          std::min(originalHeight, (int) ((y + h / 2) * heightRatio))),
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
