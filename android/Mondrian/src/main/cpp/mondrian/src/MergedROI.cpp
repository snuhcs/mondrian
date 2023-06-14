#include "mondrian/MergedROI.hpp"

#include "mondrian/Frame.hpp"

namespace md {

int MergedROI::BORDER = -1e5;
const cv::Scalar MergedROI::BORDER_COLOR(255, 255, 255);
const IntPair MergedROI::INVALID_XY{-1, -1};

MergedROI::MergedROI(const std::vector<ROI*>& rois, float targetScale, bool isProbing)
    : rois_(rois), targetScale_(targetScale), isProbing_(isProbing),
      frame_(frameOf(rois)), loc_(locOf(rois)),
      packedXY_(INVALID_XY), relativePackedCanvasIndex_(-1), absolutePackedCanvasIndex_(-1),
      packedCanvasSize_(-1), probingBox_(nullptr) {}

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
  float newScale = std::max(m0->targetScale_, m1->targetScale_);
  return std::make_unique<MergedROI>(newROIs, newScale, false);
}

cv::Mat MergedROI::mat() const {
  int l = std::max(0, std::min(frame_->rgbMat.cols, int(loc_.l)));
  int t = std::max(0, std::min(frame_->rgbMat.rows, int(loc_.t)));
  int w = std::max(0, std::min(frame_->rgbMat.cols - l, int(loc_.w)));
  int h = std::max(0, std::min(frame_->rgbMat.rows - t, int(loc_.h)));
  return frame_->rgbMat.operator()(cv::Rect(l, t, w, h));
}

cv::Mat MergedROI::resizedMat() const {
  int rw = resizedLengthOf(loc_.w, targetScale_);
  int rh = resizedLengthOf(loc_.h, targetScale_);
  cv::Mat rgbMat;
  cv::resize(mat(), rgbMat, cv::Size(rw, rh));
  return rgbMat;
}

cv::Mat MergedROI::borderedMat() const {
  cv::Mat rgbMat = resizedMat();
  cv::copyMakeBorder(rgbMat, rgbMat,
                     BORDER, BORDER, BORDER, BORDER,
                     cv::BORDER_CONSTANT, BORDER_COLOR);
  int bw = borderedLengthOf(loc_.w, targetScale_);
  int bh = borderedLengthOf(loc_.h, targetScale_);
  assert(bw == rgbMat.cols && bh == rgbMat.rows);
  return rgbMat;
}

void MergedROI::setPackInfo(IntPair xy, int relativePackedCanvasIndex,
                            ExecutionType executionType, int roiSize) {
  if (executionType == EMULATED_BATCH) {
    int bw = borderedLengthOf(loc_.w, targetScale_);
    int bh = borderedLengthOf(loc_.h, targetScale_);
    if (roiSize < std::max(bw, bh)) {
      LOGE("ROISize %d < bw %d or bh %d", roiSize, bw, bh);
      assert(false);
    }
    xy.first += (roiSize - bw) / 2;
    xy.second += (roiSize - bh) / 2;
  }
  packedXY_ = xy;
  relativePackedCanvasIndex_ = relativePackedCanvasIndex;
}

} // namespace md
