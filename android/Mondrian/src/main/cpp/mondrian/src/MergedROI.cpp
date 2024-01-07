#include "mondrian/MergedROI.hpp"

#include <numeric>

#include "mondrian/Frame.hpp"
#include "mondrian/Utils.hpp"

namespace md {

MergedROI::MergedROI(ROI* roi, bool isProbing)
    : rois_({roi}),
      isProbing_(isProbing),
      frame_(roi->frame),
      loc_(roi->paddedLoc),
      targetScaleTable_(roi->targetScaleTable()),
      packedXY_(INVALID_XY),
      packedCanvasIndex_(-1),
      pid_(-1),
      packedCanvasSize_(-1),
      isInferenced_(false),
      dispatchTargetDevice_(Device::INVALID) {
  roi->mergedROI = this;
}

static std::vector<ROI*> roisOf(const std::vector<MergedROI*>& mergedROIs) {
  std::vector<ROI*> rois;
  for (const auto& mergedROI : mergedROIs) {
    rois.insert(rois.end(), mergedROI->rois().begin(), mergedROI->rois().end());
  }
  return rois;
}

static cv::Rect2f locOf(const std::vector<MergedROI*>& mergedROIs) {
  cv::Rect2f loc = mergedROIs[0]->loc();
  for (int i = 1; i < mergedROIs.size(); i++) {
    loc |= mergedROIs[i]->loc();
  }
  return loc;
}

static std::map<Device, float> targetScaleTableOf(const std::vector<MergedROI*>& mergedROIs) {
  std::map<Device, float> targetScaleTable;
  for (const auto& mergedROI : mergedROIs) {
    for (const auto& [device, scale] : mergedROI->targetScaleTable()) {
      targetScaleTable[device] = std::max(targetScaleTable[device], scale);
    }
  }
  return targetScaleTable;
}

MergedROI::MergedROI(const std::vector<MergedROI*>& mergedROIs)
    : rois_(roisOf(mergedROIs)),
      isProbing_(mergedROIs[0]->isProbing()),
      frame_(mergedROIs[0]->frame()),
      loc_(locOf(mergedROIs)),
      targetScaleTable_(targetScaleTableOf(mergedROIs)),
      packedXY_(INVALID_XY),
      packedCanvasIndex_(-1),
      pid_(-1),
      packedCanvasSize_(-1),
      isInferenced_(false),
      dispatchTargetDevice_(Device::INVALID) {
  for (const auto& roi : rois_) {
    roi->mergedROI = this;
  }
  for (const auto& mergedROI : mergedROIs) {
    assert(mergedROI->frame() == frame_);
    assert(mergedROI->isProbing() == isProbing_);
  }
}

void MergedROI::mergeROIs(std::vector<std::unique_ptr<MergedROI>>& mergedROIs, int maxSize) {
  assert(!mergedROIs.empty());
  Frame* frame = mergedROIs[0]->frame();
  for (const auto& mergedROI : mergedROIs) {
    assert(mergedROI->frame() == frame);
    for (const auto& roi : mergedROI->rois()) {
      assert(roi->frame == mergedROI->frame());
    }
  }

  auto isMergeBenefit = [maxSize](MergedROI* mi, MergedROI* mj) -> bool {
    MergedROI merged(std::vector<MergedROI*>{mi, mj});
    return std::all_of(
        merged.targetScaleTable_.begin(),
        merged.targetScaleTable_.end(),
        [&merged, &mi, &mj, maxSize](const auto& deviceAndScale) -> bool {
          const auto& [device, scale] = deviceAndScale;
          int bw = borderedLengthOf(merged.loc_.width, scale);
          int bh = borderedLengthOf(merged.loc_.height, scale);
          int newArea = merged.borderedArea(device);
          int origArea = mi->borderedArea(device) + mj->borderedArea(device);
          return std::max(bw, bh) <= maxSize && newArea <= origArea;
        });
  };

  while (true) {
    std::vector<int> groupIDs(mergedROIs.size());
    std::iota(std::begin(groupIDs), std::end(groupIDs), 0);
    for (int i = 0; i < mergedROIs.size(); i++) {
      for (int j = i + 1; j < mergedROIs.size(); j++) {
        const auto& mi = mergedROIs[i].get();
        const auto& mj = mergedROIs[j].get();
        if (isMergeBenefit(mi, mj)) {
          groupIDs[j] = groupIDs[i];
        }
      }
    }

    std::map<int, std::vector<int>> idGroups;
    for (int i = 0; i < groupIDs.size(); i++) {
      idGroups[groupIDs[i]].push_back(i);
    }
    if (idGroups.size() == mergedROIs.size()) break;

    std::vector<int> mergedROIIndicesToRemove;
    std::vector<std::unique_ptr<MergedROI>> mergedROIsToAdd;
    for (const auto& [_, idGroup] : idGroups) {
      if (idGroup.size() == 1) continue;
      std::vector<MergedROI*> group;
      for (int i : idGroup) {
        group.push_back(mergedROIs[i].get());
        mergedROIIndicesToRemove.push_back(i);
      }
      mergedROIsToAdd.emplace_back(new MergedROI(group));
    }
    std::sort(mergedROIIndicesToRemove.begin(), mergedROIIndicesToRemove.end(), std::greater<>());
    for (int i : mergedROIIndicesToRemove) {
      mergedROIs.erase(mergedROIs.begin() + i);
    }
    mergedROIs.insert(mergedROIs.end(),
                      std::make_move_iterator(mergedROIsToAdd.begin()),
                      std::make_move_iterator(mergedROIsToAdd.end()));
  }

  for (auto& merged : mergedROIs) {
    for (auto& roi : merged->rois()) {
      roi->mergedROI = merged.get();
    }
  }
}

cv::Mat MergedROI::mat() const {
  int l = std::max(0, std::min(frame_->width(), (int) loc_.x));
  int t = std::max(0, std::min(frame_->height(), (int) loc_.y));
  int r = std::max(0, std::min(frame_->width(), (int) (loc_.x + loc_.width)));
  int b = std::max(0, std::min(frame_->height(), (int) (loc_.y + loc_.height)));
  return extractRgbROIFromYuvMat(frame_->yuvMat, l, t, r, b);
}

cv::Mat MergedROI::resizedMat(Device device) const {
  int rw = resizedLengthOf(loc_.width, targetScaleTable_.at(device));
  int rh = resizedLengthOf(loc_.height, targetScaleTable_.at(device));
  cv::Mat rgbMat;
  cv::resize(mat(), rgbMat, cv::Size(rw, rh));
  return rgbMat;
}

cv::Mat MergedROI::borderedMat(Device device) const {
  cv::Mat rgbMat = resizedMat(device);
  cv::copyMakeBorder(rgbMat, rgbMat,
                     BORDER, BORDER, BORDER, BORDER,
                     cv::BORDER_CONSTANT, BORDER_COLOR);
  int bw = borderedLengthOf(loc_.width, targetScaleTable_.at(device));
  int bh = borderedLengthOf(loc_.height, targetScaleTable_.at(device));
  assert(bw == rgbMat.cols && bh == rgbMat.rows);
  return rgbMat;
}

void MergedROI::setPackInfo(Device device,
                            cv::Point2i xy,
                            int packedCanvasIndex,
                            ExecutionType executionType,
                            int roiSize) {
  if (executionType == ExecutionType::EMULATED_BATCH
      || executionType == ExecutionType::ROI_WISE_INFERENCE) {
    int bw = borderedLengthOf(loc_.width, targetScaleTable_.at(Device::GPU));
    int bh = borderedLengthOf(loc_.height, targetScaleTable_.at(Device::GPU));
    if (roiSize < std::max(bw, bh)) {
      LOGE("ROISize %d < bw %d or bh %d", roiSize, bw, bh);
      assert(false);
    }
    xy.x += (roiSize - bw) / 2;
    xy.y += (roiSize - bh) / 2;
  }
  dispatchTargetDevice_ = device;
  packedXY_ = xy;
  packedCanvasIndex_ = packedCanvasIndex;
}

cv::Rect2i MergedROI::packedLoc() const {
  auto [rw, rh] = resizedMatWH();
  return {packedXY_.x + MergedROI::BORDER,
          packedXY_.y + MergedROI::BORDER,
          rw,
          rh};
}

cv::Rect2f MergedROI::reconstructBoxPos(const BoundingBox& packedBox) const {
  cv::Rect2f reconBox = packedBox.loc;
  reconBox -= static_cast<cv::Point2f>(packedXY_);
  reconBox -= cv::Point2f((float) MergedROI::BORDER, (float) MergedROI::BORDER);
  reconBox *= targetScale();
  reconBox += loc_.tl();
  reconBox &= frame_->rectf();
  assert(reconBox.width >= 0 && reconBox.height >= 0);
  return reconBox;
}

std::string MergedROI::header() {
  std::stringstream ss;
  ss << "mergedROIs" << DELIM
     << "mergedLoc_l" << DELIM
     << "mergedLoc_t" << DELIM
     << "mergedLoc_r" << DELIM
     << "mergedLoc_b" << DELIM
     << "mergedScale[GPU]" << DELIM
     << "mergedScale[DSP]" << DELIM
     << "pid" << DELIM
     << "packedXY_x" << DELIM
     << "packedXY_y" << DELIM
     << "packedCanvasIndex" << DELIM
     << "packedCanvasSize" << DELIM
     << "dispatchTargetDevice" << DELIM
     << "isProbing" << DELIM
     << "probingBoxID[GPU]" << DELIM
     << "probingBoxID[DSP]";
  return ss.str();
}

std::string MergedROI::str() const {
  std::stringstream ss;
  for (int i = 0; i < rois_.size(); i++) {
    ss << rois_[i]->rid;
    if (i < rois_.size() - 1) {
      ss << SUBDELIM;
    }
  }
  ss << DELIM;
  ss << loc_.x << DELIM
     << loc_.y << DELIM
     << loc_.x + loc_.width << DELIM
     << loc_.y + loc_.height << DELIM
     << (targetScaleTable_.find(Device::GPU) != targetScaleTable_.end()
         ? targetScaleTable_.at(Device::GPU)
         : -1) << DELIM
     << (targetScaleTable_.find(Device::DSP) != targetScaleTable_.end()
         ? targetScaleTable_.at(Device::DSP)
         : -1) << DELIM
     << pid_ << DELIM
     << packedXY_.x << DELIM
     << packedXY_.y << DELIM
     << packedCanvasIndex_ << DELIM
     << packedCanvasSize_ << DELIM
     << ::md::str(dispatchTargetDevice_) << DELIM
     << isProbing_ << DELIM
     << (probingBoxIDTable_.find(Device::GPU) != probingBoxIDTable_.end()
         ? probingBoxIDTable_.at(Device::GPU)
         : -1) << DELIM
     << (probingBoxIDTable_.find(Device::DSP) != probingBoxIDTable_.end()
         ? probingBoxIDTable_.at(Device::DSP)
         : -1);
  return ss.str();
}

void MergedROI::setProbingBox(BoundingBox* box) {
  probingBoxTable_[Device::GPU] = box;
  probingBoxIDTable_[Device::GPU] = box != nullptr ? box->bid : -1;
}

} // namespace md
