#ifndef INTERPOLATOR_HPP_
#define INTERPOLATOR_HPP_

#include <map>

#include "strm/DataType.hpp"

namespace rm {

class Interpolator {
 public:
  static std::set<idType> interpolate(std::map<std::string, SortedFrames>& frames);

 private:
  static std::set<idType> getRoIIds(const SortedFrames& frames);

  static std::vector<RoI*> getRoIStream(const SortedFrames& frames, idType roIId);

  static std::vector<int> findValidRoIs(std::vector<RoI*>& childRoIs);

  static std::pair<int, int> sumMotionVectors(std::vector<RoI*> childRoIs, int start, int end);

  static std::pair<int, int> getBbxShift(std::vector<RoI*> childRoIs, int start, int end);

  static void extrapolateLeft(std::vector<RoI*> childRoIs, int idx);

  static void extrapolateRight(std::vector<RoI*> childRoIs, int idx);

  static void interpolateBetween(std::vector<RoI*> childRoIs, int leftIdx, int rightIdx);

  static void addBoxWithPrevInfo(RoI* currRoI, const BoundingBox* prevBox,
                                 const std::pair<int, int>& newCenter);
};

} // namespace rm

#endif // INTERPOLATOR_HPP_
