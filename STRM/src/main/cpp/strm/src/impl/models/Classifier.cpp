#include "strm/impl/models/Classifier.hpp"

#include <set>

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

Classifier::Classifier(const int numLabels, const int inputSize, const int outputSize,
                       const float confidenceThreshold, const float iouThreshold, Device device)
    : numLabels(numLabels), inputSize(inputSize, inputSize), outputSize(outputSize),
      confidenceThreshold(confidenceThreshold), iouThreshold(iouThreshold), device(device) {}

Result Classifier::recognizeImage(const cv::Mat& rgbMat) {
  cv::Mat preprocessedMat = preprocess(rgbMat);

  time_us start = NowMicros();
  inference(preprocessedMat);
  time_us end = NowMicros();

  // Exponential smoothing
  inferenceTime = (3 * (end - start) + 7 * inferenceTime) / 10;
  LOGV("Inference time: %lld us", inferenceTime);

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
          UNASSIGNED_ID,
          reconstructBox(float(box[0]),
                         float(box[1]),
                         float(box[2]),
                         float(box[3]),
                         rgbMat.cols, rgbMat.rows),
          maxConfidence, maxLabel, origin_Null));
    }
  }
  return {nms(detections, numLabels, iouThreshold), {start, end}, device};
}

const cv::Size& Classifier::getInputSize() const {
  return inputSize;
}

time_us Classifier::getInferenceTime() const {
  return inferenceTime;
}

void Classifier::setInferenceTime(time_us currInferenceTime) {
  inferenceTime = currInferenceTime;
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

time_us Classifier::profileInferenceTime(int profileWarmups, int profileRuns) const {
  for (int i = 0; i < profileWarmups; i++) {
    singleInference();
  }
  time_us start = NowMicros();
  for (int i = 0; i < profileRuns; i++) {
    singleInference();
  }
  time_us end = NowMicros();
  return (end - start) / profileRuns;
}

} // namespace rm
