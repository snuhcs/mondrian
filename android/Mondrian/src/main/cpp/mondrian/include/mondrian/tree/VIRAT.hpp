#ifndef TREE_VIRAT_HPP_
#define TREE_VIRAT_HPP_

namespace md {

int VIRAT_FP16(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
          float shiftNcc, float avgErr);
int VIRAT_INT8(float maxEdgeLength, float area, float xyRatio, float shiftAvg, float shiftStd,
               float shiftNcc, float avgErr);

} // namespace md

#endif // TREE_VIRAT_HPP_
