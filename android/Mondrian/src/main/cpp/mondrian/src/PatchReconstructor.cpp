#include "mondrian/PatchReconstructor.hpp"

#include <set>

#include "mondrian/BoundingBox.hpp"
#include "mondrian/Log.hpp"
#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Utils.hpp"
#include "mondrian/Hungarian.hpp"

namespace md {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config,
                                       ROIResizer* roiResizer)
    : mConfig(config), mROIResizer(roiResizer) {}

static Rect moveResizeROIPos(const MergedROI* mergedROI) {
  auto [rw, rh] = mergedROI->resizedMatWH();
  auto [x, y] = mergedROI->packedXY();
  auto packX = x + MergedROI::BORDER;
  auto packY = y + MergedROI::BORDER;
  auto newL = float(packX);
  auto newT = float(packY);
  auto newR = float(packX + rw);
  auto newB = float(packY + rh);
  assert(0 <= newL && 0 <= newT && newL <= newR && newT <= newB);
  return {newL, newT, newR, newB};
}

static Rect reconstructBoxPos(const BoundingBox& packedBox, const MergedROI* mergedROI) {
  float scale = mergedROI->targetScale();
  const Rect& packedBoxLoc = packedBox.loc;
  const Rect& mergedROILoc = mergedROI->loc();
  auto [x, y] = mergedROI->packedXY();
  auto packX = float(x + MergedROI::BORDER);
  auto packY = float(y + MergedROI::BORDER);
  auto width = float(mergedROI->frame()->width());
  auto height = float(mergedROI->frame()->height());
  float newL = std::max(0.0f, (packedBoxLoc.l - packX) / scale + mergedROILoc.l);
  float newT = std::max(0.0f, (packedBoxLoc.t - packY) / scale + mergedROILoc.t);
  float newR = std::min(width, (packedBoxLoc.r - packX) / scale + mergedROILoc.l);
  float newB = std::min(height, (packedBoxLoc.b - packY) / scale + mergedROILoc.t);
  assert(0 <= newL && 0 <= newT && newL <= newR && newT <= newB);
  return {newL, newT, newR, newB};
}

