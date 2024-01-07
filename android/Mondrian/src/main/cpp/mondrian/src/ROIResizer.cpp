#include "mondrian/ROIResizer.hpp"

#include <set>
#include <cmath>

#include "mondrian/DataType.hpp"
#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"

namespace md {

const int ROIResizer::STATIC_LEVEL = 0;

const int ROIResizer::INVALID_LEVEL = -1;

const std::map<std::pair<Device, std::string>,
               std::pair<Predictor, std::vector<float>>> ROIResizer::PREDICTORS = {
    {{Device::GPU, "virat"}, {VIRAT_FP16, {
        333.913147627257,
        638.5106976744186,
        764.6907839189453,
        880.0315355329949,
        1e10
    }}},
    {{Device::GPU, "mta"}, {MTA_FP16, {
        368.62745098039215,
        459.33734939759046,
        558.7062937062936,
        848.4255998838352,
        1e10
    }}},
    // TODO: Update dummy values to real values
    {{Device::DSP, "virat"}, {VIRAT_INT8, {
        333.913147627257,
        638.5106976744186,
        764.6907839189453,
        880.0315355329949,
        1e10
    }}},
    {{Device::DSP, "mta"}, {MTA_INT8, {
        368.62745098039215,
        459.33734939759046,
        558.7062937062936,
        848.4255998838352,
        1e10
    }}},
};

ROIResizer::ROIResizer(const ROIResizerConfig& config) : config_(config) {}

std::map<Device, std::pair<float, int>> ROIResizer::getTargetScale(const OID oid,
                                                                   const Features& features,
                                                                   const float area) {
  assert(features.type == ROIType::OF && "PD ROI should not be resized");
  assert(features.width * features.height == area); // TODO: remove redundant argument

  std::map<Device, std::pair<float, int>> scaleLevelTable;
  for (Device device : DEVICES) {
    const int targetLevel = getMaxVotedLevel(oid, features, device);
    float targetScale = calcTargetScale(targetLevel, area, device);
    if (isCalibrated(oid, device)) {
      if (calibrationTableTable_[device].at(oid).first == targetLevel) {
        // hit : use the calibrated scale
        targetScale = calibrationTableTable_[device].at(oid).second;
      } else {
        // miss : invalidate the calibration
        calibrationTableTable_[device].erase(oid);
      }
    }
    scaleLevelTable[device] = {targetScale, targetLevel};
  }

  return scaleLevelTable;
}

float ROIResizer::calcTargetScale(const int scaleLevel,
                                  const float originalArea,
                                  Device device) const {
  float targetArea = config_.STATIC_AREA ? config_.STATIC_TARGET_AREA
                                         : areaLevelsOf(device).at(scaleLevel);
  targetArea += config_.AREA_SHIFT;
  float targetScale = std::sqrt(targetArea / originalArea);
  targetScale = std::min(1.0f, targetScale + config_.SCALE_SHIFT);
  return targetScale;
}

bool ROIResizer::isCalibrated(const OID oid, Device device) const {
  return calibrationTableTable_.find(device) != calibrationTableTable_.end() &&
      calibrationTableTable_.at(device).find(oid) != calibrationTableTable_.at(device).end();
}

int ROIResizer::getMaxVotedLevel(const OID oid, const Features& features, Device device) {
  // TODO: not sure if this is appropriate.
  if (prevPredictionBufferTable_.find(device) == prevPredictionBufferTable_.end()) {
    prevPredictionBufferTable_[device] = std::map<OID, CircularBuffer>();
  }

  int numLevels = (int) areaLevelsOf(device).size();
  auto record = prevPredictionBufferTable_[device].find(oid);
  if (record == prevPredictionBufferTable_[device].end()) {
    prevPredictionBufferTable_[device][oid] = CircularBuffer(numLevels, config_.VOTING_WINDOW_SIZE);
  }
  prevPredictionBufferTable_[device][oid].push(predictLevelWithFeatures(features, device));
  int maxVotedLevel = prevPredictionBufferTable_[device][oid].maxVote();
  assert(0 <= maxVotedLevel && maxVotedLevel < numLevels && "Index out of range");
  return maxVotedLevel;
}

int ROIResizer::predictLevelWithFeatures(const Features& features, Device device) const {
  if (config_.STATIC_AREA) return STATIC_LEVEL;
  auto& [shiftAvgX, shiftAvgY] = features.ofFeatures.shiftAvg;
  auto& [shiftStdX, shiftStdY] = features.ofFeatures.shiftStd;
  float shiftAvg = shiftAvgX * shiftAvgX + shiftAvgY * shiftAvgY;
  float shiftStd = shiftStdX * shiftStdX + shiftStdY * shiftStdY;
  return predictorOf(device)(
      std::max(features.width, features.height),
      features.width * features.height,
      features.xyRatio,
      shiftAvg, shiftStd,
      features.ofFeatures.shiftNcc,
      features.ofFeatures.avgErr);
}

void ROIResizer::updateTable(ROI* roi, Device device) {
  if (roi->roisForProbingTable.find(device) == roi->roisForProbingTable.end()) return;
  if (roi->roisForProbingTable[device].empty()) return;

  std::map<float, MergedROI*> probingROIs;
  for (MergedROI* probeROI : roi->roisForProbingTable[device]) {
    probingROIs[probeROI->targetScale()] = probeROI;
  }

  // find the smallest target size with a usable box
  float newScale = calcTargetScale(roi->scaleLevel(),
                                   roi->features.width * roi->features.height,
                                   device);
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
        largestProbeROIBox->label, largestProbeROIBox->origin);
    copiedBox->setSrcROI(roi);
    roi->setLabel(copiedBox->label);
    roi->setBox(copiedBox.get());
    roi->frame->boxes.push_back(std::move(copiedBox));
  }
  calibrationTableTable_[device][roi->oid] = {roi->scaleLevel(), newScale};
}

