#include "strm/impl/AccuracyAwareResizeProfile.hpp"

#include "strm/impl/DecisionTree.hpp"

namespace rm {

AccuracyAwareResizeProfile::AccuracyAwareResizeProfile(int resizeMargin,
                                                       float resizeSmoothingFactor,
                                                       int probeStep)
    : resizeMargin(resizeMargin), resizeSmoothingFactor(resizeSmoothingFactor),
      probeStep(probeStep), calibration(0) {
  assert(0 <= resizeSmoothingFactor && resizeSmoothingFactor <= 1);
}

int AccuracyAwareResizeProfile::getTargetSize(const idType id, const RoI::Features& features) {
  int targetSize = getSmoothedTargetSize(id, features);
  prevTargetSizeTable[id] = targetSize;
  return targetSize + calibration + resizeMargin;
}

int AccuracyAwareResizeProfile::getSmoothedTargetSize(const idType id,
                                                      const RoI::Features& features) {
  int sizeWithFeatures = getSizeWithFeature(features);
  auto record = prevTargetSizeTable.find(id);
  if (record == prevTargetSizeTable.end()) {
    return sizeWithFeatures;
  }
  int prevTargetSize = prevTargetSizeTable[id];
  return (int) (resizeSmoothingFactor * (float) sizeWithFeatures +
                (1 - resizeSmoothingFactor) * (float) prevTargetSize);
}

int AccuracyAwareResizeProfile::getSizeWithFeature(const RoI::Features& features) {
  if (features.type == RoI::OF) {
    return (int) OFTree(features.xyRatio, (float) features.getShiftSize(), features.err);
  } else if (features.type == RoI::PD) {
    return (int) PDTree(features.xyRatio, features.diffAreaRatio);
  }
  return INT_MAX / 2;
}

void AccuracyAwareResizeProfile::updateTable(RoI* roi) {
  assert(!roi->roisForProbing.empty());
  assert(roi->roisForProbing.back()->targetSize > roi->targetSize);
  assert(prevTargetSizeTable.find(roi->id) != prevTargetSizeTable.end());

  // Sort : Largest box first
  std::sort(roi->roisForProbing.begin(), roi->roisForProbing.end(),
            [](const auto& r, const auto& l) { return r->targetSize > l->targetSize; });

  // find box from largest RoI
  BoundingBox* boxFromLargestRoI = nullptr;
  for (auto& probeRoI : roi->roisForProbing) {
    if (probeRoI->probingBox != nullptr) {
      boxFromLargestRoI = probeRoI->probingBox;
      break;
    }
  }

  // if box is found nowhere, record to use even bigger size than biggest size
  if (boxFromLargestRoI == nullptr) {
    int newTargetSize = roi->roisForProbing.front()->targetSize + probeStep;
    calibration += newTargetSize - roi->targetSize;
    return;
  }

  // from largest RoI to smallest RoI, find smallest target size with marginal confidence & IoU loss
  int smallestSizePossible = boxFromLargestRoI->targetSize;
  for (auto& probeRoI : roi->roisForProbing) {
    BoundingBox* probeBox = probeRoI->probingBox;
    // if box not found, stop checking
    if (probeBox == nullptr) {
      break;
    }
    // check if the size is usable
    if (isUsable(*probeBox, *boxFromLargestRoI)) {
      smallestSizePossible = probeBox->targetSize;
    } else {
      break;
    }
  }
  calibration += smallestSizePossible - roi->targetSize;
}

float AccuracyAwareResizeProfile::getOverlap(Rect& targetRect, Rect& baseRect) {
  int intersection = targetRect.intersection(baseRect);
  float overlapRatio = (float) intersection / (float) (baseRect.area());
  return overlapRatio;
}

bool AccuracyAwareResizeProfile::isUsable(BoundingBox& targetBox, BoundingBox& baseBox) {
  float overlapThreshold = 0.8;
  float confidenceThreshold = 0.3;
  float confidenceDiffThreshold = 0.1;
  return (getOverlap(targetBox.location, baseBox.location) > overlapThreshold)
         && (targetBox.confidence > confidenceThreshold)
         && ((baseBox.confidence - targetBox.confidence) < confidenceDiffThreshold);
}

} // namespace rm