void PatchReconstructor::assignBoxesToFrame(PackedCanvas& packedCanvas,
                                            const std::vector<BoundingBox>& results) const {
  time_us reconstructStartTime = NowMicros();
  Stream packedFrames = packedCanvas.getPackedFrames();
  std::set<MergedROI*>& packedROIs = packedCanvas.packedROIs;

  for (MergedROI* packedROI : packedROIs) {
    if (!packedROI->isProbing()) {
      assert(
          std::any_of(packedROI->frame()->mergedROIs.begin(), packedROI->frame()->mergedROIs.end(),
                      [&packedROI](auto& mergedROICandidate) {
                        return mergedROICandidate.get() == packedROI;
                      }));
    }
  }
  for (const BoundingBox& box : results) {
    assert(box.srcROI() == nullptr);
    assert(box.oid == INVALID_OID);
  }

  // Insert boxes to appropriate Frame::boxes
  for (const BoundingBox& box : results) {
    float maxOverlap = 0;
    MergedROI* maxROI = nullptr;
    for (MergedROI* mergedROI : packedROIs) {
      assert(mergedROI->isPacked());
      float intersection = box.loc.intersection(moveResizeROIPos(mergedROI));
      float overlapRatio = intersection / box.loc.area;
      if (maxOverlap < overlapRatio) {
        maxOverlap = overlapRatio;
        maxROI = mergedROI;
      }
    }
    if (maxROI != nullptr && maxOverlap >= mConfig.BOX_FILTER_OVERLAP_THRES) {
      // filter overly large boxes from packed inference by PROBE_IOU_THRES
      if (maxROI->isProbing()) {
        maxROI->frame()->probingBoxes.push_back(std::make_unique<BoundingBox>(
            INVALID_OID, packedCanvas.pid, reconstructBoxPos(box, maxROI),
            box.confidence, box.label, Origin::FULL_FRAME));
        maxROI->setProbingBox(maxROI->frame()->probingBoxes.rbegin()->get());
      } else {
        maxROI->frame()->boxes.push_back(std::make_unique<BoundingBox>(
            INVALID_OID, packedCanvas.pid, reconstructBoxPos(box, maxROI),
            box.confidence, box.label, Origin::INVALID));
      }
    }
  }

  time_us reconstructEndTime = NowMicros();
  for (Frame* frame : packedFrames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
}


void PatchReconstructor::matchBoxesROIs(Frame* frame, bool isFullFrame) const {
  std::vector<std::unique_ptr<ROI>>& rois = frame->rois;
  std::vector<std::unique_ptr<MergedROI>>& mergedROIs = frame->mergedROIs;
  std::vector<std::unique_ptr<BoundingBox>>& boxes = frame->boxes;
  assert(std::all_of(rois.begin(), rois.end(), [](auto& roi) { return roi->oid != INVALID_OID; }));
  assert(std::all_of(boxes.begin(), boxes.end(), [](auto& box) { return box->oid == INVALID_OID; }));
  assert(std::all_of(rois.begin(), rois.end(), [](auto& roi) { return roi->box() == nullptr; }));
  assert(std::all_of(boxes.begin(), boxes.end(), [](auto& box) { return box->srcROI() == nullptr; }));

  std::vector<BoundingBox*> unassignedBoxes;

  // 1. Assign boxes to mergedROIs.
  // each box selects the mergedROI with the largest overlap
  std::map<MergedROI*, std::vector<BoundingBox*>> mergedROIToBoxesMap;
  for (std::unique_ptr<BoundingBox>& box : boxes) {
    float maxOverlap = 0;
    MergedROI* maxMergedROI = nullptr;

    for (auto& mergedROI : mergedROIs) {
      if (isFullFrame || mergedROI->isPacked()) {
        float intersection = box->loc.intersection(moveResizeROIPos(mergedROI.get()));
        float overlapRatio = intersection / box->loc.area;
        if (maxOverlap < overlapRatio) {
          maxOverlap = overlapRatio;
          maxMergedROI = mergedROI.get();
        }
      }
    }

    if (maxMergedROI == nullptr) {
      // cases :
      // - when mergedROIs. size == 0
      // else?
      unassignedBoxes.push_back(box.get());
      continue;
    }
    mergedROIToBoxesMap[maxMergedROI].push_back(box.get());
  }

  // 2. Assign boxes to ROIs.
  // within ROIs in merged ROI and boxes assigned to that merged ROI,
  // do hungarian matching to assign boxes to ROIs
  for (auto& mergedROI : mergedROIs) {
    // prepare two groups
    std::vector<BoundingBox*> boxesInMergedROI = mergedROIToBoxesMap[mergedROI.get()];
    if (boxesInMergedROI.empty()) {
      continue;
    }
    std::vector<ROI*> roisInMergedROI = mergedROI->rois();

    // prepare cost matrix. max is calculated to make it a minimization problem
    std::vector<std::vector<float>> costMatrix(boxesInMergedROI.size());
    float max = 0;
    for (int i = 0; i < boxesInMergedROI.size(); i++) {
      costMatrix[i].resize(roisInMergedROI.size());
      for (int j = 0; j < roisInMergedROI.size(); j++) {
        float iou = boxesInMergedROI[i]->loc.iou(roisInMergedROI[j]->paddedLoc);
        costMatrix[i][j] = iou;
        if (iou > max) {
          max = iou;
        }
      }
    }
    for (int i = 0; i < boxesInMergedROI.size(); i++) {
      for (int j = 0; j < roisInMergedROI.size(); j++) {
        costMatrix[i][j] = max - costMatrix[i][j];
      }
    }

    std::vector<int> assignment;
    // assignment[i] is the index of ROI assigned to boxesInMergedROI[i]
    // -1 means that box is not assigned to any ROI
    HungarianAlgorithm::Solve(costMatrix, assignment);

    for (unsigned int x = 0; x < costMatrix.size(); x++) {
      if (assignment[x] == -1) {
        unassignedBoxes.push_back(boxesInMergedROI[x]);
        continue;
      }

      ROI* srcROI = roisInMergedROI[assignment[x]];

      boxesInMergedROI[x]->oid = srcROI->oid;
      boxesInMergedROI[x]->setSrcROI(srcROI);
      if (!isFullFrame) {
        boxesInMergedROI[x]->origin = srcROI->origin();
      }
      srcROI->setBox(boxesInMergedROI[x]);
      srcROI->setLabel(boxesInMergedROI[x]->label);
    }
  }

  // 3. Handle unassigned boxes
  if (!unassignedBoxes.empty()) {
    auto [startOID, endOID] = ROI::getNewOIDs(unassignedBoxes.size());
    OID oid = startOID;
    for (BoundingBox* box : unassignedBoxes) {
      assert(oid < endOID);
      box->oid = oid++;
      if (isFullFrame) {
        box->origin = Origin::NEW_FULL_FRAME;
      } else {
        box->origin = Origin::NEW_PACKED_CANVAS;
      }
      assert(box->srcROI() == nullptr);
    }
  }

  // Test
  const auto testROIBoxConnection = [&boxes, &rois]() {
    for (const std::unique_ptr<BoundingBox>& box : boxes) {
      assert(box->oid != INVALID_OID);
      if (box->srcROI() != nullptr) {
        assert(box->srcROI()->box() == box.get());
        assert(box->srcROI()->label() == box->label);
        assert(box->srcROI()->oid == box->oid);
      }
    }
    for (auto& roi : rois) {
      assert(roi->oid != INVALID_OID);
      if (roi->box() != nullptr) {
        assert(roi->box()->oid == roi->oid);
      }
    }
  };

  // Update resize profile
  testROIBoxConnection();
  if (frame->isLastFrame) {
    for (auto& roi : rois) {
      mROIResizer->updateTable(roi.get());
    }
  } else {
    assert(std::all_of(rois.begin(), rois.end(),
                       [](const auto& roi) { return roi->roisForProbing.empty(); }));
  }
  testROIBoxConnection();
}

float PatchReconstructor::iouThres() const {
  return mConfig.FRAME_BOXES_IOU_THRES;
}

} // namespace md
