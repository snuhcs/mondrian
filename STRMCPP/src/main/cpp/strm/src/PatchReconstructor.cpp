#include "strm/PatchReconstructor.hpp"

#include <set>

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config, ResizeProfile* resizeProfile): mConfig(config), mResizeProfile(resizeProfile) {}

void PatchReconstructor::reconstructResults(MixedFrame& mixedFrame,
                                            const std::vector<BoundingBox>& results) const {
  bool runOriginalCode = false;

  time_us reconstructStartTime = NowMicros();
  for (const BoundingBox& box : results) {
    float maxOverlap = -1;
    Rect maxBoxPos;
    RoI* maxRoI = nullptr;
    Frame* maxFrame = nullptr;
    for (RoI* roi : mixedFrame.packedRoIs) {
      if (roi->isPacked()) {
        Rect movedAndResizedBoxPos(
            std::max(0,
                     (box.location.left - roi->packedLocation.first)
                     * roi->maxEdgeLength / roi->targetSize + roi->location.left),
            std::max(0,
                     (box.location.top - roi->packedLocation.second)
                     * roi->maxEdgeLength / roi->targetSize + roi->location.top),
            std::min(roi->frame->mat.cols,
                     (box.location.right - roi->packedLocation.first)
                     * roi->maxEdgeLength / roi->targetSize + roi->location.left),
            std::min(roi->frame->mat.rows,
                     (box.location.bottom - roi->packedLocation.second)
                     * roi->maxEdgeLength / roi->targetSize + roi->location.top));
        int intersection = roi->location.intersection(movedAndResizedBoxPos);
        float overlapRatio = (float) intersection / (float) movedAndResizedBoxPos.area();
        if (maxOverlap < overlapRatio) {
          maxOverlap = overlapRatio;
          maxBoxPos = movedAndResizedBoxPos;
          maxRoI = roi;
        }
      }
    }
    if (runOriginalCode) {
      if (maxFrame != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
        maxFrame->boxes.emplace_back(UNASSIGNED_ID, maxBoxPos, box.confidence, box.labelName,
                                     maxRoI->targetSize);
      }
    } else {
      if (maxRoI != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
        maxRoI->boxes.emplace_back(UNASSIGNED_ID, maxBoxPos, box.confidence, box.labelName,
                                   maxRoI->targetSize);
      }
    }
  }

  if (!runOriginalCode) {
    std::vector<std::pair<BoundingBox, Frame*>> unassignedBoxes;
    for (RoI* roi : mixedFrame.packedRoIs) {
      if (roi->isPacked()) {
        int maxIntersection = -1;
        int maxIndex = -1;
        for (int i = 0; i < roi->boxes.size(); ++i) {
          BoundingBox& box = roi->boxes[i];
          int intersection = roi->location.intersection(box.location);
          if (maxIntersection < intersection) {
            maxIntersection = intersection;
            maxIndex = i;
          }
        }
        if (maxIndex != -1) {
          BoundingBox& box = roi->boxes[maxIndex];
          roi->frame->boxes.emplace_back(roi->id, box.location, box.confidence, box.labelName,
                                         box.targetSize);
          for (int i = 0; i < roi->boxes.size(); ++i) {
            if (i == maxIndex) continue;
            unassignedBoxes.emplace_back(
                BoundingBox{UNASSIGNED_ID, box.location, box.confidence, box.labelName, box.targetSize},
                roi->frame);
          }
        }
      }
    }
    for (RoI* roi : mixedFrame.packedRoIs) {
      roi->isBoxReady = true;
    }

    // If new Boxes exist (those who lost competition between other Boxes in single RoI),
    // classify them as newly appeared objects and assign new Id
    if (!unassignedBoxes.empty()) {
      std::pair<idType, idType> idRange = RoI::getNewIds(unassignedBoxes.size());
      idType id = idRange.first;
      for (auto& unassignedBox : unassignedBoxes) {
        assert(id < idRange.second);
        BoundingBox& box = unassignedBox.first;
        Frame* frame = unassignedBox.second;
        frame->boxes.emplace_back(id++, box.location, box.confidence, box.labelName,
                                  box.targetSize);
      }
    }
  }

  std::set<Frame*> packedFrames = mixedFrame.getPackedFrames();
  for (Frame* frame : packedFrames) {
    frame->boxes = nms(frame->boxes, NUM_LABELS, mConfig.FRAME_BOXES_IOU_THRESHOLD);
  }

  std::set<Frame*> lastFrames = filterLastFrames(packedFrames);
  for (auto lastFrame : lastFrames) {
    for (RoI& roi : lastFrame->origRoIs) {
      if (roi.isProbingReady()) {
        mResizeProfile->updateTable(roi);
      }
    }
  }

  time_us reconstructEndTime = NowMicros();
  for (Frame* frame : packedFrames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
  LOGD("PatchReconstructor::reconstructResults(%lu, %lu) took %lu us", mixedFrame.packedRoIs.size(),
       results.size(), reconstructEndTime - reconstructStartTime);
}

} // namespace rm
