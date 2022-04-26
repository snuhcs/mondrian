#ifndef INFERENCE_ENGINE_HPP_
#define INFERENCE_ENGINE_HPP_

#include <vector>

#include "opencv2/core/mat.hpp"

#include "strm/DataType.hpp"

namespace rm {

class InferenceEngine {
 public:
  virtual int enqueue(const cv::Mat mat, const bool isFull) = 0;
  virtual std::vector<BoundingBox> getResults(const int handle) = 0;
  virtual long long getInferenceTimeMs() = 0;
};

}

#endif // INFERENCE_ENGINE_HPP_
