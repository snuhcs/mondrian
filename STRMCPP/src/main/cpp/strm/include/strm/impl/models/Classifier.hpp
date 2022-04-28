#ifndef IMPL_MODELS_CLASSIFIER_HPP_
#define IMPL_MODELS_CLASSIFIER_HPP_

#include "strm/DataType.hpp"

namespace rm {

class Classifier {
 public:
  virtual std::vector<BoundingBox> recognizeImage(const cv::Mat& mat, int originalWidth, int originalHeight) = 0;

  virtual int getInputSize() const = 0;

  virtual long long getInferenceTimeMs() const = 0;
};

} // namespace rm

#endif // IMPL_MODELS_CLASSIFIER_HPP_
