#include "strm/PatchReconstructor.hpp"

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config) : mConfig(config) {}

void PatchReconstructor::reconstructResults(
    MixedFrame& mixedFrame, const std::vector<BoundingBox>& results) const {
  LOGD("PatchReconstructor::reconstructResults() start %lu %lu", mixedFrame.frames.size(), results.size());
  time_us reconstructStartTime = NowMicros();
  for (const BoundingBox& box : results) {
    float maxOverlap = -1;
    Rect maxBoxPos;
    Frame* maxFrame = nullptr;
    for (Frame* frame : mixedFrame.frames) {
      for (RoI& roi : frame->rois) {
        if (roi.isPacked()) {
          Rect movedAndResizedBoxPos(
              std::max(0,
                       (box.location.left - roi.packedLocation.first)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.left),
              std::max(0,
                       (box.location.top - roi.packedLocation.second)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.top),
              std::min(roi.frame->mat.cols,
                       (box.location.right - roi.packedLocation.first)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.left),
              std::min(roi.frame->mat.rows,
                       (box.location.bottom - roi.packedLocation.second)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.top));
          int intersection = roi.location.intersection(movedAndResizedBoxPos);
          float overlapRatio = (float) intersection / (float) movedAndResizedBoxPos.area();
          if (maxOverlap < overlapRatio) {
            maxOverlap = overlapRatio;
            maxBoxPos = movedAndResizedBoxPos;
            maxFrame = frame;
          }
        }
      }
    }
    if (maxFrame != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
      maxFrame->boxes.emplace_back(maxBoxPos, box.confidence, box.labelName);
    }
  }
  for (Frame* frame : mixedFrame.frames) {
    frame->boxes = nms(frame->boxes, NUM_LABELS, mConfig.FRAME_BOXES_IOU_THRESHOLD);
  }
  time_us reconstructEndTime = NowMicros();
  for (Frame* frame : mixedFrame.frames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
  LOGD("PatchReconstructor::reconstructResults() end");
}

} // namespace rm
