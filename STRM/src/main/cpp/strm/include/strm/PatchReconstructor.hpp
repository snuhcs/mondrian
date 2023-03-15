#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "strm/Config.hpp"

namespace rm {

class Frame;
class MixedFrame;
class RoIResizer;

class PatchReconstructor {
 public:
  PatchReconstructor(const PatchReconstructorConfig& config, RoIResizer* roiResizer, int border);

  void assignBoxesToFrame(MixedFrame& mixedFrame, const std::vector<BoundingBox>& results) const;

  void matchBoxesWithChildRoIs(Frame* frame, bool isFullFrame) const;

  float getIoUThreshold() const;

 private:
  PatchReconstructorConfig mConfig;
  RoIResizer* mRoIResizer;
  const int mBorder;
};

} // namespace rm

#endif // PATCH_RECONSTRUCTOR_HPP_
