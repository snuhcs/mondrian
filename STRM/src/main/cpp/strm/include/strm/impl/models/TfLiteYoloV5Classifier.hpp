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

 protected:
  void singleInference() const override;

 private:
  cv::Mat preprocess(const cv::Mat& mat) override;

  void inference(const cv::Mat& mat) override;

  const float* getBox(const int i) const override;

  float getObjectConfidence(const int i) const override;

  const float* getClassConfidences(const int i) const override;

  Rect reconstructBox(float x, float y, float w, float h, float imageWidth, float imageHeight) override;

  TfLiteDelegate* delegate;
  std::unique_ptr<tflite::Interpreter> interpreter;

  int resizeWidth;
  int resizeHeight;
  int left;
  int top;
  int right;
  int bottom;

  float* input; // 1 x inputSize.height x inputSize.width x 3
  float* outputs; // 1 x outputSize x 85 (boxes, maxConf, confidences)
};

} // namespace rm

#endif // IMPL_TFLITE_YOLO_V5_CLASSIFIER_HPP_
