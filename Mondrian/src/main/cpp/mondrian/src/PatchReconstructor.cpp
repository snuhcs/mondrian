#include "mondrian/PatchReconstructor.hpp"

#include <set>

#include "mondrian/Log.hpp"
#include "mondrian/MixedFrame.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Utils.hpp"

namespace md {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config,
                                       ROIResizer* roiResizer)
    : mConfig(config), mROIResizer(roiResizer) {}

static Rect moveResizeROIPos(const ROI* pROI) {
  auto[rw, rh] = pROI->getResizedMatWidthHeight();
  auto[x, y] = pROI->getPackedXY();
  auto packX = float(x + pROI->roiBorder);
  auto packY = float(y + pROI->roiBorder);
  return {float(packX), float(packY), float(packX + rw), float(packY + rh)};
}

static Rect reconstructBoxPos(const BoundingBox& packedBox, const ROI* pROI) {
  float scale = pROI->getTargetScale();
  const Rect& packedBoxLoc = packedBox.location;
  const Rect& pROILoc = pROI->paddedLoc;
  auto[x, y] = pROI->getPackedXY();
  auto packX = float(x + pROI->roiBorder);
  auto packY = float(y + pROI->roiBorder);
  float newL = (packedBoxLoc.l - packX) / scale + pROILoc.l;
  float newT = (packedBoxLoc.t - packY) / scale + pROILoc.t;
  float newR = (packedBoxLoc.r - packX) / scale + pROILoc.l;
  float newB = (packedBoxLoc.b - packY) / scale + pROILoc.t;
  return {std::max(0.0f, newL),
          std::max(0.0f, newT),
          std::min(float(pROI->frame->rgbMat.cols), newR),
          std::min(float(pROI->frame->rgbMat.rows), newB)};
}

