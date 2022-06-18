#ifndef INTERPOLATOR_HPP_
#define INTERPOLATOR_HPP_

#include <set>

#include "strm/DataType.hpp"

namespace rm {

class Interpolator {
 public:
  static void interpolate(FrameSet& frames);

 private:
  static std::set<idType> getRoIIds(FrameSet& frames);

  static std::vector<RoI*> getRoIStream(FrameSet& frames, idType roIId);

  static std::vector<int> findValidRoIs(std::vector<RoI*>& rois);

  static std::pair<int, int> sumMotionVectors(std::vector<RoI*> rois, int start, int end);

  static std::pair<int, int> getBbxShift(std::vector<RoI*> rois, int start, int end);

  static void extrapolateLeft(std::vector<RoI*> rois, int idx);

  static void extrapolateRight(std::vector<RoI*> rois, int idx);

  static void interpolateBetween(std::vector<RoI*> rois, int leftIdx, int rightIdx);
};

} // namespace rm

#endif // INTERPOLATOR_HPP_
