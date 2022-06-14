#include "strm/PatchReconstructor.hpp"

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config) : mConfig(config) {}

void PatchReconstructor::reconstructResults(
    MixedFrame& mixedFrame, const std::vector<BoundingBox>& results) const {
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
  std::map<std::string, Frame*> lastFrames;
  for (Frame* frame : mixedFrame.frames) {
    frame->boxes = nms(frame->boxes, NUM_LABELS, mConfig.FRAME_BOXES_IOU_THRESHOLD);
    if (lastFrames.find(frame->key) == lastFrames.end() ||
        lastFrames.at(frame->key)->frameIndex < frame->frameIndex) {
      lastFrames[frame->key] = frame;
    }
  }
  for (auto it = lastFrames.begin(); it != lastFrames.end(); it++) {
    Frame* frame = it->second;
    std::transform(frame->boxes.begin(), frame->boxes.end(),
                   std::back_inserter(frame->boxesToTrack),
                   [this, &frame](const BoundingBox& box) {
                     return BoundingBox{Rect(
                         std::max(0, box.location.left - mConfig.ROI_PADDING),
                         std::max(0, box.location.top - mConfig.ROI_PADDING),
                         std::min(frame->width, box.location.right + mConfig.ROI_PADDING),
                         std::min(frame->height, box.location.bottom + mConfig.ROI_PADDING)),
                                        box.confidence, box.labelName};
                   });
    frame->isResultReady = true;
  }
  time_us reconstructEndTime = NowMicros();
  for (Frame* frame : mixedFrame.frames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
  for (Frame* frame : mixedFrame.frames) {
    frame->isResultReady = true;
  }
}

} // namespace rm
