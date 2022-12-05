#include "strm/RoIResizer.hpp"

#include <set>

#include "strm/DataType.hpp"
#include "strm/Frame.hpp"
#include "strm/Log.hpp"

namespace rm {

const int RoIResizer::STATIC_LEVEL = 0;

const int RoIResizer::INVALID_LEVEL = -1;

const std::map<std::string, Predictor> RoIResizer::candidatePredictors = {
    {"MTA",   MTA},
    {"VIRAT", VIRAT}
};

const std::map<std::string, std::vector<float>> RoIResizer::scalesForLevels = {
    {"VIRAT", {0.2f, 0.4f, 0.9f, 1.0f}},
    {"MTA",   {0.2f, 0.3f, 0.5f, 0.7f, 1.0f}}
};

static const auto toVec = [](float staticScale) -> std::vector<float> {
  return {staticScale};
};

RoIResizer::RoIResizer(const RoIResizerConfig& config)
    : mConfig(config),
      mPredictor(candidatePredictors.at(config.TRAIN_DATA)),
      mTargetScales(config.STATIC_SCALE
                    ? toVec(config.STATIC_TARGET_SCALE)
                    : scalesForLevels.at(config.TRAIN_DATA)) {}

std::pair<float, int> RoIResizer::getTargetScale(const idType id,
                                                 const Features& features,
                                                 const float maxEdgeLength) {
  auto[targetScale, targetLevel] = getTargetScale(id, features);
  if (maxEdgeLength * targetScale > mConfig.MAX_OF_ROI_SIZE) {
    for (int level = int(mTargetScales.size()) - 1; level >= 0; level--) {
      if (maxEdgeLength * getTargetScale(level) <= mConfig.MAX_OF_ROI_SIZE) {
        targetScale = mTargetScales[level];
        break;
      }
    }
    while (maxEdgeLength * targetScale > mConfig.MAX_OF_ROI_SIZE) {
      targetScale -= mConfig.PROBE_STEP_SIZE;
    }
    assert(targetScale > 0.0f);
  }
  return {targetScale, targetLevel};
}

std::pair<float, int> RoIResizer::getTargetScale(const idType id,
                                                 const Features& features) {
  assert(features.type == OF);
  const int targetLevel = getMaxVotedLevel(id, features);
  assert(0 <= targetLevel && targetLevel < mTargetScales.size());
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
  return std::min(1.0f, mTargetScales[scaleLevel] + mConfig.SCALE_SHIFT);
}

int RoIResizer::getMaxVotedLevel(const idType id, const Features& features) {
  auto record = prevPredictionBuffer.find(id);
  if (record == prevPredictionBuffer.end()) {
    prevPredictionBuffer[id] = CircularBuffer(int(mTargetScales.size()));
  }
  prevPredictionBuffer[id].push(predictLevelWithFeatures(features));
  return prevPredictionBuffer[id].maxVote();
}

int RoIResizer::predictLevelWithFeatures(const Features& features) const {
  if (mConfig.STATIC_SCALE) {
    return STATIC_LEVEL;
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
  assert(prevPredictionBuffer.find(cRoI->id) != prevPredictionBuffer.end());
  if (cRoI->roisForProbing.empty()) {
    return;
  }

  std::map<float, RoI*> probingRoIs;
  for (RoI* probeRoI : cRoI->roisForProbing) {
    probingRoIs[probeRoI->getTargetScale()] = probeRoI;
  }

  const auto addBoxToOriginal = [](RoI* cRoI, BoundingBox* probeBox) {
    auto copiedBox = std::make_unique<BoundingBox>(
        cRoI->id, probeBox->location, probeBox->confidence, probeBox->label, cRoI->origin);
    copiedBox->srcRoI = cRoI;
    cRoI->label = copiedBox->label;
    cRoI->box = copiedBox.get();
    cRoI->frame->boxes.push_back(std::move(copiedBox));
  };

  float originalRoIScale = cRoI->parentRoI->getTargetScale();
  if (probingRoIs.find(originalRoIScale) != probingRoIs.end()) {
    auto box = probingRoIs[originalRoIScale]->probingBox;
    if (cRoI->box == nullptr && box != nullptr) {
      addBoxToOriginal(cRoI, box);
    }
  }
  cRoI->probingBox = cRoI->box;
  probingRoIs[originalRoIScale] = cRoI;

  // find the smallest target size with a usable box
  auto[largestScale, largestRoI] = (*probingRoIs.rbegin());
  BoundingBox* referenceBox = largestRoI->probingBox;
  float newScale = std::min(1.0f, largestScale + mConfig.PROBE_STEP_SIZE);
  for (auto it = probingRoIs.rbegin(); it != probingRoIs.rend(); it++) {
    auto[scale, probeRoI] = *it;
    BoundingBox* probeBox = probeRoI->probingBox;
    if (isUsable(probeBox, referenceBox)) {
      newScale = scale;
    } else {
      break;
    }
  }

  if (cRoI->box != referenceBox && isUsable(referenceBox, referenceBox)) {
    if (cRoI->box != nullptr) {
      cRoI->box->id = cRoI->id;
      cRoI->box->location = referenceBox->location;
      cRoI->box->confidence = referenceBox->confidence;
      assert(cRoI->box->origin == Origin::origin_BB
             && cRoI->box->origin == Origin::origin_PD
             && cRoI->box->origin == Origin::origin_NewMF);
    } else {
      addBoxToOriginal(cRoI, referenceBox);
    }
  }
  calibrationTable[cRoI->id] = {cRoI->getScaleLevel(), newScale};
}

std::vector<float> RoIResizer::getProbingCandidates(
    float scale, int level, int numProbeSteps) {
  assert(0.0f <= scale && scale <= 1.0f);
  std::vector<float> candidates;
  if (mConfig.STATIC_SCALE) {
    for (int i = 0; i < numProbeSteps; i++) {
      candidates.push_back(scale - float(i + 1) * mConfig.PROBE_STEP_SIZE);
      candidates.push_back(scale + float(i + 1) * mConfig.PROBE_STEP_SIZE);
    }
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [](float candidate) {
                                      return candidate <= 1e-5 || 1.0f < candidate;
                                    }), candidates.end());
  } else {
    for (int i = 0; i < numProbeSteps; i++) {
      candidates.push_back(scale - float(i + 1) * mConfig.PROBE_STEP_SIZE);
    }
    float lowerBound = level == 0 ? float(1e-5) : getTargetScale(level - 1);
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [lowerBound](float candidate) {
                                      return candidate <= lowerBound;
                                    }), candidates.end());
  }
  return candidates;
}

bool RoIResizer::isUsable(BoundingBox* box, BoundingBox* referenceBox) const {
  return box != nullptr && referenceBox != nullptr
         && box->label == referenceBox->label
         && box->location.iou(referenceBox->location) > mConfig.PROBE_IOU_THRESHOLD
         && box->confidence > mConfig.PROBE_CONF_THRESHOLD;
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
