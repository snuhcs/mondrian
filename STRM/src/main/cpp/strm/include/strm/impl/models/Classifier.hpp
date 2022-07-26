#ifndef IMPL_MODELS_CLASSIFIER_HPP_
#define IMPL_MODELS_CLASSIFIER_HPP_

#include "strm/DataType.hpp"

namespace rm {

class Classifier {
 public:
  Classifier(const int numLabels, const int inputSize, const int outputSize,
             const float confidenceThreshold, const float iouThreshold);

  virtual ~Classifier() {};

  std::vector<BoundingBox> recognizeImage(const cv::Mat& mat);

  const cv::Size& getInputSize() const;

  long long getInferenceTimeMs() const;

  virtual long long int profileInferenceTime() = 0;

  void setInferenceTimeMs(long long inferenceTime);

 protected:
  virtual cv::Mat preprocess(const cv::Mat& mat) = 0;

  virtual void inference(const cv::Mat& mat) = 0;

  virtual const float* getBox(const int i) const = 0;

  virtual const float getObjectConfidence(const int i) const = 0;

  virtual const float* getClassConfidences(const int i) const = 0;

  virtual Rect reconstructBox(float x, float y, float w, float h,
                              int imageWidth, int imageHeight) = 0;

  const int numLabels;
  const cv::Size inputSize; // width, height
  const int outputSize;
  const float confidenceThreshold;
  const float iouThreshold;
  long long inferenceTimeMs = 0;
};

} // namespace rm

#endif // IMPL_MODELS_CLASSIFIER_HPP_