std::map<Device, std::vector<float>> ROIResizer::getProbingCandidatesTable(
    std::map<Device, float> scaleTable, int level, float area) const {

  for (const auto& [_, scale] : scaleTable) {
    assert(0.0f <= scale && scale <= 1.0f);
  }

  std::map<Device, std::vector<float>> candidatesTable;
  for (auto& [device, candidates] : candidatesTable) {
    for (int i = 0; i < config_.NUM_PROBE_STEPS; i++) {
      if (config_.ONLY_SMALLER_PROBING) {
        candidates.push_back(scaleTable[device] * (1 - (float) i * config_.PROBE_STEP_SIZE));
      } else {
        candidates.push_back(scaleTable[device] * (1 - (float) i * config_.PROBE_STEP_SIZE));
        candidates.push_back(scaleTable[device] * (1 + (float) i * config_.PROBE_STEP_SIZE));
      }
    }
    float lowerBound = level == 0 ? 1e-5f : calcTargetScale(level - 1, area, Device::INVALID);
    float upperBound = 1.0f;
    candidates.erase(std::remove_if(
        candidates.begin(), candidates.end(),
        [lowerBound, upperBound](float candidate) {
          return candidate <= lowerBound || upperBound <= candidate;
        }), candidates.end());
  }
  return candidatesTable;
}

bool ROIResizer::isUsable(BoundingBox* box, BoundingBox* referenceBox) const {
  float intersection = (box->loc & referenceBox->loc).area();
  float iou = intersection / (box->loc.area() + referenceBox->loc.area() - intersection);
  return box != nullptr && referenceBox != nullptr
      && box->label == referenceBox->label
      && iou > config_.PROBE_IOU_THRES
      && box->confidence > config_.PROBE_CONF_THRES;
}

std::vector<float> ROIResizer::areaLevelsOf(Device device) const {
  return PREDICTORS.at({device, config_.DATASET}).second;
}

Predictor ROIResizer::predictorOf(Device device) const {
  return PREDICTORS.at({device, config_.DATASET}).first;
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
