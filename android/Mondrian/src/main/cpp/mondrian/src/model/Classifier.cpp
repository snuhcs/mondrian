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
  // Latency Breakdown on Galaxy S22
  // Input Size  640   1024   1280
  // Preprocess  750us 1500us 2300us
  // Inference   130ms 480ms  470ms
  // Postprocess 14ms  38ms   60ms
  // NMS         10us  12us   13us
  time_us start = NowMicros();
  cv::Mat inputTensor = preprocess(rgbMat);
  time_us preprocessTime = NowMicros();
  inference(inputTensor);
  time_us inferenceTime = NowMicros();
  std::vector<BoundingBox> boxesAll = postprocess(rgbMat.cols, rgbMat.rows);
  time_us postprocessTime = NowMicros();
  std::vector<BoundingBox> boxesNms = nms(boxesAll, numLabels, iouThreshold);
  time_us nmsTime = NowMicros();

  // TODO: remove android log and add detailed latency to frame.csv
  LOGD("Latency with %dx%d input on %s: "
       "preprocess %7lld us, inference %7lld us, postprocess %7lld us, nms %7lld us",
       inputSize.width, inputSize.height, str(device()).c_str(),
       preprocessTime - start,
       inferenceTime - preprocessTime,
       postprocessTime - inferenceTime,
       nmsTime - postprocessTime);

  return boxesNms;
}

std::vector<BoundingBox> Classifier::postprocess(int width, int height) const {
  std::vector<BoundingBox> boxes;
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
      Rect rect = reconstructBox(float(box[0]), float(box[1]),
                                 float(box[2]), float(box[3]),
                                 float(width), float(height));
      if (rect.l <= rect.r && rect.t <= rect.b) {
        boxes.emplace_back(INVALID_ID, rect, maxConfidence, maxLabel, O_INVALID);
      }
    }
  }
  return boxes;
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
