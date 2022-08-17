#ifndef TREE_MTA_HPP_
#define TREE_MTA_HPP_

namespace rm {

float MTA(float width, float height, float maxEdgeLength, float type, float origin, float xyRatio,
          float avgShiftX, float avgShiftY, float avgShift, float stdShiftX, float stdShiftY,
          float stdShift, float avgErr, float ncc);

} // namespace rm

#endif // TREE_MTA_HPP_
