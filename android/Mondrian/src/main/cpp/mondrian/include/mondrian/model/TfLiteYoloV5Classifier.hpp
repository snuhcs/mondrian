#ifndef MODEL_TFLITE_YOLO_V5_CLASSIFIER_HPP_
#define MODEL_TFLITE_YOLO_V5_CLASSIFIER_HPP_

#include "mondrian/model/Classifier.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/c/common.h"

namespace md {

class TfLiteYoloV5Classifier : public Classifier {
 public:
  TfLiteYoloV5Classifier(std::string dataset, int inputSize,
                         float confThres, float iouThres,
                         bool isTiny, bool forFullFrame);

 private:
  cv::Mat preprocess(const cv::Mat& rgbMat) const override;

  void inference(const cv::Mat& inputTensor) const override;

  std::vector<BoundingBox> postprocess(int width, int height) const override;

  Device device() const override;

  Rect reconstructBox(float x, float y, float w, float h,
                      float imageWidth, float imageHeight) const;

  TfLiteDelegate* delegate;
  std::unique_ptr<tflite::Interpreter> interpreter;

  float* input; // 1 x inputSize.height x inputSize.width x 3
  float* outputs; // 1 x outputSize x 85 (boxes, maxConf, confidences)
};

} // namespace md

#endif // MODEL_TFLITE_YOLO_V5_CLASSIFIER_HPP_
