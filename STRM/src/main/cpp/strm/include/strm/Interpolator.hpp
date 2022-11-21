#ifndef INTERPOLATOR_HPP_
#define INTERPOLATOR_HPP_

#include "strm/Frame.hpp"

namespace rm {

class Interpolator {
 public:
  static std::set<idType> interpolate(MultiStream& frames, float threshold);

 private:
  static std::set<idType> getRoIIds(const Stream& frames);

  static std::vector<RoI*> getRoIStream(const Stream& frames, idType roIId);

  static std::vector<int> findValidRoIs(std::vector<RoI*>& childRoIs);

  static std::pair<float, float> sumMotionVectors(std::vector<RoI*> childRoIs, int start, int end);

  static std::pair<float, float> getBbxShift(std::vector<RoI*> childRoIs, int start, int end);

  static void extrapolateLeft(std::vector<RoI*> childRoIs, int idx);

  static void extrapolateRight(std::vector<RoI*> childRoIs, int idx);

  static void interpolateBetween(std::vector<RoI*> childRoIs, int leftIdx, int rightIdx);

  static void addBoxWithPrevInfo(RoI* currRoI, const BoundingBox* prevBox,
                                 const std::pair<float, float>& newCenter);
};

} // namespace rm

#endif // INTERPOLATOR_HPP_
