#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "strm/Config.hpp"

namespace rm {

class MixedFrame;
class RoIResizer;

class PatchReconstructor {
 public:
  PatchReconstructor(const PatchReconstructorConfig& config, RoIResizer* roiResizer);

  void assignBoxesToFrame(MixedFrame& mixedFrame, const std::vector<BoundingBox>& results) const;

  void matchBoxesWithRoIs(std::vector<std::unique_ptr<RoI>>& childRoIs,
                          std::vector<std::unique_ptr<BoundingBox>>& boxes,
                          bool isFullFrame) const;

  float getIoUThreshold() const;

 private:
  PatchReconstructorConfig mConfig;
  RoIResizer* mRoIResizer;
};

} // namespace rm

#endif // PATCH_RECONSTRUCTOR_HPP_
