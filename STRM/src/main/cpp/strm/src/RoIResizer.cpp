#include "strm/RoIResizer.hpp"

namespace rm {

const std::map<std::string, std::function<float(
    float, float, float, float, float, float,
    float, float, float, float, float, float)>> RoIResizer::candidatePredictors = {
    {"VIRAT",   VIRAT},
    {"MTA",     MTA},
    {"YouTube", YouTube}
};

const std::map<std::string, std::vector<float>> RoIResizer::candidateResizeTargets = {
    {"VIRAT",   {68.0f, 120.0f, 165.0f}},
    {"MTA",     {52.0f, 66.0f,  165.0f}},
    {"YouTube", {68.0f, 105.0f, 165.0f}}
};

RoIResizer::RoIResizer(const RoIResizerConfig& config)
    : mConfig(config), calibration(0),
      mPredictor(candidatePredictors.at(config.TRAIN_DATA)),
      mResizeTargets(candidateResizeTargets.at(config.TRAIN_DATA)) {}

float RoIResizer::getTargetSize(const idType id, const int frameIndex, const RoI::Features& features) {
  assert(features.type == RoI::Type::OF);
  if (mConfig.RESIZE_SMOOTHING_FACTOR == 0) {
    return mConfig.STATIC_RESIZE_TARGET;
  }
  float targetSize = getSmoothedTargetSize(id, frameIndex, features);
  prevTargetSizeTable[{id, frameIndex}] = targetSize;
  auto removeIt = prevTargetSizeTable.find({id, frameIndex - mConfig.MAX_CACHE_SIZE});
  if (removeIt != prevTargetSizeTable.end()) {
    prevTargetSizeTable.erase(removeIt);
  }
  if (calibrationStartSizeTable.find(id) == calibrationStartSizeTable.end() ||
      std::abs(targetSize - calibrationStartSizeTable[id]) > mConfig.PROBE_RESET_THRESHOLD) {
    calibrationStartSizeTable[id] = targetSize;
    calibration = 0;
  }
  float calibratedTargetSize = targetSize + calibration + mConfig.RESIZE_MARGIN;
  return std::max(calibratedTargetSize, 1.0f);
}

float RoIResizer::getSmoothedTargetSize(const idType id, const int frameIndex,
                                        const RoI::Features& features) {
  float sizeWithFeatures = getSizeWithFeature(features);
  auto it = prevTargetSizeTable.find({id, frameIndex - 1});
  if (it == prevTargetSizeTable.end()) {
    return sizeWithFeatures;
  }
  float prevTargetSize = it->second;
  return mConfig.RESIZE_SMOOTHING_FACTOR * sizeWithFeatures +
         (1 - mConfig.RESIZE_SMOOTHING_FACTOR) * prevTargetSize;
}

float RoIResizer::getSizeWithFeature(const RoI::Features& features) const {
  assert(features.type == RoI::OF);
  auto&[avgX, avgY] = features.ofFeatures.avgShift;
  auto&[stdX, stdY] = features.ofFeatures.stdShift;
  float avg = avgX * avgX + avgY * avgY;
  float std = stdX * stdX + stdY * stdY;
  return mResizeTargets[(int) mPredictor(
      features.width, features.height, (float) features.label, features.xyRatio,
      avgX, avgY, avg, stdX, stdY, std, features.ofFeatures.avgErr, features.ofFeatures.ncc)];
}

void RoIResizer::updateTable(RoI* roi) {
  assert(!roi->roisForProbing.empty());
  assert(roi->roisForProbing.back()->getTargetSize() > roi->getTargetSize());
  assert(prevTargetSizeTable.find({roi->id, roi->frame->frameIndex}) != prevTargetSizeTable.end());

  // Sort : Largest box first
  std::sort(roi->roisForProbing.begin(), roi->roisForProbing.end(),
            [](const auto& r, const auto& l) { return r->getTargetSize() > l->getTargetSize(); });

  // find box from largest RoI
  RoI* largestRoI = roi->roisForProbing.front();

  float newResizeTarget;
  if (largestRoI->probingBox == nullptr) {
    // if box is found nowhere, record to use even bigger size than biggest size
    newResizeTarget = roi->roisForProbing.front()->getTargetSize() + mConfig.PROBE_STEP_SIZE;
  } else {
    // if box is found for the largest probe, find the smallest target size with a usable box
    newResizeTarget = (float) largestRoI->getTargetSize();
    for (auto& probeRoI : roi->roisForProbing) {
      if (probeRoI->probingBox != nullptr &&
          isUsable(probeRoI->probingBox, largestRoI->probingBox)) {
        newResizeTarget = (float) probeRoI->getTargetSize();
      } else {
        break;
      }
    }
  }
  calibration = newResizeTarget - (float) roi->getTargetSize();
}

bool RoIResizer::isUsable(BoundingBox* targetBox, BoundingBox* baseBox) const {
  return (getOverlap(targetBox->location, baseBox->location) > mConfig.OVERLAP_THRESHOLD)
         && (targetBox->confidence > mConfig.ABSOLUTE_CONFIDENCE_THRESHOLD)
         && ((baseBox->confidence - targetBox->confidence) < mConfig.RELATIVE_CONFIDENCE_THRESHOLD);
}

float RoIResizer::getOverlap(Rect& targetRect, Rect& baseRect) {
  float intersection = targetRect.intersection(baseRect);
  float overlapRatio = intersection / (baseRect.area());
  return overlapRatio;
}

} // namespace rm
