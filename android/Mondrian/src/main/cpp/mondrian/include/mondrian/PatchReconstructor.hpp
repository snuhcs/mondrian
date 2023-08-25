#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "mondrian/Config.hpp"

namespace md {

class BoundingBox;
class Frame;
class PackedCanvas;
class ROIResizer;

class PatchReconstructor {
 public:
  PatchReconstructor(const PatchReconstructorConfig& config, ROIResizer* roiResizer);

  void assignBoxesToFrame(PackedCanvas& packedCanvas,
                          const std::vector<BoundingBox>& results) const;

  void matchBoxesROIs(Frame* frame, bool isFullFrame) const;

  float iouThres() const;

 private:
  PatchReconstructorConfig config_;
  ROIResizer* ROIResizer_;
};

} // namespace md

#endif // PATCH_RECONSTRUCTOR_HPP_
