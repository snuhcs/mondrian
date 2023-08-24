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
        maxROI->frame()->probingBoxesTable[Device::GPU].push_back(std::make_unique<BoundingBox>(
            INVALID_OID, packedCanvas.pid, reconstructBoxPos(box, maxROI),
            box.confidence, box.label, Origin::FULL_FRAME));
        maxROI->setProbingBox(maxROI->frame()->probingBoxesTable[Device::GPU].rbegin()->get());
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
  // 1. Assign boxes to ROIs.
  // do hungarian matching to assign boxes to ROIs
  // prepare two groups
  std::vector<BoundingBox*> boxes;
  std::vector<ROI*> rois;
  std::transform(frame->boxes.begin(), frame->boxes.end(),
                 std::back_inserter(boxes),
                 [](const std::unique_ptr<BoundingBox>& box) { return box.get(); });
  std::transform(frame->rois.begin(), frame->rois.end(),
                 std::back_inserter(rois),
                 [](const std::unique_ptr<ROI>& roi) { return roi.get(); });
  assert(std::all_of(rois.begin(), rois.end(), [](auto& roi) { return roi->oid != INVALID_OID; }));
  assert(std::all_of(boxes.begin(),
                     boxes.end(),
                     [](auto& box) { return box->oid == INVALID_OID; }));
  assert(std::all_of(rois.begin(), rois.end(), [](auto& roi) { return roi->box() == nullptr; }));
  assert(std::all_of(boxes.begin(),
                     boxes.end(),
                     [](auto& box) { return box->srcROI() == nullptr; }));

  std::vector<BoundingBox*> unassignedBoxes;

  // prepare cost matrix. max is calculated to make it a minimization problem
  std::vector<std::vector<float>> costMatrix;
  int length = (int) std::max(boxes.size(), rois.size());
  costMatrix.resize(length);
  for (int i = 0; i < length; i++) {
    costMatrix[i].resize(length);
  }
  float max = 0;
  for (int i = 0; i < boxes.size(); i++) {
    for (int j = 0; j < rois.size(); j++) {
      float iou = boxes[i]->loc.iou(rois[j]->paddedLoc);
      costMatrix[i][j] = iou;
      if (iou > max) {
        max = iou;
      }
    }
  }
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < length; j++) {
      costMatrix[i][j] = max - costMatrix[i][j];
    }
  }

  std::vector<int> assignment;
  // assignment[i] is the index of ROI assigned to boxes[i]
  // -1 means that box is not assigned to any ROI
  HungarianAlgorithm::Solve(costMatrix, assignment);

  for (unsigned int x = 0; x < boxes.size(); x++) {
    if (assignment[x] == -1 || assignment[x] >= rois.size()) {
      unassignedBoxes.push_back(boxes[x]);
      continue;
    }

    ROI* srcROI = rois[assignment[x]];

    boxes[x]->oid = srcROI->oid;
    boxes[x]->setSrcROI(srcROI);
    if (!isFullFrame) {
      boxes[x]->origin = srcROI->origin();
    }
    srcROI->setBox(boxes[x]);
    srcROI->setLabel(boxes[x]->label);
  }

  // 2. Handle unassigned boxes
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
    for (BoundingBox* box : boxes) {
      assert(box->oid != INVALID_OID);
      if (box->srcROI() != nullptr) {
        assert(box->srcROI()->box() == box);
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
    for (ROI* roi : rois) {
      mROIResizer->updateTable(roi, roi->mergedROI->getTargetDevice());
    }
  } else {
    assert(std::all_of(rois.begin(), rois.end(),
                       [](const auto& roi) {
                         bool ret = true;
                         for (Device device : Devices) {
                           ret &= roi->roisForProbingTable[device].empty();
                         }
                         return ret;
                       }));
  }
  testROIBoxConnection();
}

float PatchReconstructor::iouThres() const {
  return mConfig.FRAME_BOXES_IOU_THRES;
}

} // namespace md
