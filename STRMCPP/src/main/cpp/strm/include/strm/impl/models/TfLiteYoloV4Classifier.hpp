#ifndef IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
#define IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_

#include "MNN/Interpreter.hpp"
#include "tensorflow/lite/interpreter.h"

#include "strm/impl/models/Classifier.hpp"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/interpreter.h"

namespace rm {

class TfLiteYoloV4Classifier : public Classifier {
 public:
  TfLiteYoloV4Classifier(int size, float confThreshold, float iouThreshold, bool isTiny);
  ~TfLiteYoloV4Classifier();
  std::vector<BoundingBox> recognizeImage(const cv::Mat& mat, int originalWidth, int originalHeight) override;
  int getInputSize() const override;
  long long getInferenceTimeMs() const override;

 private:
  std::vector<BoundingBox> getDetectionsForFull(
      const cv::Mat& mat, int originalWidth, int originalHeight);
  std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes) const;

  const int NUM_LABELS = 80;
  const int INPUT_SIZE;
  const int OUTPUT_WIDTH;
  const float CONF_THRESHOLD;
  const float IOU_THRESHOLD;

  long long inferenceTimeMs;

  TfLiteDelegate* delegate;
  std::unique_ptr<tflite::Interpreter> interpreter;
};

} // namespace rm

#endif // IMPL_TFLITE_YOLO_V4_CLASSIFIER_HPP_
