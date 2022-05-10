#ifndef UTILS_HPP
#define UTILS_HPP

#include "strm/DataType.hpp"

namespace rm {

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold);

} // namespace rm

#endif // UTILS_HPP
