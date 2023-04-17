#ifndef MODEL_MNN_YOLO_V4_CLASSIFIER_HPP_
#define MODEL_MNN_YOLO_V4_CLASSIFIER_HPP_

#include "mondrian/model/Classifier.hpp"

#include "MNN/Interpreter.hpp"

namespace md {

class MnnYoloV4Classifier : public Classifier {
 public:
  MnnYoloV4Classifier(std::string dataset, int inputSize,
                      float confidenceThreshold, float iouThreshold,
                      bool isTiny, bool forFullFrame);

  ~MnnYoloV4Classifier();

 protected:
  void singleInference() const override;

 private:
  cv::Mat preprocess(const cv::Mat& mat) override;

  void inference(const cv::Mat& mat) override;

  const float* getBox(const int i) const override;

  float getObjectConfidence(const int i) const override;

  const float* getClassConfidences(const int i) const override;

  Rect reconstructBox(float x, float y, float w, float h, float imageWidth, float imageHeight) override;

  MNN::Interpreter* interpreter;
  MNN::Session* session;

  float* input; // 1 x inputSize.height x inputSize.width x 3
  float* boxes; // 1 x outputSize x 4
  float* confidences; // 1 x outputSize x numLabels

  const std::string INPUT_TENSOR_NAME = "x";
  const std::string OUTPUT_TENSOR_NAME_BOXES = "model/tf.concat_22/concat";
  const std::string OUTPUT_TENSOR_NAME_CONFS = "model/tf.concat_23/concat";
};

} // namespace md

#endif // MODEL_MNN_YOLO_V4_CLASSIFIER_HPP_
