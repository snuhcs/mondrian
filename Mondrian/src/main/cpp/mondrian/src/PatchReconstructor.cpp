#include "mondrian/PatchReconstructor.hpp"

#include <set>

#include "mondrian/Log.hpp"
#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Utils.hpp"

namespace md {

PatchReconstructor::PatchReconstructor(const PatchReconstructorConfig& config,
                                       ROIResizer* roiResizer)
    : mConfig(config), mROIResizer(roiResizer) {}

static Rect moveResizeROIPos(const MergedROI* mergedROI) {
  auto[rw, rh] = mergedROI->resizedMatWH();
  auto[x, y] = mergedROI->packedXY();
  auto packX = x + MergedROI::BORDER;
  auto packY = y + MergedROI::BORDER;
  return {float(packX), float(packY), float(packX + rw), float(packY + rh)};
}

static Rect reconstructBoxPos(const BoundingBox& packedBox, const MergedROI* mergedROI) {
  float scale = mergedROI->targetScale();
  const Rect& packedBoxLoc = packedBox.loc;
  const Rect& mergedROILoc = mergedROI->loc();
  auto[x, y] = mergedROI->packedXY();
  auto packX = float(x + MergedROI::BORDER);
  auto packY = float(y + MergedROI::BORDER);
  float newL = (packedBoxLoc.l - packX) / scale + mergedROILoc.l;
  float newT = (packedBoxLoc.t - packY) / scale + mergedROILoc.t;
  float newR = (packedBoxLoc.r - packX) / scale + mergedROILoc.l;
  float newB = (packedBoxLoc.b - packY) / scale + mergedROILoc.t;
  return {std::max(0.0f, newL),
          std::max(0.0f, newT),
          std::min(float(mergedROI->frame()->rgbMat.cols), newR),
          std::min(float(mergedROI->frame()->rgbMat.rows), newB)};
}

void PatchReconstructor::assignBoxesToFrame(PackedCanvas& packedCanvas,
                                            const std::vector<BoundingBox>& results) const {
  time_us reconstructStartTime = NowMicros();
  Stream packedFrames = packedCanvas.getPackedFrames();
  std::set<MergedROI*>& packedROIs = packedCanvas.packedROIs;

  for (MergedROI* packedROI: packedROIs) {
    if (!packedROI->isProbing()) {
      assert(
          std::any_of(packedROI->frame()->mergedROIs.begin(), packedROI->frame()->mergedROIs.end(),
                      [&packedROI](auto& mergedROICandidate) {
                        return mergedROICandidate.get() == packedROI;
                      }));
    }
  }
  for (const BoundingBox& box: results) {
    assert(box.srcROI == nullptr);
    assert(box.id == INVALID_ID);
  }

  // Insert boxes to appropriate Frame::boxes
  for (const BoundingBox& box: results) {
    float maxOverlap = 0;
    MergedROI* maxROI = nullptr;
    for (MergedROI* mergedROI: packedROIs) {
      assert(mergedROI->isPacked());
      float intersection = box.loc.intersection(moveResizeROIPos(mergedROI));
      float overlapRatio = intersection / box.loc.area;
      if (maxOverlap < overlapRatio) {
        maxOverlap = overlapRatio;
        maxROI = mergedROI;
      }
    }
    if (maxROI != nullptr && maxOverlap >= mConfig.BOX_FILTER_OVERLAP_THRESHOLD) {
      // filter overly large boxes from packed inference by PROBE_IOU_THRESHOLD
      if (maxROI->isProbing()) {
        maxROI->frame()->probingBoxes.push_back(std::make_unique<BoundingBox>(
            INVALID_ID, reconstructBoxPos(box, maxROI),
            box.confidence, box.label, O_PACKED_CANVAS));
        maxROI->setProbingBox(maxROI->frame()->probingBoxes.rbegin()->get());
      } else {
        maxROI->frame()->boxes.push_back(std::make_unique<BoundingBox>(
            INVALID_ID, reconstructBoxPos(box, maxROI),
            box.confidence, box.label, O_INVALID));
      }
    }
  }

  time_us reconstructEndTime = NowMicros();
  for (Frame* frame: packedFrames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
  LOGD("%-25s took %-7lld us for             mixed %-4d // %4lu packedROIs %4lu boxes",
       "PR::assignBoxesToFrame", reconstructEndTime - reconstructStartTime,
       packedCanvas.absolutePackedCanvasIndex, packedFrames.size(), results.size());
}

void PatchReconstructor::matchBoxesROIs(Frame* frame, bool isFullFrame) const {
  std::vector<std::unique_ptr<ROI>>& rois = frame->rois;
  std::vector<std::unique_ptr<BoundingBox>>& boxes = frame->boxes;

  std::vector<BoundingBox*> unassignedBoxes;

  assert(std::all_of(rois.begin(), rois.end(),
                     [](auto& roi) { return roi->id != INVALID_ID && roi->box == nullptr; }));
  assert(std::all_of(boxes.begin(), boxes.end(),
                     [](auto& box) { return box->id == INVALID_ID && box->srcROI == nullptr; }));

  // 1. Let Boxes to select their favorite ROI.
  // - Boxes can be unmatched, if overlap ratio is lower than threshold
  // - Selection result is saved in roi.boxes
  std::map<ROI*, std::vector<BoundingBox*>> roiToBoxesMap;
  for (std::unique_ptr<BoundingBox>& box: boxes) {
    // find ROI with largest overlap
    float maxIoU = 0;
    ROI* maxROI = nullptr;
    for (auto& roi: rois) {
      if (isFullFrame || roi->mergedROI->isPacked()) {
        float iou = roi->paddedLoc.iou(box->loc);
        assert(box->loc.area != 0);
        if (maxIoU < iou) {
          maxIoU = iou;
          maxROI = roi.get();
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
  for (auto& roi: rois) {
    if (isFullFrame || roi->mergedROI->isPacked()) {
      ROI* roiPtr = roi.get();
      float maxIntersection = -1.0f;
      int maxIndex = -1;
      for (int i = 0; i < roiToBoxesMap[roiPtr].size(); ++i) {
        BoundingBox* box = roiToBoxesMap[roiPtr][i];
        float intersection = roi->paddedLoc.intersection(box->loc);
        if (maxIntersection < intersection) {
          maxIntersection = intersection;
          maxIndex = i;
        }
      }
      if (!roiToBoxesMap[roiPtr].empty()) {
        // 1-1 matching with ROI & box
        assert(maxIndex >= 0 && maxIndex < roiToBoxesMap[roiPtr].size());
        BoundingBox* maxBox = roiToBoxesMap[roiPtr][maxIndex];
        maxBox->id = roi->id;
        maxBox->srcROI = roiPtr;
        if (!isFullFrame) {
          maxBox->origin = roi->origin;
        }
        roi->box = maxBox;
        roi->label = maxBox->label;

        // Collect all unselected boxes
        for (int i = 0; i < roiToBoxesMap[roiPtr].size(); ++i) {
          if (i == maxIndex) continue;
          unassignedBoxes.push_back(roiToBoxesMap[roiPtr][i]);
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
    std::pair<ID, ID> idRange = ROI::getNewIds(unassignedBoxes.size());
    ID id = idRange.first;
    for (BoundingBox* box: unassignedBoxes) {
      assert(id < idRange.second);
      box->id = id++;
      if (isFullFrame) {
        box->origin = O_NEW_FULL_FRAME;
      } else {
        box->origin = O_NEW_PACKED_CANVAS;
      }
      assert(box->srcROI == nullptr);
    }
  }
  // End of 3

  const auto testROIBoxConnection = [&boxes, &rois]() {
    for (const std::unique_ptr<BoundingBox>& box: boxes) {
      assert(box->id != INVALID_ID);
      if (box->srcROI != nullptr) {
        assert(box->srcROI->box == box.get());
        assert(box->srcROI->label == box->label);
        assert(box->srcROI->id == box->id);
      }
    }
    for (auto& roi: rois) {
      assert(roi->id != INVALID_ID);
      if (roi->box != nullptr) {
        assert(roi->box->id == roi->id);
      }
    }
  };

  // 2. Update resize profile
  testROIBoxConnection();
  if (frame->isLastFrame) {
    for (auto& roi: rois) {
      mROIResizer->updateTable(roi.get());
    }
  } else {
    assert(std::all_of(rois.begin(), rois.end(),
                       [](const auto& roi) { return roi->roisForProbing.empty(); }));
  }
  testROIBoxConnection();
}

float PatchReconstructor::getIoUThreshold() const {
  return mConfig.FRAME_BOXES_IOU_THRESHOLD;
}

} // namespace md
