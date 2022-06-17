#include "strm/PatchReconstructor.hpp"

#include <set>

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config, ResizeProfile* resizeProfile): mConfig(config), mResizeProfile(resizeProfile) {}

void PatchReconstructor::reconstructResults(MixedFrame& mixedFrame,
                                            const std::vector<BoundingBox>& results) const {
  time_us reconstructStartTime = NowMicros();
  for (const BoundingBox& box : results) {
    float maxOverlap = -1;
    Rect maxBoxPos;
    RoI* maxRoI = nullptr;
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
    if (maxRoI != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
      maxRoI->boxes.emplace_back(UNASSIGNED_ID, maxBoxPos, box.confidence, box.labelName, maxRoI->targetSize);
      float maxOrigOverlap = -1;
      RoI* maxOrigRoI = nullptr;
      for (RoI* origRoI : maxRoI->childrenRoIs) {
        int intersection = origRoI->location.intersection(maxBoxPos);
        float overlapRatio = (float) intersection / (float) maxBoxPos.area();
        if (maxOrigOverlap < overlapRatio) {
          maxOrigOverlap = overlapRatio;
          maxOrigRoI = origRoI;
        }
      }
      if (maxOrigRoI != nullptr && maxOrigOverlap >= mConfig.OVERLAP_THRESHOLD) {
        maxOrigRoI->boxes.emplace_back(maxOrigRoI->id, maxBoxPos, box.confidence, box.labelName, maxOrigRoI->targetSize);
      }
    }
  }

  std::vector<std::pair<BoundingBox, Frame*>> unassignedBoxes;
  for (RoI* roi : mixedFrame.packedRoIs) {
    if (roi->isPacked()) {
      for (RoI* origRoI : roi->childrenRoIs) {
        int maxIntersection = -1;
        int maxIndex = -1;
        for (int i = 0; i < origRoI->boxes.size(); ++i) {
          BoundingBox& box = origRoI->boxes[i];
          int intersection = origRoI->location.intersection(box.location);
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
            unassignedBoxes.emplace_back(BoundingBox{
              UNASSIGNED_ID, box.location, box.confidence, box.labelName, box.targetSize},
                roi->frame);
          }
        }
      }
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
  for (RoI* roi : mixedFrame.packedRoIs) {
    assert(std::any_of(roi->boxes.begin(), roi->boxes.end(),
                       [](const BoundingBox& box){ return box.id != UNASSIGNED_ID; }));
    roi->isBoxReady = true;
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
