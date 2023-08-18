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
    {"mta", MTA},
};

const std::map<std::string, std::vector<float>> ROIResizer::AREA_LEVELS = {
    {"virat", {
        333.913147627257,
        638.5106976744186,
        764.6907839189453,
        880.0315355329949,
        1e10
    }},
    {"mta", {
        368.62745098039215,
        459.33734939759046,
        558.7062937062936,
        848.4255998838352,
        1e10
    }}
};

ROIResizer::ROIResizer(const ROIResizerConfig& config)
    : config_(config),
      predictor_(CANDIDATE_PREDICTORS.at(config.DATASET)),
      targetAreas_(AREA_LEVELS.at(config.DATASET)) {}

std::pair<float, int> ROIResizer::getTargetScale(const ID id,
                                                 const Features& features,
                                                 const float area) {
  assert(features.type == OF);
  const int targetLevel = getMaxVotedLevel(id, features);
  assert(0 <= targetLevel && targetLevel < targetAreas_.size());
  assert(features.width * features.height == area);
  float targetScale = getTargetScale(targetLevel, area);

  if (isCalibrated(id, targetLevel)) {
    targetScale = calibrationTable_[id].second;
  } else if (calibrationTable_.find(id) != calibrationTable_.end()) {
    calibrationTable_.erase(id);
  }
  return {targetScale, targetLevel};
}

float ROIResizer::getTargetScale(const int scaleLevel, const float originalArea) const {
  float targetArea = config_.STATIC_AREA ? config_.STATIC_TARGET_AREA : targetAreas_.at(scaleLevel);
  targetArea += config_.AREA_SHIFT;
  float targetScale = std::sqrt(targetArea / originalArea);
  targetScale += config_.SCALE_SHIFT;
  return std::min(1.0f, targetScale);
}

bool ROIResizer::isCalibrated(const ID id, const int scaleLevel) const {
  return calibrationTable_.find(id) != calibrationTable_.end()
      && calibrationTable_.at(id).first == scaleLevel;
}

int ROIResizer::getMaxVotedLevel(const ID id, const Features& features) {
  auto record = prevPredictionBuffer_.find(id);
  if (record == prevPredictionBuffer_.end()) {
    prevPredictionBuffer_[id] = CircularBuffer(int(targetAreas_.size()),
                                               config_.VOTING_WINDOW_SIZE);
  }
  prevPredictionBuffer_[id].push(predictLevelWithFeatures(features));
  return prevPredictionBuffer_[id].maxVote();
}

int ROIResizer::predictLevelWithFeatures(const Features& features) const {
  if (config_.STATIC_AREA) {
    return STATIC_LEVEL;
  }
  assert(features.type == OF);
  auto& [shiftAvgX, shiftAvgY] = features.ofFeatures.shiftAvg;
  auto& [shiftStdX, shiftStdY] = features.ofFeatures.shiftStd;
  float shiftAvg = shiftAvgX * shiftAvgX + shiftAvgY * shiftAvgY;
  float shiftStd = shiftStdX * shiftStdX + shiftStdY * shiftStdY;
  return predictor_(
      std::max(features.width, features.height),
      features.width * features.height,
      features.xyRatio,
      shiftAvg, shiftStd,
      features.ofFeatures.shiftNcc,
      features.ofFeatures.avgErr);
}

void ROIResizer::updateTable(ROI* roi) {
  if (roi->roisForProbing.empty()) {
    return;
  }

  std::map<float, MergedROI*> probingROIs;
  for (MergedROI* probeROI : roi->roisForProbing) {
    probingROIs[probeROI->targetScale()] = probeROI;
  }

  // find the smallest target size with a usable box
  float newScale = getTargetScale(roi->scaleLevel(), roi->features.width * roi->features.height);
  BoundingBox* largestProbeROIBox = probingROIs.rbegin()->second->probingBox();
  if (largestProbeROIBox != nullptr) {
    for (auto it = probingROIs.rbegin(); it != probingROIs.rend(); it++) {
      auto [scale, probeROI] = *it;
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

std::vector<float> ROIResizer::getProbingCandidates(float scale, int level, float area) const {
  assert(0.0f <= scale && scale <= 1.0f);
  std::vector<float> candidates;
  for (int i = 0; i < config_.NUM_PROBE_STEPS; i++) {
    candidates.push_back(scale * (1 - (float) i * config_.PROBE_STEP_SIZE));
  }
  float lowerBound = level == 0 ? 1e-5f : getTargetScale(level - 1, area);
  candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                  [lowerBound](float candidate) {
                                    return candidate <= lowerBound;
                                  }), candidates.end());
  return candidates;
}

bool ROIResizer::isUsable(BoundingBox* box, BoundingBox* referenceBox) const {
  return box != nullptr && referenceBox != nullptr
      && box->label == referenceBox->label
      && box->loc.iou(referenceBox->loc) > config_.PROBE_IOU_THRES
      && box->confidence > config_.PROBE_CONF_THRES;
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
