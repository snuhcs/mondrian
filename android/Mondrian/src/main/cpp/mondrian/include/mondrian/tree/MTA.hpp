#ifndef TREE_MTA_HPP_
#define TREE_MTA_HPP_

namespace md {

int MTA_FP16(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
        float shiftNcc, float avgErr);
int MTA_INT8(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
             float shiftNcc, float avgErr);

} // namespace md

#endif // TREE_MTA_HPP_
