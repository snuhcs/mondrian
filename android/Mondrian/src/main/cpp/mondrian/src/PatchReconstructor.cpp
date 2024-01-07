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
    : config_(config), ROIResizer_(roiResizer) {}

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
      float intersection = (box.loc & static_cast<cv::Rect2f>(mergedROI->packedLoc())).area();
      float overlapRatio = intersection / box.loc.area();
      if (maxOverlap < overlapRatio) {
        maxOverlap = overlapRatio;
        maxROI = mergedROI;
      }
    }
    if (maxROI != nullptr && maxOverlap >= config_.BOX_FILTER_OVERLAP_THRES) {
      // filter overly large boxes from packed inference by PROBE_IOU_THRES
      if (maxROI->isProbing()) {
        std::unique_ptr<BoundingBox> probeBox(new BoundingBox(
            INVALID_OID, packedCanvas.pid, maxROI->reconstructBoxPos(box),
            box.confidence, box.label, BoxOrigin::PACKED_CANVAS));
        maxROI->setProbingBox(probeBox.get());
        maxROI->frame()->probingBoxesTable[Device::GPU].push_back(std::move(probeBox));
      } else {
        std::unique_ptr<BoundingBox> newBox(new BoundingBox(
            INVALID_OID, packedCanvas.pid, maxROI->reconstructBoxPos(box),
            box.confidence, box.label, BoxOrigin::PACKED_CANVAS));
        maxROI->frame()->boxes.push_back(std::move(newBox));
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
      float intersection = (boxes[i]->loc & rois[j]->origLoc).area();
      float iou = intersection / (boxes[i]->loc.area() + rois[j]->origLoc.area() - intersection);
      float overlap = (boxes[i]->loc & rois[j]->paddedLoc).area() / boxes[i]->loc.area();
      costMatrix[i][j] = iou * overlap;
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

  for (unsigned int i = 0; i < boxes.size(); i++) {
    if (costMatrix[i][assignment[i]] == max) {
      unassignedBoxes.push_back(boxes[i]);
      continue;
    }
    ROI* srcROI = rois[assignment[i]];
    boxes[i]->oid = srcROI->oid;
    boxes[i]->setSrcROI(srcROI);
    srcROI->setBox(boxes[i]);
    srcROI->setLabel(boxes[i]->label);
  }

  // 2. Handle unassigned boxes
  if (!unassignedBoxes.empty()) {
    auto [startOID, endOID] = ROI::getNewOIDs(unassignedBoxes.size());
    OID oid = startOID;
    for (BoundingBox* box : unassignedBoxes) {
      assert(oid < endOID);
      box->oid = oid++;
      if (isFullFrame) {
        box->origin = BoxOrigin::NEW_FULL_FRAME;
      } else {
        box->origin = BoxOrigin::NEW_PACKED_CANVAS;
      }
      assert(box->srcROI() == nullptr);
    }
  }

  // Test
  const auto testROIBoxConnection = [&boxes, &rois]() {
    for (BoundingBox* box : boxes) {
      assert(box->oid != INVALID_OID);
      if (box->srcROI() != nullptr) {
        assert((box->loc & box->srcROI()->paddedLoc).area() > 0);
        assert(box->srcROI()->box() == box);
        assert(box->srcROI()->label() == box->label);
        assert(box->srcROI()->oid == box->oid);
      }
    }
    for (auto& roi : rois) {
      assert(roi->oid != INVALID_OID);
      if (roi->box() != nullptr) {
        assert((roi->paddedLoc & roi->box()->loc).area() > 0);
        assert(roi->box()->oid == roi->oid);
      }
    }
  };

  // Update resize profile
  testROIBoxConnection();
  if (frame->isLastFrame) {
    for (ROI* roi : rois) {
      ROIResizer_->updateTable(roi, roi->mergedROI->targetDevice());
    }
  } else {
    assert(std::all_of(rois.begin(), rois.end(),
                       [](const ROI* roi) {
                         bool ret = true;
                         for (Device device : DEVICES) {
                           ret &= roi->roisForProbingTable.find(device)
                               == roi->roisForProbingTable.end();
                         }
                         return ret;
                       }));
  }
  testROIBoxConnection();
}

float PatchReconstructor::iouThres() const {
  return config_.FRAME_BOXES_IOU_THRES;
}

} // namespace md
