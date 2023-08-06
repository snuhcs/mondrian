#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <map>
#include <memory>
#include <set>

#include "mondrian/DataType.hpp"

namespace md {

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold);

void nms(std::vector<std::unique_ptr<BoundingBox>>& boxes,
         const int numLabels, const float iouThreshold);

} // namespace md

#endif // UTILS_HPP_
