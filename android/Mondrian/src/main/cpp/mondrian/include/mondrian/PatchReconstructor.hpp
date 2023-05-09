#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "mondrian/Config.hpp"

namespace md {

class Frame;
class PackedCanvas;
class ROIResizer;

class PatchReconstructor {
 public:
  PatchReconstructor(const PatchReconstructorConfig& config, ROIResizer* roiResizer);

  void assignBoxesToFrame(PackedCanvas& packedCanvas, const std::vector<BoundingBox>& results) const;

  void matchBoxesROIs(Frame* frame, bool isFullFrame) const;

  float getIoUThreshold() const;

 private:
  PatchReconstructorConfig mConfig;
  ROIResizer* mROIResizer;
};

} // namespace md

#endif // PATCH_RECONSTRUCTOR_HPP_
