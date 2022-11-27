#include "strm/RoIResizer.hpp"
#include "strm/DataType.hpp"

namespace rm {

const int RoIResizer::INVALID_LEVEL = -1;

const std::map<std::string, Predictor> RoIResizer::candidatePredictors = {
    {"MTA",   MTA},
    {"VIRAT", VIRAT}
};

const std::map<std::string, std::vector<float>> RoIResizer::scalesForLevels = {
    {"VIRAT", {0.2f, 0.4f, 0.9f, 1.0f}},
    {"MTA",   {0.2f, 0.3f, 0.5f, 0.7f, 1.0f}}
};

RoIResizer::RoIResizer(const RoIResizerConfig& config)
    : mConfig(config),
      mPredictor(candidatePredictors.at(config.TRAIN_DATA)),
      mTargetSize(scalesForLevels.at(config.TRAIN_DATA)) {}

std::pair<float, int> RoIResizer::getTargetScale(const idType id,
                                                 const Features& features,
                                                 const float maxEdgeLength) {
  auto[targetScale, targetLevel] = getTargetScale(id, features);
  int resizedEdgeLength = RoI::getResizedMatEdgeLength(maxEdgeLength, targetScale);
  if (float(resizedEdgeLength) > mConfig.MAX_OF_ROI_SIZE) {
    targetScale = mConfig.MAX_OF_ROI_SIZE / maxEdgeLength;
    targetLevel = INVALID_LEVEL;
  }
  return {targetScale, targetLevel};
}

std::pair<float, int> RoIResizer::getTargetScale(const idType id,
                                                 const Features& features) {
  assert(features.type == OF);
  const int targetLevel = getMaxVotedLevel(id, features);
  assert(0 <= targetLevel && targetLevel < mTargetSize.size());
  float targetScale = getTargetScale(targetLevel);

  auto it = calibrationTable.find(id);
  if (it != calibrationTable.end()) {
    if (targetLevel == calibrationTable[id].first) {
      // in same Level
      targetScale = calibrationTable[id].second;
    } else {
      // out of threshold range
      calibrationTable.erase(it);
    }
  }
  return {targetScale, targetLevel};
}

float RoIResizer::getTargetScale(const int scaleLevel) {
  if (mConfig.STATIC_SCALE) {
    return mConfig.STATIC_TARGET_SCALE;
  }
  return std::min(1.0f, mTargetSize[scaleLevel] + mConfig.SCALE_SHIFT);
}

int RoIResizer::getMaxVotedLevel(const idType id, const Features& features) {
  auto record = prevPredictionBuffer.find(id);
  if (record == prevPredictionBuffer.end()) {
    prevPredictionBuffer[id] = CircularBuffer(mTargetSize.size());
  }
  prevPredictionBuffer[id].push(predictLevelWithFeatures(features));
  return prevPredictionBuffer[id].maxVote();
}

int RoIResizer::predictLevelWithFeatures(const Features& features) const {
  if (mConfig.STATIC_SCALE) {
    return 0;
  }
  assert(features.type == OF);
  auto&[shiftAvgX, shiftAvgY] = features.ofFeatures.shiftAvg;
  auto&[shiftStdX, shiftStdY] = features.ofFeatures.shiftStd;
  float shiftAvg = shiftAvgX * shiftAvgX + shiftAvgY * shiftAvgY;
  float shiftStd = shiftStdX * shiftStdX + shiftStdY * shiftStdY;
  return mPredictor(
      std::max(features.width, features.height),
      features.width * features.height,
      features.xyRatio,
      shiftAvg, shiftStd,
      features.ofFeatures.shiftNcc,
      features.ofFeatures.avgErr,
      features.confidence);
}

void RoIResizer::updateTable(RoI* cRoI) {
  assert(!cRoI->roisForProbing.empty());
  assert(prevPredictionBuffer.find(cRoI->id) != prevPredictionBuffer.end());

  // Sort : Largest box first
  std::sort(cRoI->roisForProbing.begin(), cRoI->roisForProbing.end(),
            [](const auto& l, const auto& r) { return l->getTargetScale() > r->getTargetScale(); });
  assert(cRoI->roisForProbing.front()->getTargetScale() < cRoI->getTargetScale());

  // find box from largest RoI
  RoI* largestProbingRoI = cRoI->roisForProbing.front();
  BoundingBox* box = largestProbingRoI->probingBox;

  if (box == nullptr) {
    // if box is found nowhere, just return
    return;
  }

  // if box is found for the largest probe, find the smallest target size with a usable box
  float newScale = largestProbingRoI->getTargetScale();
  for (auto& probeRoI: cRoI->roisForProbing) {
    if (probeRoI->probingBox != nullptr && isUsable(probeRoI->probingBox, box)) {
      newScale = probeRoI->getTargetScale();
    } else {
      break;
    }
  }
  calibrationTable[cRoI->id] = {cRoI->getScaleLevel(), newScale};
}

std::vector<float> RoIResizer::getProbingCandidates(
    float scale, int level, int numProbeSteps) {
  float lowerLevelScale = level == 0 ? 0.0f : getTargetScale(level - 1);
  std::vector<float> candidates;
  for (int i = 0; i < numProbeSteps; i++) {
    float candidate = scale - float(i + 1) * mConfig.PROBE_STEP_SIZE;
    if (candidate > lowerLevelScale) {
      candidates.push_back(candidate);
    } else {
      break;
    }
  }
  return candidates;
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

RoIResizer::CircularBuffer::CircularBuffer(int numLevels)
    : capacity_(5), oldest_index(0), size_(0), numLevels(numLevels) {
  // NOTE that capacity should be even number to avoid tie
  data_.resize(capacity_);
}

void RoIResizer::CircularBuffer::push(int data) {
  data_[oldest_index] = data;
  oldest_index = (oldest_index + 1) % capacity_;
  if (size_ < capacity_) {
    ++size_;
  }
}

int RoIResizer::CircularBuffer::maxVote() {
  std::vector<size_t> count(numLevels, 0);
  for (int i = 0; i < size_; i++) {
    ++count[data_[i]];
  }
  return int(std::max_element(count.begin(), count.end()) - count.begin());
}

} // namespace rm
