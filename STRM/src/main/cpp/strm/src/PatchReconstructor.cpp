#include "strm/PatchReconstructor.hpp"

#include <set>

#include "strm/Log.hpp"
#include "strm/MixedFrame.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/Utils.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config,
                                       RoIResizer* roiResizer)
    : mConfig(config), mRoIResizer(roiResizer) {}

static Rect moveResizeRoIPos(const RoI* roi) {
  IntPair wh = roi->getResizedMatWidthHeight();
  return {float(roi->getPackedXY().first),
          float(roi->getPackedXY().second),
          float(roi->getPackedXY().first + wh.first),
          float(roi->getPackedXY().second + wh.second)};
}

static Rect reconstructBoxPos(const BoundingBox& packedBox, const RoI* pRoI) {
  float scale = pRoI->getTargetScale();
  float newLeft = (packedBox.location.left - pRoI->getPackedXY().first)
                  / scale + pRoI->paddedLoc.left;
  float newTop = (packedBox.location.top - pRoI->getPackedXY().second)
                 / scale + pRoI->paddedLoc.top;
  float newRight = (packedBox.location.right - pRoI->getPackedXY().first)
                   / scale + pRoI->paddedLoc.left;
  float newBottom = (packedBox.location.bottom - pRoI->getPackedXY().second)
                    / scale + pRoI->paddedLoc.top;
  return {std::max(0.0f, newLeft),
          std::max(0.0f, newTop),
          std::min(float(pRoI->frame->mat.cols), newRight),
          std::min(float(pRoI->frame->mat.rows), newBottom)};
}

void PatchReconstructor::assignBoxesToFrame(MixedFrame& mixedFrame,
                                            const std::vector<BoundingBox>& results) const {
  time_us reconstructStartTime = NowMicros();
  Stream packedFrames = mixedFrame.getPackedFrames();
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
    if (maxRoI != nullptr && maxOverlap >= mConfig.BOX_FILTER_OVERLAP_THRESHOLD) {
      // filter overly large boxes from mixed frame inference by PROBE_IOU_THRESHOLD
      if (maxRoI->isProbingRoI) {
        maxRoI->frame->probingBoxes.push_back(std::make_unique<BoundingBox>(
            UNASSIGNED_ID, reconstructBoxPos(box, maxRoI),
            box.confidence, box.label, maxRoI->origin));
        maxRoI->probingBox = maxRoI->frame->probingBoxes.rbegin()->get();
      } else {
        maxRoI->frame->boxes.push_back(std::make_unique<BoundingBox>(
            UNASSIGNED_ID, reconstructBoxPos(box, maxRoI),
            box.confidence, box.label, origin_Null));
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

void PatchReconstructor::matchBoxesWithChildRoIs(Frame* frame, bool isFullFrame) const {
  std::vector<std::unique_ptr<RoI>>& childRoIs = frame->childRoIs;
  std::vector<std::unique_ptr<BoundingBox>>& boxes = frame->boxes;

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
    float maxIoU = 0;
    RoI* maxRoI = nullptr;
    for (auto& cRoI : childRoIs) {
      if (isFullFrame || cRoI->parentRoI->isPacked()) {
        float iou = cRoI->paddedLoc.iou(box->location);
        assert(box->location.area() != 0);
        if (maxIoU < iou) {
          maxIoU = iou;
          maxRoI = cRoI.get();
        }
      }
    }
    // if overlap is large enough, assign box to roi.boxes
    // else that bounding box is considered as newly appeared object
    if (maxRoI != nullptr && maxIoU >= mConfig.ID_MAPPING_IOU_THRESHOLD) {
      roiToBoxesMap[maxRoI].push_back(box.get());
      box->choiceOfBox = maxRoI->id;
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
        if (!isFullFrame) {
          maxBox->origin = cRoI->origin;
        }
        cRoI->box = maxBox;
        cRoI->label = maxBox->label;

        // Collect all unselected boxes
        for (int i = 0; i < roiToBoxesMap[cRoIPtr].size(); ++i) {
          if (i == maxIndex) continue;
          unassignedBoxes.push_back(roiToBoxesMap[cRoIPtr][i]);
        }
      }
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
      if (isFullFrame) {
        box->origin = origin_NewFF;
      } else {
        box->origin = origin_NewMF;
      }
      assert(box->srcRoI == nullptr);
    }
  }
  // End of 3

  // 2. Update resize profile
  if (frame->isLastFrame) {
    for (auto& cRoI : childRoIs) {
      mRoIResizer->updateTable(cRoI.get());
    }
  } else {
    assert(std::all_of(childRoIs.begin(), childRoIs.end(),
                       [](const auto& cRoI) { return cRoI->roisForProbing.empty(); }));
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
