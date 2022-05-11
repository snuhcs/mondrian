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
  std::pair<float*, float*> inference(const cv::Mat& mat) override;

  MNN::Interpreter* interpreter;
  MNN::Session* session;

  const std::string INPUT_TENSOR_NAME = "x";
  const std::string OUTPUT_TENSOR_NAME_BOXES = "model/tf.concat_22/concat";
  const std::string OUTPUT_TENSOR_NAME_CONFS = "model/tf.concat_23/concat";
};

} // namespace rm

#endif // IMPL_MNN_YOLO_V4_CLASSIFIER_HPP_
