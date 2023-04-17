#ifndef MODEL_CLASSIFIER_HPP_
#define MODEL_CLASSIFIER_HPP_

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/Time.hpp"

namespace md {

using Result = std::tuple<std::vector<BoundingBox>, std::pair<time_us, time_us>, Device>;

class Classifier {
 public:
  Classifier(const int numLabels, const int inputSize, const int outputSize,
             const float confidenceThreshold, const float iouThreshold, Device device);

  virtual ~Classifier() {};

  virtual Result recognizeImage(const cv::Mat& rgbMat);

  const cv::Size& getInputSize() const;

  time_us getInferenceTime() const;

  void setInferenceTime(time_us currInferenceTime);

  time_us profileInferenceTime(int profileWarmups, int profileRuns) const;

 protected:
  virtual cv::Mat preprocess(const cv::Mat& mat) = 0;

  virtual void inference(const cv::Mat& mat) = 0;

  virtual const float* getBox(const int i) const;

  virtual float getObjectConfidence(const int i) const;

  virtual const float* getClassConfidences(const int i) const;

  virtual Rect reconstructBox(float x, float y, float w, float h,
                              float imageWidth, float imageHeight) = 0;

  virtual void singleInference() const = 0;

  const Device device;
  const int numLabels;
  const cv::Size inputSize; // width, height
  const int outputSize;
  const float confidenceThreshold;
  const float iouThreshold;
  time_us inferenceTime = 0;
};

} // namespace md

#endif // MODEL_CLASSIFIER_HPP_
