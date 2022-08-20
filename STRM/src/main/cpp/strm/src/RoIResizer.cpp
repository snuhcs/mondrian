#include "strm/RoIResizer.hpp"
#include "strm/DataType.hpp"

namespace rm {

const std::map<std::string, Predictor> RoIResizer::candidatePredictors = {
    {"VIRAT",   VIRAT},
    {"MTA",     MTA},
    {"YouTube", YouTube}
};

const std::map<std::string, std::vector<float>> RoIResizer::scalesForLevels = {
    {"VIRAT",   {0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f, 1.0f}},
    {"MTA",     {0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f, 1.0f}},
    {"Youtube", {0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f, 0.6f, 0.65f, 0.7f, 0.75f, 0.8f, 0.85f, 0.9f, 0.95f, 1.0f}}
};

RoIResizer::RoIResizer(const RoIResizerConfig& config)
    : mConfig(config),
      mScaleGranularity(5),
      mPredictor(candidatePredictors.at(config.TRAIN_DATA)),
      mTargetSize(scalesForLevels.at(config.TRAIN_DATA)) {}

std::pair<float, RoI::ScaleLevel> RoIResizer::getTargetScale(const idType id,
                                                             const RoI::Features& features) {
  assert(features.type == RoI::Type::OF);
  if (mConfig.RESIZE_SMOOTHING_FACTOR == 0) {
    return {mConfig.STATIC_TARGET_SCALE, RoI::scale_NULL};
  }
  auto targetLevel = static_cast<RoI::ScaleLevel>(getMaxVotedLevel(id, features));
  float targetScale = mTargetSize[(targetLevel + 1) * mScaleGranularity - 1];

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

int RoIResizer::getMaxVotedLevel(const idType id, const RoI::Features& features) {
  auto record = prevPredictionBuffer.find(id);
  if (record == prevPredictionBuffer.end()) {
    prevPredictionBuffer[id] = CircularBuffer();
  }
  prevPredictionBuffer[id].push(predictLevelWithFeatures(features));
  return prevPredictionBuffer[id].maxVote();
}

int RoIResizer::predictLevelWithFeatures(const RoI::Features& features) const {
  assert(features.type == RoI::OF);
  auto&[avgX, avgY] = features.ofFeatures.avgShift;
  auto&[stdX, stdY] = features.ofFeatures.stdShift;
  float avg = avgX * avgX + avgY * avgY;
  float std = stdX * stdX + stdY * stdY;
  return int(mPredictor(
      features.width, features.height, std::max(features.width, features.height), features.type,
      features.origin, features.xyRatio, avgX, avgY, avg, stdX, stdY, std,
      features.ofFeatures.avgErr, features.ofFeatures.ncc));
}

void RoIResizer::updateTable(RoI* roi) {
  assert(!roi->roisForProbing.empty());
  assert(roi->roisForProbing.back()->getTargetScale() > roi->getTargetScale());
  assert(prevPredictionBuffer.find(roi->id) != prevPredictionBuffer.end());

  // Sort : Largest box first
  std::sort(roi->roisForProbing.begin(), roi->roisForProbing.end(),
            [](const auto& r, const auto& l) { return r->getTargetScale() > l->getTargetScale(); });

  // find box from largest RoI
  RoI* largestRoI = roi->roisForProbing.front();
  BoundingBox* box = roi->box;

  float newScale;
  if (box == nullptr) {
    // if box is found nowhere, just return
    return;
  } else {
    // if box is found for the largest probe, find the smallest target size with a usable box
    newScale = roi->getTargetScale();
    for (auto& probeRoI : roi->roisForProbing) {
      if (probeRoI->probingBox != nullptr && isUsable(probeRoI->probingBox, box)) {
        newScale = probeRoI->getTargetScale();
      } else {
        break;
      }
    }
  }
  calibrationTable[roi->id] = {roi->getScaleLevel(), newScale};
}

std::vector<float> RoIResizer::getProbingCandidates(float scale, RoI::ScaleLevel level,
                                                    int probeStep) {
  std::vector<float> candidates;

  auto const it = std::lower_bound(mTargetSize.begin(), mTargetSize.end(), scale);
  int index = int(std::distance(mTargetSize.begin(), it)) - 1;
  int lowerLevelIndex = level * mScaleGranularity - 1;
  int margin = std::max(-1, lowerLevelIndex);

  while (index > margin && candidates.size() < probeStep) {
    candidates.push_back(mTargetSize[index--]);
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

RoIResizer::CircularBuffer::CircularBuffer()
    : capacity_(5), oldest_index(0), size_(0) {
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
  std::vector<size_t> count(3, 0);
  for (int i = 0; i < size_; i++) {
    ++count[data_[i]];
  }
  return int(std::max_element(count.begin(), count.end()) - count.begin());
}

} // namespace rm
