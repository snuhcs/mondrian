#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <map>
#include <memory>
#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"

namespace md {

std::vector<Rect> extractPD(const cv::Mat& prevGrayMat, const cv::Mat& nextGrayMat);

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold);

void nms(std::vector<std::unique_ptr<BoundingBox>>& boxes,
         const int numLabels, const float iouThreshold);

} // namespace md

#endif // UTILS_HPP_
