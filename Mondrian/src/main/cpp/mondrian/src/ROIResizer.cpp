#include "mondrian/ROIResizer.hpp"

#include <set>
#include <cmath>

#include "mondrian/DataType.hpp"
#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"

namespace md {

const int ROIResizer::STATIC_LEVEL = 0;

const int ROIResizer::INVALID_LEVEL = -1;

const std::map<std::string, Predictor> ROIResizer::CANDIDATE_PREDICTORS = {
    {"virat", VIRAT},
    {"mta",   MTA},
};

const std::map<std::string, std::vector<float>> ROIResizer::SCALE_LEVELS = {
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
    : config_(config),
      predictor_(CANDIDATE_PREDICTORS.at(config.TRAIN_DATA)),
      targetAreas_(config.STATIC_SCALE
                    ? toVec(config.STATIC_TARGET_SCALE)
                    : SCALE_LEVELS.at(config.TRAIN_DATA)) {}

std::pair<float, int> ROIResizer::getTargetScale(const ID id,
                                                 const Features& features,
                                                 const float maxEdgeLength) {
  auto[targetScale, targetLevel] = getTargetScale(id, features);
  float originalArea = features.width * features.height;
  if (maxEdgeLength * targetScale > config_.MAX_OF_ROI_SIZE) {
    for (int level = int(targetAreas_.size()) - 1; level >= 0; level--) {
      float scale = getTargetScale(level, originalArea);
      if (maxEdgeLength * scale <= config_.MAX_OF_ROI_SIZE) {
        targetScale = scale;
        break;
      }
    }
    while (maxEdgeLength * targetScale > config_.MAX_OF_ROI_SIZE) {
      targetScale -= config_.PROBE_STEP_SIZE;
    }
    assert(targetScale > 0.0f);
  }
  return {targetScale, targetLevel};
}

std::pair<float, int> ROIResizer::getTargetScale(const ID id,
                                                 const Features& features) {
  assert(features.type == OF);
  const int targetLevel = getMaxVotedLevel(id, features);
  assert(0 <= targetLevel && targetLevel < targetAreas_.size());
  float targetScale = getTargetScale(targetLevel, features.width * features.height);

  if (isCalibrated(id, targetLevel)) {
    targetScale = calibrationTable_[id].second;
  } else if (calibrationTable_.find(id) != calibrationTable_.end()) {
    calibrationTable_.erase(id);
  }
  return {targetScale, targetLevel};
}

float ROIResizer::getTargetScale(const int scaleLevel, const float originalArea) const {
  return std::min(1.0f, calculateTargetScale(targetAreas_.at(scaleLevel), originalArea));
}

bool ROIResizer::isCalibrated(const ID id, const int scaleLevel) const {
  return calibrationTable_.find(id) != calibrationTable_.end()
         && calibrationTable_.at(id).first == scaleLevel;
}

int ROIResizer::getMaxVotedLevel(const ID id, const Features& features) {
  auto record = prevPredictionBuffer_.find(id);
  if (record == prevPredictionBuffer_.end()) {
    prevPredictionBuffer_[id] = CircularBuffer(int(targetAreas_.size()),
                                               config_.VOTING_WINDOW);
  }
  prevPredictionBuffer_[id].push(predictLevelWithFeatures(features));
  return prevPredictionBuffer_[id].maxVote();
}

int ROIResizer::predictLevelWithFeatures(const Features& features) const {
  if (config_.STATIC_SCALE) {
    return STATIC_LEVEL;
  }
  assert(features.type == OF);
  auto&[shiftAvgX, shiftAvgY] = features.ofFeatures.shiftAvg;
  auto&[shiftStdX, shiftStdY] = features.ofFeatures.shiftStd;
  float shiftAvg = shiftAvgX * shiftAvgX + shiftAvgY * shiftAvgY;
  float shiftStd = shiftStdX * shiftStdX + shiftStdY * shiftStdY;
  return predictor_(
      std::max(features.width, features.height),
      features.width * features.height,
      features.xyRatio,
      shiftAvg, shiftStd,
      features.ofFeatures.shiftNcc,
      features.ofFeatures.avgErr,
      features.confidence);
}

void ROIResizer::updateTable(ROI* roi) {
  if (roi->roisForProbing.empty()) {
    return;
  }

  std::map<float, MergedROI*> probingROIs;
  for (MergedROI* probeROI: roi->roisForProbing) {
    probingROIs[probeROI->targetScale()] = probeROI;
  }

  // find the smallest target size with a usable box
  float newScale = getTargetScale(roi->scaleLevel(), roi->features.width * roi->features.height);
  BoundingBox* largestProbeROIBox = probingROIs.rbegin()->second->probingBox();
  if (largestProbeROIBox != nullptr) {
    for (auto it = probingROIs.rbegin(); it != probingROIs.rend(); it++) {
      auto[scale, probeROI] = *it;
      BoundingBox* probeROIBox = probeROI->probingBox();
      if (isUsable(probeROIBox, largestProbeROIBox)) {
        newScale = scale;
      } else {
        break;
      }
    }
  }

  if (roi->box == nullptr && largestProbeROIBox != nullptr) {
    auto copiedBox = std::make_unique<BoundingBox>(
        roi->id, largestProbeROIBox->loc, largestProbeROIBox->confidence,
        largestProbeROIBox->label, roi->origin);
    copiedBox->srcROI = roi;
    roi->label = copiedBox->label;
    roi->box = copiedBox.get();
    roi->frame->boxes.push_back(std::move(copiedBox));
  }
  calibrationTable_[roi->id] = {roi->scaleLevel(), newScale};
}

void ROIResizer::getProbingCandidates(ROI* roi) const {
  float scale = roi->targetScale();
  int level = roi->scaleLevel();
  int numProbeSteps = getNumProbeSteps();
  float originalArea = roi->features.width * roi->features.height;
  assert(0.0f <= scale && scale <= 1.0f);
  std::vector<float>& candidates = roi->probeScales;
  if (config_.STATIC_SCALE) {
    for (int i = 0; i < numProbeSteps; i++) {
      if (i == 0) {
        candidates.push_back(scale);
      } else {
        candidates.push_back(scale * (1 - float(i) * config_.PROBE_STEP_SIZE));
        candidates.push_back(scale * (1 + float(i) * config_.PROBE_STEP_SIZE));
      }
    }
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [](float candidate) {
                                      return candidate <= 1e-5 || 1.0f < candidate;
                                    }), candidates.end());
  } else {
    for (int i = 0; i < numProbeSteps; i++) {
      candidates.push_back(scale * (1 - float(i) * config_.PROBE_STEP_SIZE));
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
         && box->loc.iou(referenceBox->loc) > config_.PROBE_IOU_THRESHOLD
         && box->confidence > config_.PROBE_CONF_THRESHOLD;
}

float ROIResizer::calculateTargetScale(float targetArea, float originalArea) const {
  if (config_.STATIC_SCALE) {
    return std::min(1.0f, sqrt(targetArea / originalArea));
  } else {
    float scale = sqrt((targetArea + config_.AREA_SHIFT) / originalArea) + config_.SCALE_SHIFT;
    return std::min(1.0f, scale);
  }
}

ROIResizer::CircularBuffer::CircularBuffer(int numLevels, int capacity)
    : capacity_(capacity), oldest_index_(0), size_(0), numLevels_(numLevels) {
  // NOTE that capacity should be even number to avoid tie
  data_.resize(capacity_);
}

void ROIResizer::CircularBuffer::push(int data) {
  data_[oldest_index_] = data;
  oldest_index_ = (oldest_index_ + 1) % capacity_;
  if (size_ < capacity_) {
    ++size_;
  }
}

int ROIResizer::CircularBuffer::maxVote() {
  std::vector<size_t> count(numLevels_, 0);
  for (int i = 0; i < size_; i++) {
    ++count[data_[i]];
  }
  return int(std::max_element(count.begin(), count.end()) - count.begin());
}

} // namespace md
