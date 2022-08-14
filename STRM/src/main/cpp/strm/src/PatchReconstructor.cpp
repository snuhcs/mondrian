#include "strm/PatchReconstructor.hpp"

#include <set>

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config,
                                       RoIResizer* roiResizer)
    : mConfig(config), mRoIResizer(roiResizer) {}

static Rect moveResizeRoIPos(const RoI* roi) {
  std::pair<float, float> wh = roi->getResizedWidthHeight();
  return Rect(roi->packedLocation.first,
              roi->packedLocation.second,
              roi->packedLocation.first + wh.first,
              roi->packedLocation.second + wh.second);
}

static Rect reconstructBoxPos(const BoundingBox& packedBox, const RoI* pRoI) {
  float newLeft = (packedBox.location.left - pRoI->packedLocation.first)
                * pRoI->maxEdgeLength / pRoI->targetSize + pRoI->paddedLoc.left;
  float newTop = (packedBox.location.top - pRoI->packedLocation.second)
               * pRoI->maxEdgeLength / pRoI->targetSize + pRoI->paddedLoc.top;
  float newRight = (packedBox.location.right - pRoI->packedLocation.first)
                 * pRoI->maxEdgeLength / pRoI->targetSize + pRoI->paddedLoc.left;
  float newBottom = (packedBox.location.bottom - pRoI->packedLocation.second)
                  * pRoI->maxEdgeLength / pRoI->targetSize + pRoI->paddedLoc.top;
  return Rect(std::max(0.0f, newLeft),
              std::max(0.0f, newTop),
              std::min(float(pRoI->frame->mat.cols), newRight),
              std::min(float(pRoI->frame->mat.rows), newBottom));
}

