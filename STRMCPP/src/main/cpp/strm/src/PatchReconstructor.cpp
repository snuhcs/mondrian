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
  std::set<RoI*>& packedRoIs = mixedFrame.packedRoIs;

  for (const BoundingBox& box : results) {
    assert(box.srcRoI == nullptr);
  }

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
      maxRoI->frame->boxes.emplace_back(new BoundingBox(UNASSIGNED_ID, maxBoxPos, box.confidence, box.label, maxRoI->targetSize));
    }
  }

  // 2. Match boxes with RoIs (per frame)
  for (Frame* frame : packedFrames) {
    assert(std::all_of(frame->boxes.begin(), frame->boxes.end(),
                       [](const std::unique_ptr<BoundingBox>& box) {
                         return box->srcRoI == nullptr ||
                                (box->srcRoI->box == box.get() &&
                                 box->srcRoI->label == box->label &&
                                 box->srcRoI->id == box->id);
                       }));
    nms(frame->boxes, NUM_LABELS, mConfig.FRAME_BOXES_IOU_THRESHOLD);
    assert(std::all_of(frame->boxes.begin(), frame->boxes.end(),
                       [](const std::unique_ptr<BoundingBox>& box) {
                         return box->srcRoI == nullptr ||
                                (box->srcRoI->box == box.get() &&
                                 box->srcRoI->label == box->label &&
                                 box->srcRoI->id == box->id);
                       }));
    matchBoxesWithRoIs(false, frame->childRoIs, frame->boxes, &packedRoIs);
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

void PatchReconstructor::matchBoxesWithRoIs(bool isFullFrame, std::vector<RoI>& childRoIs,
                                            std::vector<std::unique_ptr<BoundingBox>>& boxes,
                                            const std::set<RoI*>* packedRoIs) const {
  assert(std::all_of(childRoIs.begin(), childRoIs.end(), [&packedRoIs](RoI& roi) {
    return packedRoIs->find(&roi) == packedRoIs->end() || roi.box == nullptr;
  }));

  std::vector<BoundingBox*> unassignedBoxes;

  // 1. Let Boxes to select their favorite RoI.
  // - Boxes can be unmatched, if overlap ratio is lower than threshold
  // - Selection result is saved in roi.boxes
  std::map<RoI*, std::vector<BoundingBox*>> roiToBoxesMap;
  for (auto it = boxes.begin(); it != boxes.end();) {
    BoundingBox* box = it->get();
    // find RoI with largest overlap
    bool kill = false;
    float maxOverlap = -1;
    RoI* maxRoI = nullptr;
    for (RoI& cRoI : childRoIs) {
      if (isFullFrame || cRoI.isPacked()) {
        int intersection = cRoI.location.intersection(box->location);
        assert(box->location.area() != 0);
        float overlapRatio = (float) intersection / (float) box->location.area();
        if (maxOverlap == 1 && overlapRatio == 1) {
          kill = true;
          break;
        }
        if (maxOverlap < overlapRatio) {
          maxOverlap = overlapRatio;
          maxRoI = &cRoI;
        }
      }
    }
    if (kill) {
      it = boxes.erase(it);
      continue;
    } else {
      // if overlap is large enough, assign box to roi.boxes
      // else that bounding box is considered as newly appeared object
      if (maxRoI != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
        roiToBoxesMap[maxRoI].push_back(box);
      } else {
        unassignedBoxes.push_back(box);
      }
      it++;
    }
  }
  // End of 1

  // 2. Let RoIs select their favorite Box
  // - Selection result is saved as RoI with same id
  for (RoI& cRoI : childRoIs) {
    int maxIntersection = -1;
    int maxIndex = -1;
    for (int i = 0; i < roiToBoxesMap[&cRoI].size(); ++i) {
      BoundingBox* box = roiToBoxesMap[&cRoI][i];
      int intersection = cRoI.location.intersection(box->location);
      if (maxIntersection < intersection) {
        maxIntersection = intersection;
        maxIndex = i;
      }
    }
    if (!roiToBoxesMap[&cRoI].empty()) {
      // 1-1 matching with RoI & box
      assert(maxIndex >= 0 && maxIndex < roiToBoxesMap[&cRoI].size());
      BoundingBox* maxBox = roiToBoxesMap[&cRoI][maxIndex];
      maxBox->id = cRoI.id;
      maxBox->srcRoI = &cRoI;
      cRoI.box = maxBox;
      cRoI.label = maxBox->label;

      // Collect all unselected boxes
      for (int i = 0; i < roiToBoxesMap[&cRoI].size(); ++i) {
        if (i == maxIndex) continue;
        unassignedBoxes.push_back(roiToBoxesMap[&cRoI][i]);
      }
    } else {
      cRoI.box = nullptr;
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
      box->srcRoI = nullptr;
    }
  }
  // End of 3

  for (const std::unique_ptr<BoundingBox>& box : boxes) {
    assert(box->id != UNASSIGNED_ID);
    if (box->srcRoI != nullptr) {
      assert(box->srcRoI->box == box.get());
      assert(box->srcRoI->label == box->label);
      assert(box->srcRoI->id == box->id);
    }
  }
}

} // namespace rm
