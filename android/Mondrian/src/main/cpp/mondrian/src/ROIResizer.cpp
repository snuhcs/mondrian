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
    : config_(config) {
  predictorTable_[Device::GPU] = CANDIDATE_PREDICTORS.at(config.DATASET);
  targetAreasTable_[Device::GPU] = AREA_LEVELS.at(config.DATASET);
}

std::pair<float, int> ROIResizer::getTargetScale(const OID oid,
                                                 const Features& features,
                                                 const float area) {
  assert(features.type == ROIType::OF && "PD ROI should not be resized");
  assert(features.width * features.height == area); // XXX

  const int targetLevel = getMaxVotedLevel(oid, features);

  float targetScale = calcTargetScale(targetLevel, area);;
  if (isCalibrated(oid)) {
    if (calibrationTableTable_[Device::GPU].at(oid).first == targetLevel) {
      // hit : use the calibrated scale
      targetScale = calibrationTableTable_[Device::GPU].at(oid).second;
    } else {
      // miss : invalidate the calibration
      calibrationTableTable_[Device::GPU].erase(oid);
    }
  }

  return {targetScale, targetLevel};
}

float ROIResizer::calcTargetScale(const int scaleLevel, const float originalArea) const {
  float targetArea = config_.STATIC_AREA ? config_.STATIC_TARGET_AREA : targetAreasTable_.at(Device::GPU).at(scaleLevel);
  targetArea += config_.AREA_SHIFT;
  float targetScale = std::sqrt(targetArea / originalArea);
  targetScale = std::min(1.0f, targetScale + config_.SCALE_SHIFT);
  return targetScale;
}

bool ROIResizer::isCalibrated(const OID oid) const {
  return calibrationTableTable_.find(Device::GPU) != calibrationTableTable_.end() &&
      calibrationTableTable_.at(Device::GPU).find(oid) != calibrationTableTable_.at(Device::GPU).end();
}

int ROIResizer::getMaxVotedLevel(const OID oid, const Features& features) {
  auto record = prevPredictionBufferTable_[Device::GPU].find(oid);
  if (record == prevPredictionBufferTable_[Device::GPU].end()) {
    prevPredictionBufferTable_[Device::GPU][oid] = CircularBuffer(int(targetAreasTable_[Device::GPU].size()),
                                               config_.VOTING_WINDOW_SIZE);
  }
  prevPredictionBufferTable_[Device::GPU][oid].push(predictLevelWithFeatures(features));
  int maxVotedLevel = prevPredictionBufferTable_[Device::GPU][oid].maxVote();
  assert(0 <= maxVotedLevel && maxVotedLevel < targetAreasTable_[Device::GPU].size() && "Index out of range");
  return maxVotedLevel;
}

int ROIResizer::predictLevelWithFeatures(const Features& features) const {
  if (config_.STATIC_AREA) {
    return STATIC_LEVEL;
  }
  auto& [shiftAvgX, shiftAvgY] = features.ofFeatures.shiftAvg;
  auto& [shiftStdX, shiftStdY] = features.ofFeatures.shiftStd;
  float shiftAvg = shiftAvgX * shiftAvgX + shiftAvgY * shiftAvgY;
  float shiftStd = shiftStdX * shiftStdX + shiftStdY * shiftStdY;
  return predictorTable_.at(Device::GPU)(
      std::max(features.width, features.height),
      features.width * features.height,
      features.xyRatio,
      shiftAvg, shiftStd,
      features.ofFeatures.shiftNcc,
      features.ofFeatures.avgErr);
}

void ROIResizer::updateTable(ROI* roi) {
  if (roi->roisForProbingTable[Device::GPU].empty()) {
    return;
  }

  std::map<float, MergedROI*> probingROIs;
  for (MergedROI* probeROI : roi->roisForProbingTable[Device::GPU]) {
    probingROIs[probeROI->targetScale()] = probeROI;
  }

  // find the smallest target size with a usable box
  float newScale = calcTargetScale(roi->scaleLevel(), roi->features.width * roi->features.height);
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

  if (roi->box() == nullptr && largestProbeROIBox != nullptr) {
    auto copiedBox = std::make_unique<BoundingBox>(
        roi->oid, largestProbeROIBox->pid, largestProbeROIBox->loc, largestProbeROIBox->confidence,
        largestProbeROIBox->label, roi->origin());
    copiedBox->setSrcROI(roi);
    roi->setLabel(copiedBox->label);
    roi->setBox(copiedBox.get());
    roi->frame->boxes.push_back(std::move(copiedBox));
  }
  calibrationTableTable_[Device::GPU][roi->oid] = {roi->scaleLevel(), newScale};
}

std::vector<float> ROIResizer::getProbingCandidates(float scale, int level, float area) const {
  assert(0.0f <= scale && scale <= 1.0f);
  std::vector<float> candidates;
  for (int i = 0; i < config_.NUM_PROBE_STEPS; i++) {
    if (config_.ONLY_SMALLER_PROBING) {
      candidates.push_back(scale * (1 - (float) i * config_.PROBE_STEP_SIZE));
    } else {
      candidates.push_back(scale * (1 - (float) i * config_.PROBE_STEP_SIZE));
      candidates.push_back(scale * (1 + (float) i * config_.PROBE_STEP_SIZE));
    }
  }
  float lowerBound = level == 0 ? 1e-5f : calcTargetScale(level - 1, area);
  float upperBound = 1.0f;
  candidates.erase(std::remove_if(
      candidates.begin(), candidates.end(),
      [lowerBound, upperBound](float candidate) {
        return candidate <= lowerBound || upperBound <= candidate;
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
