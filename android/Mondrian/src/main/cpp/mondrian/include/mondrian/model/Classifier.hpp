#ifndef MODEL_CLASSIFIER_HPP_
#define MODEL_CLASSIFIER_HPP_

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/Time.hpp"

namespace md {

class Classifier {
 public:
  Classifier(const int numLabels, const int inputSize, const int outputSize,
             const float confThres, const float iouThres);

  virtual ~Classifier() {};

  std::vector<BoundingBox> recognizeImage(const cv::Mat& rgbMat);

 protected:
  virtual cv::Mat preprocess(const cv::Mat& rgbMat) const = 0;

  virtual void inference(const cv::Mat& inputTensor) const = 0;

  virtual std::vector<BoundingBox> postprocess(int width, int height) const = 0;

  virtual Device device() const = 0;

  const int numLabels;
  const cv::Size inputSize; // width, height
  const int outputSize;
  const float confThres;
  const float iouThres;
};

} // namespace md

#endif // MODEL_CLASSIFIER_HPP_
