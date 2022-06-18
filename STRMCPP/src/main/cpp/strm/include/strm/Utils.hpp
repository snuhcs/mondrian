#ifndef UTILS_HPP_
#define UTILS_HPP_

#include "strm/DataType.hpp"

namespace rm {

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold);

extern const char* COCO_LABELS[];

} // namespace rm

#endif // UTILS_HPP_
