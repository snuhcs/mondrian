#include "mondrian/model/Classifier.hpp"

#include <set>

#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

Classifier::Classifier(const int numLabels, const int inputSize, const int outputSize,
                       const float confidenceThreshold, const float iouThreshold)
    : numLabels(numLabels), inputSize(inputSize, inputSize), outputSize(outputSize),
      confidenceThreshold(confidenceThreshold), iouThreshold(iouThreshold) {}

std::vector<BoundingBox> Classifier::recognizeImage(const cv::Mat& rgbMat) {
  inference(preprocess(rgbMat));

  std::vector<BoundingBox> detections;
  for (int i = 0; i < outputSize; i++) {
    const float* box = getBox(i);
    const float* classConfidences = getClassConfidences(i);
    float maxConfidence = 0;
    int maxLabel = -1;
    for (int label = 0; label < numLabels; label++) {
      if (maxConfidence < classConfidences[label]) {
        maxLabel = label;
        maxConfidence = classConfidences[label];
      }
    }
    maxConfidence *= getObjectConfidence(i);
    if (maxLabel == 0 && maxConfidence > confidenceThreshold) {
      detections.push_back(BoundingBox(
          INVALID_ID,
          reconstructBox(float(box[0]),
                         float(box[1]),
                         float(box[2]),
                         float(box[3]),
                         rgbMat.cols, rgbMat.rows),
          maxConfidence, maxLabel, O_INVALID));
    }
  }
  return nms(detections, numLabels, iouThreshold);
}

const cv::Size& Classifier::getInputSize() const {
  return inputSize;
}

const float* Classifier::getBox(const int i) const {
  return nullptr;
}

float Classifier::getObjectConfidence(const int i) const {
  return 0.0f;
}

const float* Classifier::getClassConfidences(const int i) const {
  return nullptr;
}

} // namespace md
