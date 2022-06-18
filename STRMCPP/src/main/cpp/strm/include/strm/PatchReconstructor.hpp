#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/ResizeProfile.hpp"

namespace rm {

class PatchReconstructor {
 public:
  PatchReconstructor(const PatchReconstructorConfig& config, ResizeProfile* resizeProfile);

  void reconstructResults(MixedFrame& mixedFrame, const std::vector<BoundingBox>& results) const ;
  void matchBoxesWithRoIs(bool isFullFrame, std::vector<RoI>& childrenRoIs, std::vector<BoundingBox>& boxes) const;

 private:
  PatchReconstructorConfig mConfig;
  ResizeProfile* mResizeProfile;
};

} // namespace rm

#endif // PATCH_RECONSTRUCTOR_HPP_
