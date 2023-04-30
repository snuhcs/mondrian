#include "mondrian/ROIResizer.hpp"

#include <set>
#include <cmath>

#include "mondrian/DataType.hpp"
#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"

namespace md {

const int ROIResizer::STATIC_LEVEL = 0;

const int ROIResizer::INVALID_LEVEL = -1;

const std::map<std::string, Predictor> ROIResizer::candidatePredictors = {
    {"virat", VIRAT},
    {"mta",   MTA},
};

const std::map<std::string, std::vector<float>> ROIResizer::scalesForLevels = {
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

ROIResizer::ROIResizer(const ROIResizerConfig& config)
    : mConfig(config),
      mPredictor(candidatePredictors.at(config.TRAIN_DATA)),
      mTargetAreas(config.STATIC_SCALE
                    ? toVec(config.STATIC_TARGET_SCALE)
                    : scalesForLevels.at(config.TRAIN_DATA)) {}

std::pair<float, int> ROIResizer::getTargetScale(const idType id,
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

std::pair<float, int> ROIResizer::getTargetScale(const idType id,
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

float ROIResizer::getTargetScale(const int scaleLevel, const float originalArea) const {
  return std::min(1.0f, calculateTargetScale(mTargetAreas.at(scaleLevel), originalArea));
}

bool ROIResizer::isCalibrated(const idType id, const int scaleLevel) const {
  [](){}();
  return calibrationTable.find(id) != calibrationTable.end()
         && calibrationTable.at(id).first == scaleLevel;
}

int ROIResizer::getMaxVotedLevel(const idType id, const Features& features) {
  auto record = prevPredictionBuffer.find(id);
  if (record == prevPredictionBuffer.end()) {
    prevPredictionBuffer[id] = CircularBuffer(int(mTargetAreas.size()),
                                              mConfig.VOTING_WINDOW);
  }
  prevPredictionBuffer[id].push(predictLevelWithFeatures(features));
  return prevPredictionBuffer[id].maxVote();
}

int ROIResizer::predictLevelWithFeatures(const Features& features) const {
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

void ROIResizer::updateTable(ROI* cROI) {
  if (cROI->roisForProbing.empty()) {
    return;
  }

  std::map<float, ROI*> probingROIs;
  for (ROI* probeROI : cROI->roisForProbing) {
    probingROIs[probeROI->getTargetScale()] = probeROI;
  }

  const auto addBoxToOriginal = [](ROI* cROI, BoundingBox* probeBox) {
    auto copiedBox = std::make_unique<BoundingBox>(
        cROI->id, probeBox->location, probeBox->confidence, probeBox->label, cROI->origin);
    copiedBox->srcROI = cROI;
    cROI->label = copiedBox->label;
    cROI->box = copiedBox.get();
    cROI->frame->boxes.push_back(std::move(copiedBox));
  };

  float originalROIScale = cROI->parentROI->getTargetScale();
  if (probingROIs.find(originalROIScale) != probingROIs.end()) {
    auto box = probingROIs[originalROIScale]->probingBox;
    if (cROI->box == nullptr && box != nullptr) {
      addBoxToOriginal(cROI, box);
    }
  }
  cROI->probingBox = cROI->box;
  probingROIs[originalROIScale] = cROI;

  // find the smallest target size with a usable box
  auto[largestScale, largestROI] = (*probingROIs.rbegin());
  BoundingBox* referenceBox = largestROI->probingBox;
  float newScale = std::min(1.0f, largestScale + mConfig.PROBE_STEP_SIZE);
  ROI* selectedROI = largestROI;
  for (auto it = probingROIs.rbegin(); it != probingROIs.rend(); it++) {
    auto[scale, probeROI] = *it;
    BoundingBox* probeBox = probeROI->probingBox;
    if (isUsable(probeBox, referenceBox)) {
      newScale = scale;
      selectedROI = probeROI;
    } else {
      break;
    }
  }

  if (!mConfig.STATIC_SCALE) {
    // When cROI is merged and all of probing ROIs fail
    // newScale can be larger than getTargetScale(cROI->getScaleLevel())
    if (selectedROI == cROI && isCalibrated(cROI->id, cROI->getScaleLevel())) {
      newScale = std::min(newScale, calibrationTable[cROI->id].second
                                    + cROI->getTargetScale() * mConfig.PROBE_STEP_SIZE);
    } else {
      newScale = std::min(newScale, getTargetScale(cROI->getScaleLevel(), cROI->features.width * cROI->features.height));
    }
  }

  if (cROI->box != referenceBox && isUsable(referenceBox, referenceBox)) {
    if (cROI->box != nullptr) {
      cROI->box->id = cROI->id;
      cROI->box->location = referenceBox->location;
      cROI->box->confidence = referenceBox->confidence;
      assert(cROI->box->origin == Origin::origin_BB
             && cROI->box->origin == Origin::origin_PD
             && cROI->box->origin == Origin::origin_NewMF);
    } else {
      addBoxToOriginal(cROI, referenceBox);
    }
  }
  calibrationTable[cROI->id] = {cROI->getScaleLevel(), newScale};
}

void ROIResizer::getProbingCandidates(ROI* roi) const {
  float scale = roi->getTargetScale();
  int level = roi->getScaleLevel();
  int numProbeSteps = getNumProbeSteps();
  float originalArea = roi->features.width * roi->features.height;
  assert(0.0f <= scale && scale <= 1.0f);
  std::vector<float>& candidates = roi->probeScales;
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
}

bool ROIResizer::isUsable(BoundingBox* box, BoundingBox* referenceBox) const {
  return box != nullptr && referenceBox != nullptr
         && box->label == referenceBox->label
         && box->location.iou(referenceBox->location) > mConfig.PROBE_IOU_THRESHOLD
         && box->confidence > mConfig.PROBE_CONF_THRESHOLD;
}

float ROIResizer::calculateTargetScale(float targetArea, float originalArea) const {
  if (mConfig.STATIC_SCALE) {
    return std::min(1.0f, sqrt(targetArea / originalArea));
  } else {
    float scale = sqrt((targetArea + mConfig.AREA_SHIFT) / originalArea) + mConfig.SCALE_SHIFT;
    return std::min(1.0f, scale);
  }
}

ROIResizer::CircularBuffer::CircularBuffer(int numLevels, int capacity)
    : capacity_(capacity), oldest_index(0), size_(0), numLevels(numLevels) {
  // NOTE that capacity should be even number to avoid tie
  data_.resize(capacity_);
}

void ROIResizer::CircularBuffer::push(int data) {
  data_[oldest_index] = data;
  oldest_index = (oldest_index + 1) % capacity_;
  if (size_ < capacity_) {
    ++size_;
  }
}

int ROIResizer::CircularBuffer::maxVote() {
  std::vector<size_t> count(numLevels, 0);
  for (int i = 0; i < size_; i++) {
    ++count[data_[i]];
  }
  return int(std::max_element(count.begin(), count.end()) - count.begin());
}

} // namespace md
