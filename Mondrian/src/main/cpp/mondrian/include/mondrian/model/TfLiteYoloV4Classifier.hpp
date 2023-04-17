#ifndef MODEL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
#define MODEL_TFLITE_YOLO_V4_CLASSIFIER_HPP_

#include "mondrian/model/Classifier.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/c/common.h"

namespace md {

class TfLiteYoloV4Classifier : public Classifier {
 public:
  TfLiteYoloV4Classifier(std::string dataset, int inputSize,
                         float confidenceThreshold, float iouThreshold,
                         bool isTiny, bool forFullFrame);

  ~TfLiteYoloV4Classifier();

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

  float* input; // 1 x inputSize.height x inputSize.width x 3
  float* boxes; // 1 x outputSize x 4
  float* confidences; // 1 x outputSize x numLabels
};

} // namespace md

#endif // MODEL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
