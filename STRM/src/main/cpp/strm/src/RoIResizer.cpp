#include "strm/RoIResizer.hpp"

#include "strm/DecisionTree.hpp"

namespace rm {

RoIResizer::RoIResizer(const RoIResizerConfig& config)
    : mConfig(config), calibration(0) {}

float RoIResizer::getStaticTargetSize() const {
  return mConfig.STATIC_RESIZE_TARGET;
}

float RoIResizer::getTargetSize(const idType id, const RoI::Features& features) {
  assert(features.type == RoI::Type::OF);
  if (mConfig.RESIZE_SMOOTHING_FACTOR == 0) {
    return mConfig.STATIC_RESIZE_TARGET;
  }
  float targetSize = getSmoothedTargetSize(id, features);
  prevTargetSizeTable[id] = targetSize;
  if (calibrationStartSizeTable.find(id) == calibrationStartSizeTable.end() ||
      std::abs(targetSize - calibrationStartSizeTable[id]) > mConfig.PROBE_RESET_THRESHOLD) {
    calibrationStartSizeTable[id] = targetSize;
    calibration = 0;
  }
  float calibratedTargetSize = targetSize + calibration + mConfig.RESIZE_MARGIN;
  return std::max(calibratedTargetSize, 1.0f);
}

float RoIResizer::getSmoothedTargetSize(const idType id,
                                        const RoI::Features& features) {
  float sizeWithFeatures = getSizeWithFeature(features);
  auto record = prevTargetSizeTable.find(id);
  if (record == prevTargetSizeTable.end()) {
    return sizeWithFeatures;
  }
  float prevTargetSize = prevTargetSizeTable[id];
  return (mConfig.RESIZE_SMOOTHING_FACTOR * sizeWithFeatures +
          (1 - mConfig.RESIZE_SMOOTHING_FACTOR) * prevTargetSize);
}

float RoIResizer::getSizeWithFeature(const RoI::Features& features) {
  assert(features.type == RoI::OF);
  auto&[x, y] = features.ofFeatures.avgShift;
  float shiftSize = x * x + y * y;
  return OFTree(features.xyRatio, shiftSize, features.ofFeatures.avgErr);
}

void RoIResizer::updateTable(RoI* roi) {
  assert(!roi->roisForProbing.empty());
  assert(roi->roisForProbing.back()->targetSize > roi->targetSize);
  assert(prevTargetSizeTable.find(roi->id) != prevTargetSizeTable.end());

  // Sort : Largest box first
  std::sort(roi->roisForProbing.begin(), roi->roisForProbing.end(),
            [](const auto& r, const auto& l) { return r->targetSize > l->targetSize; });

  // find box from largest RoI
  RoI* largestRoI = roi->roisForProbing.front();

  float newResizeTarget;
  if (largestRoI->probingBox == nullptr) {
    // if box is found nowhere, record to use even bigger size than biggest size
    newResizeTarget = (float) roi->roisForProbing.front()->targetSize
                      + (float) mConfig.PROBE_STEP_SIZE;
  } else {
    // if box is found for the largest probe, find the smallest target size with a usable box
    newResizeTarget = (float) largestRoI->targetSize;
    for (auto& probeRoI : roi->roisForProbing) {
      if (probeRoI->probingBox != nullptr &&
          isUsable(probeRoI->probingBox, largestRoI->probingBox)) {
        newResizeTarget = (float) probeRoI->targetSize;
      } else {
        break;
      }
    }
  }
  calibration = newResizeTarget - (float) roi->targetSize;
}

bool RoIResizer::isUsable(BoundingBox* targetBox, BoundingBox* baseBox) const {
  return (getOverlap(targetBox->location, baseBox->location) > mConfig.OVERLAP_THRESHOLD)
         && (targetBox->confidence > mConfig.ABSOLUTE_CONFIDENCE_THRESHOLD)
         && ((baseBox->confidence - targetBox->confidence) < mConfig.RELATIVE_CONFIDENCE_THRESHOLD);
}

float RoIResizer::getOverlap(Rect& targetRect, Rect& baseRect) {
  int intersection = targetRect.intersection(baseRect);
  float overlapRatio = (float) intersection / (float) (baseRect.area());
  return overlapRatio;
}

} // namespace rm
