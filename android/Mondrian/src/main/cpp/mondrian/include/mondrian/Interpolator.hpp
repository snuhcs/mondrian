#ifndef INTERPOLATOR_HPP_
#define INTERPOLATOR_HPP_

#include "mondrian/Frame.hpp"

namespace md {

class Interpolator {
 public:
  static void interpolate(MultiStream& frames, float thres);

 private:
  static std::set<OID> getObjectIDs(const Stream& frames);

  static std::vector<ROI*> getROIStream(const Stream& frames, OID oid);

  static std::vector<int> findValidROIIndices(std::vector<ROI*>& childROIs);

  static std::pair<float, float> sumMotionVectors(std::vector<ROI*> childROIs, int start, int end);

  static std::pair<float, float> getBoxShift(std::vector<ROI*> childROIs, int start, int end);

  static void extrapolateLeft(std::vector<ROI*> childROIs, int idx);

  static void extrapolateRight(std::vector<ROI*> childROIs, int idx);

  static void interpolateBetween(std::vector<ROI*> childROIs, int leftIdx, int rightIdx);

  static void addBoxWithPrevInfo(ROI* currROI, const BoundingBox* prevBox,
                                 const std::pair<float, float>& newCenter);
};

} // namespace md

#endif // INTERPOLATOR_HPP_
