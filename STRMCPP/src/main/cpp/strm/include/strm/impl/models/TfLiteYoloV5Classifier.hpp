#ifndef IMPL_TFLITE_YOLO_V5_CLASSIFIER_HPP_
#define IMPL_TFLITE_YOLO_V5_CLASSIFIER_HPP_

#include "strm/impl/models/Classifier.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/c/common.h"

namespace rm {

class TfLiteYoloV5Classifier : public Classifier {
 public:
  TfLiteYoloV5Classifier(int inputSize, float confidenceThreshold, float iouThreshold, bool isTiny);

  ~TfLiteYoloV5Classifier();

 private:
  void inference(const cv::Mat& mat) override;
  const float* getBoxes(const int i) const override;
  const float* getConfidences(const int i) const override;

  TfLiteDelegate* delegate;
  std::unique_ptr<tflite::Interpreter> interpreter;

  float* outputs; // 1 x outputSize x 85 (boxes, maxConf, confidences)
};

} // namespace rm

#endif // IMPL_TFLITE_YOLO_V5_CLASSIFIER_HPP_
