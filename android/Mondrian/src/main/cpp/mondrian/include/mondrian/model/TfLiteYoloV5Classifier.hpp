#ifndef MODEL_TFLITE_YOLO_V5_CLASSIFIER_HPP_
#define MODEL_TFLITE_YOLO_V5_CLASSIFIER_HPP_

#include "mondrian/model/Classifier.hpp"

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/c/common.h"

namespace md {

class TfLiteYoloV5Classifier : public Classifier {
 public:
  TfLiteYoloV5Classifier(const std::string& modelName,
                         const int inputSize,
                         const bool forFullFrame,
                         const std::string& dataset,
                         const float confThres,
                         const float iouThres);

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

  inline static const int OUTPUT_ELEMS = 85;
};

} // namespace md

#endif // MODEL_TFLITE_YOLO_V5_CLASSIFIER_HPP_
