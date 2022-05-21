#ifndef IMPL_MODELS_CLASSIFIER_HPP_
#define IMPL_MODELS_CLASSIFIER_HPP_

#include "strm/DataType.hpp"

namespace rm {

class Classifier {
 public:
  Classifier(const int numLabels, const int inputSize, const int outputSize,
             const float confidenceThreshold, const float iouThreshold);

  virtual ~Classifier() {};

  std::vector<BoundingBox> recognizeImage(const cv::Mat& mat, int originalWidth,
                                          int originalHeight);

  int getInputSize() const;

  long long getInferenceTimeMs() const;

 protected:
  virtual void inference(const cv::Mat& mat) = 0;
  virtual const float* getBoxes(const int i) const = 0;
  virtual const float* getConfidences(const int i) const = 0;

  const int numLabels;
  const int inputSize;
  const int outputSize;
  const float confidenceThreshold;
  const float iouThreshold;
  long long inferenceTimeMs = 0;
};

} // namespace rm

#endif // IMPL_MODELS_CLASSIFIER_HPP_
