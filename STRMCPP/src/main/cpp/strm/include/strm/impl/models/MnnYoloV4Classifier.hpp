#ifndef IMPL_MNN_YOLO_V4_CLASSIFIER_HPP_
#define IMPL_MNN_YOLO_V4_CLASSIFIER_HPP_

#include "strm/impl/models/Classifier.hpp"

#include "MNN/Interpreter.hpp"

namespace rm {

class MnnYoloV4Classifier : public Classifier {
 public:
  MnnYoloV4Classifier(int inputSize, float confidenceThreshold, float iouThreshold, bool isTiny);

  ~MnnYoloV4Classifier();

 private:
  void inference(const cv::Mat& mat) override;

  const float* getBoxes(const int i) const override;

  const float* getConfidences(const int i) const override;

  std::pair<float, float> getReconstructRatios(const int originalWidth,
                                               const int originalHeight) override;

  MNN::Interpreter* interpreter;
  MNN::Session* session;

  float* input;
  float* boxes; // 1 x outputSize x 4
  float* confidences; // 1 x outputSize x numLabels

  const std::string INPUT_TENSOR_NAME = "x";
  const std::string OUTPUT_TENSOR_NAME_BOXES = "model/tf.concat_22/concat";
  const std::string OUTPUT_TENSOR_NAME_CONFS = "model/tf.concat_23/concat";
};

} // namespace rm

#endif // IMPL_MNN_YOLO_V4_CLASSIFIER_HPP_
