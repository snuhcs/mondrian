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

 private:
  std::pair<float*, float*> inference(const cv::Mat& mat) override;

  TfLiteDelegate* delegate;
  std::unique_ptr<tflite::Interpreter> interpreter;
};

} // namespace rm

#endif // IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