void PatchReconstructor::assignBoxesToFrame(MixedFrame& mixedFrame,
                                            const std::vector<BoundingBox>& results) const {
  time_us reconstructStartTime = NowMicros();
  SortedFrames packedFrames = mixedFrame.getPackedFrames();
  std::set<RoI*>& packedRoIs = mixedFrame.packedRoIs;

  for (RoI* pRoI : packedRoIs) {
    if (!pRoI->isProbingRoI) {
      assert(std::any_of(pRoI->frame->parentRoIs.begin(), pRoI->frame->parentRoIs.end(),
                         [&pRoI](auto& pRoICandidate) { return pRoICandidate.get() == pRoI; }));
    }
  }
  for (const BoundingBox& box : results) {
    assert(box.srcRoI == nullptr);
    assert(box.id == UNASSIGNED_ID);
  }

  // Insert boxes to appropriate frame.boxes
  for (const BoundingBox& box : results) {
    float maxOverlap = 0;
    RoI* maxRoI = nullptr;
    for (RoI* pRoI : packedRoIs) {
      assert(pRoI->isPacked());
      float intersection = box.location.intersection(moveResizeRoIPos(pRoI));
      float overlapRatio = intersection / box.location.area();
      if (maxOverlap < overlapRatio) {
        maxOverlap = overlapRatio;
        maxRoI = pRoI;
      }
    }
    if (maxRoI != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
      if (maxRoI->isProbingRoI) {
        maxRoI->frame->probingBoxes.emplace_back(new BoundingBox(
            UNASSIGNED_ID, reconstructBoxPos(box, maxRoI),
            box.confidence, box.label, maxRoI->origin));
        maxRoI->probingBox = maxRoI->frame->probingBoxes.rbegin()->get();
      } else {
        maxRoI->frame->boxes.emplace_back(new BoundingBox(
            UNASSIGNED_ID, reconstructBoxPos(box, maxRoI),
            box.confidence, box.label, maxRoI->origin));
      }
    }
  }

  time_us reconstructEndTime = NowMicros();
  for (Frame* frame : packedFrames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
  LOGD("%-25s took %-7lld us for             mixed %-4d // %4lu packedRoIs %4lu boxes",
       "PR::assignBoxesToFrame", reconstructEndTime - reconstructStartTime,
       mixedFrame.mixedFrameIndex, packedFrames.size(), results.size());
}

void PatchReconstructor::matchBoxesWithRoIs(std::vector<std::unique_ptr<RoI>>& childRoIs,
                                            std::vector<std::unique_ptr<BoundingBox>>& boxes,
                                            bool isFullFrame) const {
  std::vector<BoundingBox*> unassignedBoxes;

  assert(std::all_of(childRoIs.begin(), childRoIs.end(),
                     [](auto& cRoI) { return cRoI->id != UNASSIGNED_ID && cRoI->box == nullptr; }));
  assert(std::all_of(boxes.begin(), boxes.end(),
                     [](auto& box) { return box->id == UNASSIGNED_ID && box->srcRoI == nullptr; }));

  // 1. Let Boxes to select their favorite RoI.
  // - Boxes can be unmatched, if overlap ratio is lower than threshold
  // - Selection result is saved in roi.boxes
  std::map<RoI*, std::vector<BoundingBox*>> roiToBoxesMap;
  for (std::unique_ptr<BoundingBox>& box : boxes) {
    // find RoI with largest overlap
    float maxOverlap = 0;
    RoI* maxRoI = nullptr;
    for (auto& cRoI : childRoIs) {
      if (isFullFrame || cRoI->parentRoI->isPacked()) {
        float intersection = cRoI->origLoc.intersection(box->location);
        assert(box->location.area() != 0);
        float overlapRatio = intersection / box->location.area();
        if (maxOverlap < overlapRatio) {
          maxOverlap = overlapRatio;
          maxRoI = cRoI.get();
        }
      }
    }
    // if overlap is large enough, assign box to roi.boxes
    // else that bounding box is considered as newly appeared object
    if (maxRoI != nullptr && maxOverlap >= mConfig.OVERLAP_THRESHOLD) {
      roiToBoxesMap[maxRoI].push_back(box.get());
    } else {
      unassignedBoxes.push_back(box.get());
    }
  }
  // End of 1

  // 2. Let RoIs select their favorite Box
  // - Selection result is saved as RoI with same id
  for (auto& cRoI : childRoIs) {
    if (isFullFrame || cRoI->parentRoI->isPacked()) {
      RoI* cRoIPtr = cRoI.get();
      float maxIntersection = -1.0f;
      int maxIndex = -1;
      for (int i = 0; i < roiToBoxesMap[cRoIPtr].size(); ++i) {
        BoundingBox* box = roiToBoxesMap[cRoIPtr][i];
        float intersection = cRoI->paddedLoc.intersection(box->location);
        if (maxIntersection < intersection) {
          maxIntersection = intersection;
          maxIndex = i;
        }
      }
      if (!roiToBoxesMap[cRoIPtr].empty()) {
        // 1-1 matching with RoI & box
        assert(maxIndex >= 0 && maxIndex < roiToBoxesMap[cRoIPtr].size());
        BoundingBox* maxBox = roiToBoxesMap[cRoIPtr][maxIndex];
        maxBox->id = cRoI->id;
        maxBox->srcRoI = cRoIPtr;
        cRoI->box = maxBox;
        cRoI->label = maxBox->label;

        // Collect all unselected boxes
        for (int i = 0; i < roiToBoxesMap[cRoIPtr].size(); ++i) {
          if (i == maxIndex) continue;
          unassignedBoxes.push_back(roiToBoxesMap[cRoIPtr][i]);
        }
      }
      cRoI->isMatchTried = true;
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
      assert(box->srcRoI == nullptr);
    }
  }
  // End of 3

  // 2. Update resize profile
  for (auto& cRoI : childRoIs) {
    if (cRoI->isProbingReady()) {
      mRoIResizer->updateTable(cRoI.get());
    }
  }

  for (auto& cRoI : childRoIs) {
    assert(cRoI->id != UNASSIGNED_ID);
    if (cRoI->box != nullptr) {
      assert(cRoI->box->id == cRoI->id);
    }
  }

  for (const std::unique_ptr<BoundingBox>& box : boxes) {
    assert(box->id != UNASSIGNED_ID);
    if (box->srcRoI != nullptr) {
      assert(box->srcRoI->box == box.get());
      assert(box->srcRoI->label == box->label);
      assert(box->srcRoI->id == box->id);
    }
  }
}

float PatchReconstructor::getIoUThreshold() const {
  return mConfig.FRAME_BOXES_IOU_THRESHOLD;
}

} // namespace rm
