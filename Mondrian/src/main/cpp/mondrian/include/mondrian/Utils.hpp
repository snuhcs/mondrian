#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <memory>

namespace md {

class BoundingBox;

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold);

void nms(std::vector<std::unique_ptr<BoundingBox>>& boxes,
         const int numLabels, const float iouThreshold);

extern const char* COCO_LABELS[];

} // namespace md

#endif // UTILS_HPP_
