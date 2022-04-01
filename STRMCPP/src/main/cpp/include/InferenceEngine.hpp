#ifndef INFERENCE_ENGINE_HPP_
#define INFERENCE_ENGINE_HPP_

#include <vector>

#include "opencv2/core/mat.hpp"

namespace rm {

class InferenceEngine {
public:
    virtual int enqueue(const cv::Mat& mat, bool isFull) = 0;
    virtual std::vector<BoundingBox> getResults(int handle) = 0;
};

}

#endif // INFERENCE_ENGINE_HPP_
