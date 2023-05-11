#ifndef MODEL_CLASSIFIER_HPP_
#define MODEL_CLASSIFIER_HPP_

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/Time.hpp"

namespace md {

class Classifier {
 public:
  Classifier(const int numLabels, const int inputSize, const int outputSize,
             const float confidenceThreshold, const float iouThreshold);

  virtual ~Classifier() {};

  virtual std::vector<BoundingBox> recognizeImage(const cv::Mat& rgbMat);

  const cv::Size& getInputSize() const;

 protected:
  std::vector<BoundingBox> postprocess(int width, int height) const;

  virtual cv::Mat preprocess(const cv::Mat& mat) = 0;

  virtual void inference(const cv::Mat& mat) = 0;

  virtual const float* getBox(const int i) const;

  virtual float getObjectConfidence(const int i) const;

  virtual const float* getClassConfidences(const int i) const;

  virtual Rect reconstructBox(float x, float y, float w, float h,
                              float imageWidth, float imageHeight) const = 0;

  const int numLabels;
  const cv::Size inputSize; // width, height
  const int outputSize;
  const float confidenceThreshold;
  const float iouThreshold;
};

} // namespace md

#endif // MODEL_CLASSIFIER_HPP_
