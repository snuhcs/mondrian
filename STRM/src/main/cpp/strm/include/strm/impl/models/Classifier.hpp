#ifndef IMPL_MODELS_CLASSIFIER_HPP_
#define IMPL_MODELS_CLASSIFIER_HPP_

#include "strm/DataType.hpp"

namespace rm {

class Classifier {
 public:
  Classifier(const int numLabels, const int inputSize, const int outputSize,
             const float confidenceThreshold, const float iouThreshold);

  virtual ~Classifier() {};

  virtual std::vector<BoundingBox> recognizeImage(const cv::Mat& mat);

  const cv::Size& getInputSize() const;

  time_us getInferenceTime() const;

  void setInferenceTime(time_us currInferenceTime);

  virtual time_us profileInferenceTime() = 0;

 protected:
  virtual cv::Mat preprocess(const cv::Mat& mat) = 0;

  virtual void inference(const cv::Mat& mat) = 0;

  virtual const float* getBox(const int i) const;

  virtual float getObjectConfidence(const int i) const;

  virtual const float* getClassConfidences(const int i) const;

  virtual Rect reconstructBox(float x, float y, float w, float h,
                              float imageWidth, float imageHeight) = 0;

  const int numLabels;
  const cv::Size inputSize; // width, height
  const int outputSize;
  const float confidenceThreshold;
  const float iouThreshold;
  time_us inferenceTime = 0;
};

} // namespace rm

#endif // IMPL_MODELS_CLASSIFIER_HPP_
