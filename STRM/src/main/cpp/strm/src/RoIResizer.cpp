#include "strm/RoIResizer.hpp"

#include <set>
#include <cmath>

#include "strm/DataType.hpp"
#include "strm/Frame.hpp"
#include "strm/Log.hpp"

namespace rm {

const int RoIResizer::STATIC_LEVEL = 0;

const int RoIResizer::INVALID_LEVEL = -1;

const std::map<std::string, Predictor> RoIResizer::candidatePredictors = {
    {"virat", VIRAT},
    {"mta",   MTA},
};

const std::map<std::string, std::vector<float>> RoIResizer::scalesForLevels = {
        {"virat", {
                          333.913147627257,
                          638.5106976744186,
                          764.6907839189453,
                          880.0315355329949,
                          1e10
                  }},
        {"mta",   {
                          368.62745098039215,
                          459.33734939759046,
                          558.7062937062936,
                          848.4255998838352,
                          1e10
                  }}
};

static const auto toVec = [](float staticScale) -> std::vector<float> {
  return {staticScale};
};

RoIResizer::RoIResizer(const RoIResizerConfig& config)
    : mConfig(config),
      mPredictor(candidatePredictors.at(config.TRAIN_DATA)),
      mTargetAreas(config.STATIC_SCALE
                    ? toVec(config.STATIC_TARGET_SCALE)
                    : scalesForLevels.at(config.TRAIN_DATA)) {}

std::pair<float, int> RoIResizer::getTargetScale(const idType id,
                                                 const Features& features,
                                                 const float maxEdgeLength) {
  auto[targetScale, targetLevel] = getTargetScale(id, features);
  float originalArea = features.width * features.height;
  if (maxEdgeLength * targetScale > mConfig.MAX_OF_ROI_SIZE) {
    for (int level = int(mTargetAreas.size()) - 1; level >= 0; level--) {
      float scale = getTargetScale(level, originalArea);
      if (maxEdgeLength * scale <= mConfig.MAX_OF_ROI_SIZE) {
        targetScale = scale;
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
  assert(0 <= targetLevel && targetLevel < mTargetAreas.size());
  float targetScale = getTargetScale(targetLevel, features.width * features.height);

  if (isCalibrated(id, targetLevel)) {
    targetScale = calibrationTable[id].second;
  } else if (calibrationTable.find(id) != calibrationTable.end()) {
    calibrationTable.erase(id);
  }
  return {targetScale, targetLevel};
}

float RoIResizer::getTargetScale(const int scaleLevel, const float originalArea) const {
  return std::min(1.0f, calculateTargetScale(mTargetAreas.at(scaleLevel), originalArea));
}

bool RoIResizer::isCalibrated(const idType id, const int scaleLevel) const {
  [](){}();
  return calibrationTable.find(id) != calibrationTable.end()
         && calibrationTable.at(id).first == scaleLevel;
}

int RoIResizer::getMaxVotedLevel(const idType id, const Features& features) {
  auto record = prevPredictionBuffer.find(id);
  if (record == prevPredictionBuffer.end()) {
    prevPredictionBuffer[id] = CircularBuffer(int(mTargetAreas.size()),
                                              mConfig.VOTING_WINDOW);
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
  RoI* selectedRoI = largestRoI;
  for (auto it = probingRoIs.rbegin(); it != probingRoIs.rend(); it++) {
    auto[scale, probeRoI] = *it;
    BoundingBox* probeBox = probeRoI->probingBox;
    if (isUsable(probeBox, referenceBox)) {
      newScale = scale;
      selectedRoI = probeRoI;
    } else {
      break;
    }
  }

  if (!mConfig.STATIC_SCALE) {
    // When cRoI is merged and all of probing RoIs fail
    // newScale can be larger than getTargetScale(cRoI->getScaleLevel())
    if (selectedRoI == cRoI && isCalibrated(cRoI->id, cRoI->getScaleLevel())) {
      newScale = std::min(newScale, calibrationTable[cRoI->id].second
                                    + cRoI->getTargetScale() * mConfig.PROBE_STEP_SIZE);
    } else {
      newScale = std::min(newScale, getTargetScale(cRoI->getScaleLevel(), cRoI->features.width * cRoI->features.height));
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

std::vector<float> RoIResizer::getProbingCandidates(float scale, int level, int numProbeSteps,
                                                    float originalArea) const {
  assert(0.0f <= scale && scale <= 1.0f);
  std::vector<float> candidates;
  if (mConfig.STATIC_SCALE) {
    for (int i = 0; i < numProbeSteps; i++) {
      if (i == 0) {
        candidates.push_back(scale);
      } else {
        candidates.push_back(scale * (1 - float(i) * mConfig.PROBE_STEP_SIZE));
        candidates.push_back(scale * (1 + float(i) * mConfig.PROBE_STEP_SIZE));
      }
    }
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [](float candidate) {
                                      return candidate <= 1e-5 || 1.0f < candidate;
                                    }), candidates.end());
  } else {
    for (int i = 0; i < numProbeSteps; i++) {
      candidates.push_back(scale * (1 - float(i) * mConfig.PROBE_STEP_SIZE));
    }
    float lowerBound = level == 0 ? float(1e-5) : getTargetScale(level - 1, originalArea);
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

float RoIResizer::calculateTargetScale(float targetArea, float originalArea) const {
  if (mConfig.STATIC_SCALE) {
    return std::min(1.0f, sqrt(targetArea / originalArea));
  } else {
    float scale = sqrt((targetArea + mConfig.AREA_SHIFT) / originalArea) + mConfig.SCALE_SHIFT;
    return std::min(1.0f, scale);
  }
}

RoIResizer::CircularBuffer::CircularBuffer(int numLevels, int capacity)
    : capacity_(capacity), oldest_index(0), size_(0), numLevels(numLevels) {
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
