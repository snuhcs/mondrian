#include "mondrian/MergedROI.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/Utils.hpp"

namespace md {

MergedROI::MergedROI(const std::vector<ROI*>& rois, float targetScale, bool isProbing)
    : rois_(rois), isProbing_(isProbing),
      frame_(frameOf(rois)), loc_(locOf(rois)),
      packedXY_(INVALID_XY), packedCanvasIndex_(-1), pid_(-1),
      packedCanvasSize_(-1), probingBox_(nullptr), probingBoxID_(-1) {

  for (Device device : Devices) {
    targetScaleTable_[device] = targetScale;
  }


  for (ROI* roi : rois) {
    roi->mergedROI = this;
  }
}

Frame* MergedROI::frameOf(const std::vector<ROI*>& rois) {
  assert(!rois.empty());
  Frame* frame = rois[0]->frame;
  for (int i = 1; i < rois.size(); i++) {
    assert(frame == rois[i]->frame);
  }
  return frame;
}

Rect MergedROI::locOf(const std::vector<ROI*>& rois) {
  assert(!rois.empty());
  float newL = rois[0]->paddedLoc.l;
  float newT = rois[0]->paddedLoc.t;
  float newR = rois[0]->paddedLoc.r;
  float newB = rois[0]->paddedLoc.b;
  for (int i = 1; i < rois.size(); i++) {
    newL = std::min(newL, rois[i]->paddedLoc.l);
    newT = std::min(newT, rois[i]->paddedLoc.t);
    newR = std::max(newR, rois[i]->paddedLoc.r);
    newB = std::max(newB, rois[i]->paddedLoc.b);
  }
  assert(0 <= newL && 0 <= newT && newL <= newR && newT <= newB);
  return {newL, newT, newR, newB};
}

std::unique_ptr<MergedROI> MergedROI::merge(const MergedROI* m0, const MergedROI* m1) {
  std::vector<ROI*> newROIs;
  newROIs.insert(newROIs.end(), m0->rois_.begin(), m0->rois_.end());
  newROIs.insert(newROIs.end(), m1->rois_.begin(), m1->rois_.end());
  float newScale = std::max(m0->targetScaleTable_.at(Device::GPU), m1->targetScaleTable_.at(Device::GPU));
  return std::make_unique<MergedROI>(newROIs, newScale, false);
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

  while (true) {
    int i, j;
    bool updated = false;
    std::unique_ptr<MergedROI> merged;
    for (i = 0; i < mergedROIs.size(); i++) {
      for (j = i + 1; j < mergedROIs.size(); j++) {
        const auto& mi = mergedROIs[i].get();
        const auto& mj = mergedROIs[j].get();
        merged = merge(mi, mj);
        int bw = borderedLengthOf(merged->loc_.w, merged->targetScaleTable_.at(Device::GPU));
        int bh = borderedLengthOf(merged->loc_.h, merged->targetScaleTable_.at(Device::GPU));
        if (std::max(bw, bh) > maxSize) {
          continue; // would be little more conservative for the general case
        }

        int newArea = merged->resizedArea();
        int origArea = mi->resizedArea() + mj->resizedArea();
        if (newArea >= origArea) {
          continue;
        }
        updated = true;
        break;
      }
      if (updated) {
        break;
      }
    }
    if (!updated) {
      break;
    }
    assert(j > i);
    mergedROIs.push_back(std::move(merged));
    mergedROIs.erase(mergedROIs.begin() + j);
    mergedROIs.erase(mergedROIs.begin() + i);
  }

  for (auto& merged : mergedROIs) {
    for (auto& roi : merged->rois()) {
      roi->mergedROI = merged.get();
    }
  }
}

cv::Mat MergedROI::mat() const {
  int l = std::max(0, std::min(frame_->width(), int(loc_.l)));
  int t = std::max(0, std::min(frame_->height(), int(loc_.t)));
  int r = std::max(0, std::min(frame_->width(), int(loc_.r)));
  int b = std::max(0, std::min(frame_->height(), int(loc_.b)));
  return extractRgbROIFromYuvMat(frame_->yuvMat, l, t, r, b);
}

cv::Mat MergedROI::resizedMat() const {
  int rw = resizedLengthOf(loc_.w, targetScaleTable_.at(Device::GPU));
  int rh = resizedLengthOf(loc_.h, targetScaleTable_.at(Device::GPU));
  cv::Mat rgbMat;
  cv::resize(mat(), rgbMat, cv::Size(rw, rh));
  return rgbMat;
}

cv::Mat MergedROI::borderedMat() const {
  cv::Mat rgbMat = resizedMat();
  cv::copyMakeBorder(rgbMat, rgbMat,
                     BORDER, BORDER, BORDER, BORDER,
                     cv::BORDER_CONSTANT, BORDER_COLOR);
  int bw = borderedLengthOf(loc_.w, targetScaleTable_.at(Device::GPU));
  int bh = borderedLengthOf(loc_.h, targetScaleTable_.at(Device::GPU));
  assert(bw == rgbMat.cols && bh == rgbMat.rows);
  return rgbMat;
}

void MergedROI::setPackInfo(IntPair xy, int packedCanvasIndex,
                            ExecutionType executionType, int roiSize) {
  if (executionType == ExecutionType::EMULATED_BATCH
      || executionType == ExecutionType::ROI_WISE_INFERENCE) {
    int bw = borderedLengthOf(loc_.w, targetScaleTable_.at(Device::GPU));
    int bh = borderedLengthOf(loc_.h, targetScaleTable_.at(Device::GPU));
    if (roiSize < std::max(bw, bh)) {
      LOGE("ROISize %d < bw %d or bh %d", roiSize, bw, bh);
      assert(false);
    }
    xy.first += (roiSize - bw) / 2;
    xy.second += (roiSize - bh) / 2;
  }
  packedXY_ = xy;
  packedCanvasIndex_ = packedCanvasIndex;
}

std::string MergedROI::header() {
  std::stringstream ss;
  ss << "mergedROIs" << DELIM
     << Rect::header("mergedLoc") << DELIM
     << "mergedScale" << DELIM
     << "pid" << DELIM
     << "packedXY_x" << DELIM
     << "packedXY_y" << DELIM
     << "packedCanvasIndex" << DELIM
     << "packedCanvasSize" << DELIM
     << "isProbing" << DELIM
     << "probingBoxID";
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
  ss << loc_.str() << DELIM
     << targetScaleTable_.at(Device::GPU) << DELIM
     << pid_ << DELIM
     << packedXY_.first << DELIM
     << packedXY_.second << DELIM
     << packedCanvasIndex_ << DELIM
     << packedCanvasSize_ << DELIM
     << isProbing_ << DELIM
     << probingBoxID_;
  return ss.str();
}

void MergedROI::setProbingBox(BoundingBox* box) {
  probingBox_ = box;
  probingBoxID_ = box != nullptr ? box->bid : -1;
}

} // namespace md
