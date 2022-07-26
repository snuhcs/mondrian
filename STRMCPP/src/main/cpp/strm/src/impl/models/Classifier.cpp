#include "strm/impl/models/Classifier.hpp"

#include <set>

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

Classifier::Classifier(const int numLabels, const int inputSize, const int outputSize,
                       const float confidenceThreshold, const float iouThreshold)
    : numLabels(numLabels), inputSize(inputSize, inputSize), outputSize(outputSize),
      confidenceThreshold(confidenceThreshold), iouThreshold(iouThreshold) {}

std::vector<BoundingBox>
Classifier::recognizeImage(const cv::Mat& mat) {
  cv::Mat preprocessedMat = preprocess(mat);

  auto start = std::chrono::system_clock::now();
  inference(preprocessedMat);
  auto end = std::chrono::system_clock::now();
  long long currentInferenceTimeMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  float weight = 0.1;
  inferenceTimeMs = weight * currentInferenceTimeMs + (1 - weight) * inferenceTimeMs;
  LOGV("Inference time: %lld ms", inferenceTimeMs);

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
      detections.emplace_back(UNASSIGNED_ID,
                              reconstructBox(box[0], box[1], box[2], box[3], mat.cols, mat.rows),
                              maxConfidence, maxLabel, originNull);
    }
  }
  return nms(detections, numLabels, iouThreshold);
}

const cv::Size& Classifier::getInputSize() const {
  return inputSize;
}

long long Classifier::getInferenceTimeMs() const {
  return inferenceTimeMs;
}

void Classifier::setInferenceTimeMs(long long inferenceTime) {
  inferenceTimeMs = inferenceTime;
}

} // namespace rm
