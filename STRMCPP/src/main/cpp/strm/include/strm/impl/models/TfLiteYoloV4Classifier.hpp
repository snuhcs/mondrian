#ifndef IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
#define IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_

#include "strm/impl/models/Classifier.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/c/common.h"

namespace rm {

class TfLiteYoloV4Classifier : public Classifier {
 public:
  TfLiteYoloV4Classifier(int inputSize, float confidenceThreshold, float iouThreshold, bool isTiny);

  ~TfLiteYoloV4Classifier();

  long long int profileInferenceTime() override;

 private:
  cv::Mat preprocess(const cv::Mat& mat) override;

  void inference(const cv::Mat& mat) override;

  const float* getBox(const int i) const override;

  const float getObjectConfidence(const int i) const override;

  const float* getClassConfidences(const int i) const override;

  Rect reconstructBox(float x, float y, float w, float h, int imageWidth, int imageHeight) override;

  TfLiteDelegate* delegate;
  std::unique_ptr<tflite::Interpreter> interpreter;

  float* input; // 1 x inputSize.height x inputSize.width x 3
  float* boxes; // 1 x outputSize x 4
  float* confidences; // 1 x outputSize x numLabels
};

} // namespace rm

#endif // IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
