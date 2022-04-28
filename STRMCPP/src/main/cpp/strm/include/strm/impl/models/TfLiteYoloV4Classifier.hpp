#ifndef IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
#define IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_

#include "MNN/Interpreter.hpp"

#include "strm/impl/models/Classifier.hpp"

namespace rm {

class TfLiteYoloV4Classifier : public Classifier {
 public:
  TfLiteYoloV4Classifier(int size, float confThreshold, float iouThreshold, bool isTiny);
  std::vector<BoundingBox> recognizeImage(const cv::Mat& mat, int originalWidth, int originalHeight) override;
  int getInputSize() const override;
  long long getInferenceTimeMs() const override;

 private:
  std::vector<BoundingBox> getDetectionsForFull(
      const cv::Mat& mat, int originalWidth, int originalHeight);
  std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes) const;

  const std::string INPUT_TENSOR_NAME = "x";
  const std::string OUTPUT_TENSOR_NAME_BOXES = "model/tf.concat_22/concat";
  const std::string OUTPUT_TENSOR_NAME_CONFS = "model/tf.concat_23/concat";

  const int NUM_LABELS = 80;
  const int INPUT_SIZE;
  const int OUTPUT_WIDTH;
  const float CONF_THRESHOLD;
  const float IOU_THRESHOLD;

  long long inferenceTimeMs;

  MNN::Interpreter* interpreter;
  MNN::Session* session;
};

} // namespace rm

#endif // IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
