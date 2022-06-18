#include "strm/PatchReconstructor.hpp"

#include <set>

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config, ResizeProfile* resizeProfile): mConfig(config), mResizeProfile(resizeProfile) {}

Rect getMovedAndResizedBoxPos(const BoundingBox& box, RoI& roi) {
  return Rect(
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
}

void PatchReconstructor::reconstructResults(MixedFrame& mixedFrame,
                                            const std::vector<BoundingBox>& results) const {
  time_us reconstructStartTime = NowMicros();
  SortedFrames packedFrames = mixedFrame.getPackedFrames();
  std::vector<RoI*>& packedRoIs = mixedFrame.packedRoIs;

  // 1. Insert boxes to appropriate frame.boxes
  for (const BoundingBox& box : results) {
    float maxOverlap = -1;
    Rect maxBoxPos;
    RoI* maxRoI = nullptr;
    for (RoI* roi : packedRoIs) {
      if (roi->isPacked()) {
        Rect movedAndResizedBoxPos = getMovedAndResizedBoxPos(box, *roi);
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
      maxRoI->frame->boxes.emplace_back(UNASSIGNED_ID, maxBoxPos, box.confidence, box.labelName, maxRoI->targetSize);
    }
  }

  // 2. Match boxes with RoIs (per frame)
  for (Frame* frame : packedFrames) {
    frame->boxes = nms(frame->boxes, NUM_LABELS, mConfig.FRAME_BOXES_IOU_THRESHOLD);
    matchBoxesWithRoIs(false, frame->childRoIs, frame->boxes);
  }
  // set roi->isBoxReady as true if it was packed
  for (RoI* roi : packedRoIs) {
    assert(roi->isPacked());
    roi->isBoxReady = true;
  }

  // 3. Update resize profile
  for (Frame* frame : packedFrames) {
    for (RoI& roi : frame->childRoIs) {
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
  LOGD("PatchReconstructor::reconstructResults(%lu, %lu) took %lu us", packedRoIs.size(),
       results.size(), reconstructEndTime - reconstructStartTime);
}

  void PatchReconstructor::matchBoxesWithRoIs(bool isFullFrame, std::vector<RoI>& childRoIs, std::vector<BoundingBox>& boxes) const {
    std::vector<BoundingBox*> unassignedBoxes;

    // 1. Let Boxes to select their favorite RoI.
    // - Boxes can be unmatched, if overlap ratio is lower than threshold
    // - Selection result is saved in roi.boxes
    for (BoundingBox& box : boxes) {
      // find RoI with largest overlap
      float maxOverlap = -1;
      RoI* maxRoI = nullptr;
      for (RoI& cRoI : childRoIs) {
        if (isFullFrame || cRoI.parentRoI->isPacked()) {
          int intersection = cRoI.location.intersection(box.location);
          assert(box.location.area() != 0);
          float overlapRatio = (float) intersection / (float) box.location.area();
          if (maxOverlap < overlapRatio) {
            maxOverlap = overlapRatio;
            maxRoI = &cRoI;
          }
        }
      }
      // if overlap is large enough, assign box to roi.boxes
      // else that bounding box is considered as newly appeared object
      if (maxRoI != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
        maxRoI->boxes.push_back(&box);
      } else {
        unassignedBoxes.push_back(&box);
      }
    }
    // End of 1

    int sumRoIBoxes = 0;
    for (RoI& cRoI : childRoIs) {
      sumRoIBoxes += (int) cRoI.boxes.size();
    }
    assert(sumRoIBoxes + unassignedBoxes.size() == boxes.size());

    // 2. Let RoIs select their favorite Box
    // - Selection result is saved as RoI with same id
    for (RoI& cRoI : childRoIs) {
      int maxIntersection = -1;
      int maxIndex = -1;
      for (int i = 0; i < cRoI.boxes.size(); ++i) {
        BoundingBox& box = *(cRoI.boxes[i]);
        int intersection = cRoI.location.intersection(box.location);
        if (maxIntersection < intersection) {
          maxIntersection = intersection;
          maxIndex = i;
        }
      }
      if (!cRoI.boxes.empty()) {
        // 1-1 matching with RoI & box
        assert(maxIndex >= 0 && maxIndex < cRoI.boxes.size());
        BoundingBox& box = *(cRoI.boxes[maxIndex]);
        box.id = cRoI.id;
        box.srcRoI = &cRoI;

        // Collect all unselected boxes
        for (int i = 0; i < cRoI.boxes.size(); ++i) {
          if (i == maxIndex) continue;
          unassignedBoxes.push_back(cRoI.boxes[i]);
        }

        // Only store one corresponding box in cRoI.boxes
        cRoI.boxes.clear();
        cRoI.boxes.push_back(&box);
      }
    }
    // End of 2

    // 3. Assign new IDs to newly appeared objects
    // - Those are
    //   a) who does not match with any RoI
    //   b) those who lost competition between other Boxes wrt RoI's selection
    if (!unassignedBoxes.empty()) {
      std::pair<idType, idType> idRange = RoI::getNewIds(unassignedBoxes.size());
      idType id = idRange.first;
      for (BoundingBox* box : unassignedBoxes) {
        assert(id < idRange.second);
        box->id = id++;
      }
    }
    // End of 3

    for (BoundingBox& box : boxes) {
      assert(box.id != UNASSIGNED_ID);
    }
}

} // namespace rm