void PatchReconstructor::assignBoxesToFrame(MixedFrame& mixedFrame,
                                            const std::vector<BoundingBox>& results) const {
  time_us reconstructStartTime = NowMicros();
  Stream packedFrames = mixedFrame.getPackedFrames();
  std::set<ROI*>& packedROIs = mixedFrame.packedROIs;

  for (ROI* pROI : packedROIs) {
    if (!pROI->isProbingROI) {
      assert(std::any_of(pROI->frame->parentROIs.begin(), pROI->frame->parentROIs.end(),
                         [&pROI](auto& pROICandidate) { return pROICandidate.get() == pROI; }));
    }
  }
  for (const BoundingBox& box : results) {
    assert(box.srcROI == nullptr);
    assert(box.id == UNASSIGNED_ID);
  }

  // Insert boxes to appropriate frame.boxes
  for (const BoundingBox& box : results) {
    float maxOverlap = 0;
    ROI* maxROI = nullptr;
    for (ROI* pROI : packedROIs) {
      assert(pROI->isPacked());
      float intersection = box.location.intersection(moveResizeROIPos(pROI));
      float overlapRatio = intersection / box.location.area();
      if (maxOverlap < overlapRatio) {
        maxOverlap = overlapRatio;
        maxROI = pROI;
      }
    }
    if (maxROI != nullptr && maxOverlap >= mConfig.BOX_FILTER_OVERLAP_THRESHOLD) {
      // filter overly large boxes from mixed frame inference by PROBE_IOU_THRESHOLD
      if (maxROI->isProbingROI) {
        maxROI->frame->probingBoxes.push_back(std::make_unique<BoundingBox>(
            UNASSIGNED_ID, reconstructBoxPos(box, maxROI),
            box.confidence, box.label, maxROI->origin));
        maxROI->probingBox = maxROI->frame->probingBoxes.rbegin()->get();
      } else {
        maxROI->frame->boxes.push_back(std::make_unique<BoundingBox>(
            UNASSIGNED_ID, reconstructBoxPos(box, maxROI),
            box.confidence, box.label, origin_Null));
      }
    }
  }

  time_us reconstructEndTime = NowMicros();
  for (Frame* frame : packedFrames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
  LOGD("%-25s took %-7lld us for             mixed %-4d // %4lu packedROIs %4lu boxes",
       "PR::assignBoxesToFrame", reconstructEndTime - reconstructStartTime,
       mixedFrame.mixedFrameIndex, packedFrames.size(), results.size());
}

void PatchReconstructor::matchBoxesWithChildROIs(Frame* frame, bool isFullFrame) const {
  std::vector<std::unique_ptr<ROI>>& childROIs = frame->childROIs;
  std::vector<std::unique_ptr<BoundingBox>>& boxes = frame->boxes;

  std::vector<BoundingBox*> unassignedBoxes;

  assert(std::all_of(childROIs.begin(), childROIs.end(),
                     [](auto& cROI) { return cROI->id != UNASSIGNED_ID && cROI->box == nullptr; }));
  assert(std::all_of(boxes.begin(), boxes.end(),
                     [](auto& box) { return box->id == UNASSIGNED_ID && box->srcROI == nullptr; }));

  // 1. Let Boxes to select their favorite ROI.
  // - Boxes can be unmatched, if overlap ratio is lower than threshold
  // - Selection result is saved in roi.boxes
  std::map<ROI*, std::vector<BoundingBox*>> roiToBoxesMap;
  for (std::unique_ptr<BoundingBox>& box : boxes) {
    // find ROI with largest overlap
    float maxIoU = 0;
    ROI* maxROI = nullptr;
    for (auto& cROI : childROIs) {
      if (isFullFrame || cROI->parentROI->isPacked()) {
        float iou = cROI->paddedLoc.iou(box->location);
        assert(box->location.area() != 0);
        if (maxIoU < iou) {
          maxIoU = iou;
          maxROI = cROI.get();
        }
      }
    }
    // if overlap is large enough, assign box to roi.boxes
    // else that bounding box is considered as newly appeared object
    if (maxROI != nullptr && maxIoU >= mConfig.ID_MAPPING_IOU_THRESHOLD) {
      roiToBoxesMap[maxROI].push_back(box.get());
      box->choiceOfBox = maxROI->id;
    } else {
      unassignedBoxes.push_back(box.get());
    }
  }
  // End of 1

  // 2. Let ROIs select their favorite Box
  // - Selection result is saved as ROI with same id
  for (auto& cROI : childROIs) {
    if (isFullFrame || cROI->parentROI->isPacked()) {
      ROI* cROIPtr = cROI.get();
      float maxIntersection = -1.0f;
      int maxIndex = -1;
      for (int i = 0; i < roiToBoxesMap[cROIPtr].size(); ++i) {
        BoundingBox* box = roiToBoxesMap[cROIPtr][i];
        float intersection = cROI->paddedLoc.intersection(box->location);
        if (maxIntersection < intersection) {
          maxIntersection = intersection;
          maxIndex = i;
        }
      }
      if (!roiToBoxesMap[cROIPtr].empty()) {
        // 1-1 matching with ROI & box
        assert(maxIndex >= 0 && maxIndex < roiToBoxesMap[cROIPtr].size());
        BoundingBox* maxBox = roiToBoxesMap[cROIPtr][maxIndex];
        maxBox->id = cROI->id;
        maxBox->srcROI = cROIPtr;
        if (!isFullFrame) {
          maxBox->origin = cROI->origin;
        }
        cROI->box = maxBox;
        cROI->label = maxBox->label;

        // Collect all unselected boxes
        for (int i = 0; i < roiToBoxesMap[cROIPtr].size(); ++i) {
          if (i == maxIndex) continue;
          unassignedBoxes.push_back(roiToBoxesMap[cROIPtr][i]);
        }
      }
    }
  }
  // End of 2

  // 3. Assign new IDs to newly appeared objects
  // - Those are
  //   a) who does not match with any ROI
  //   b) those who lost competition between other Boxes wrt ROI's selection
  if (!unassignedBoxes.empty()) {
    std::pair<idType, idType> idRange = ROI::getNewIds(unassignedBoxes.size());
    idType id = idRange.first;
    for (BoundingBox* box : unassignedBoxes) {
      assert(id < idRange.second);
      box->id = id++;
      if (isFullFrame) {
        box->origin = origin_NewFF;
      } else {
        box->origin = origin_NewMF;
      }
      assert(box->srcROI == nullptr);
    }
  }
  // End of 3

  const auto testROIBoxConnection = [&boxes, &childROIs](){
    for (const std::unique_ptr<BoundingBox>& box : boxes) {
      assert(box->id != UNASSIGNED_ID);
      if (box->srcROI != nullptr) {
        assert(box->srcROI->box == box.get());
        assert(box->srcROI->label == box->label);
        assert(box->srcROI->id == box->id);
      }
    }
    for (auto& cROI : childROIs) {
      assert(cROI->id != UNASSIGNED_ID);
      if (cROI->box != nullptr) {
        assert(cROI->box->id == cROI->id);
      }
    }
  };

  // 2. Update resize profile
  testROIBoxConnection();
  if (frame->isLastFrame) {
    for (auto& cROI : childROIs) {
      mROIResizer->updateTable(cROI.get());
    }
  } else {
    assert(std::all_of(childROIs.begin(), childROIs.end(),
                       [](const auto& cROI) { return cROI->roisForProbing.empty(); }));
  }
  testROIBoxConnection();
}

float PatchReconstructor::getIoUThreshold() const {
  return mConfig.FRAME_BOXES_IOU_THRESHOLD;
}

} // namespace md
