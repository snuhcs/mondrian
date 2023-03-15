#ifndef IMPL_TFLITE_YOLO_V5_CLASSIFIER_DSP_HPP_
#define IMPL_TFLITE_YOLO_V5_CLASSIFIER_DSP_HPP_

#include "strm/impl/models/Classifier.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/c/common.h"

namespace rm {

class TfLiteYoloV5ClassifierDSP : public Classifier {
 public:
  TfLiteYoloV5ClassifierDSP(int inputSize, float confidenceThreshold, float iouThreshold,
                            bool isTiny, bool forFullFrame);

  Result recognizeImage(const cv::Mat& mat) override;

 protected:
  void singleInference() const override;

 private:
  cv::Mat preprocess(const cv::Mat& mat) override;

  void inference(const cv::Mat& mat) override;

  Rect reconstructBox(float x, float y, float w, float h, float imageWidth, float imageHeight) override;

  tflite::Interpreter::TfLiteDelegatePtr delegate;
  std::unique_ptr<tflite::Interpreter> interpreter;

  int resizeWidth;
  int resizeHeight;
  int left;
  int top;
  int right;
  int bottom;

  int inputBias;
  int outputBias;
  float inputScale;
  float outputScale;
  int8_t* input; // 1 x inputSize.height x inputSize.width x 3
  int8_t* outputs; // 1 x outputSize x 85 (boxes, maxConf, confidences)
};

} // namespace rm

#endif // IMPL_TFLITE_YOLO_V5_CLASSIFIER_DSP_HPP_
