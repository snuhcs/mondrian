#include "mondrian/ROI.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

const IntPair ROI::INVALID_XY{-1, -1};

const int ROI::INVALID_CONF = -1;

const cv::Scalar ROI::BORDER_COLOR(255, 255, 255);

ROI::ROI(ROI* prevROI,
         const idType id,
         Frame* frame,
         const Rect origLoc,
         const Type type,
         const Origin origin,
         const int label,
         const OFFeatures ofFeatures,
         const float confidence,
         const float roiPadding,
         const int roiBorder,
         const bool isProbingROI)
    : prevROI(prevROI), id(id), frame(frame), origLoc(origLoc),
      type(type), origin(origin), label(label), features{
        -1,
        -1,
        label,
        type,
        origin,
        -1,
        ofFeatures,
        confidence
    }, roiBorder(roiBorder), targetScale(1.0f), // TODO: Start with targetScale(-1) and assert
      scaleLevel(ROIResizer::INVALID_LEVEL), packedXY(INVALID_XY),
      nextROI(nullptr), mergedROI(nullptr), box(nullptr), probingBox(nullptr),
      packedMixedFrameIndex(INT_MAX), packedAbsMixedFrameIndex(-1), packedMixedFrameSize(-1),
      isProbingROI(isProbingROI), priority(-1) {
  if (prevROI != nullptr) {
    prevROI->nextROI = this;
  }
  setPaddedLoc({std::max(0.0f, origLoc.l - roiPadding),
                std::max(0.0f, origLoc.t - roiPadding),
                std::min(float(frame->rgbMat.cols), origLoc.r + roiPadding),
                std::min(float(frame->rgbMat.rows), origLoc.b + roiPadding)});
}

void ROI::setPaddedLoc(const Rect& newPaddedLoc) {
  paddedLoc = newPaddedLoc;
  features.width = paddedLoc.w;
  features.height = paddedLoc.h;
  features.xyRatio = (float) paddedLoc.w / (float) paddedLoc.h;
  maxEdgeLength = std::max(paddedLoc.w, paddedLoc.h);
}

void ROI::eatPD(const Rect& PDRect) {
  setPaddedLoc(Rect::merge(paddedLoc, PDRect));
}

std::unique_ptr<ROI> ROI::mergeROIs(const ROI* pROI0, const ROI* pROI1) {
  assert(pROI0->frame == pROI1->frame);
  assert(pROI0->roiBorder == pROI1->roiBorder);
  Frame* frame = pROI0->frame;
  Rect rect = Rect::merge(pROI0->paddedLoc, pROI1->paddedLoc);
  Type roiType = pROI0->type != PD || pROI1->type != PD ? OF : PD;
  int roiLabel;
  if (pROI0->label == -1 && pROI1->label == -1) {
    roiLabel = -1;
  } else if (pROI0->label != -1) {
    roiLabel = pROI0->label;
  } else { // pROI1->label != -1
    roiLabel = pROI1->label;
  }
  std::unique_ptr<ROI> mergedROI = std::make_unique<ROI>(
      nullptr, MERGED_ROI_ID, frame, rect, roiType, O_NULL, roiLabel,
      OFFeatures({}, {}, {}), ROI::INVALID_CONF, 0, pROI0->roiBorder, false);
  float scale = std::max(pROI0->targetScale, pROI1->targetScale);
  assert(0.0f < scale && scale <= 1.0f);
  mergedROI->setTargetScale(scale, ROIResizer::INVALID_LEVEL);
  return std::move(mergedROI);
}

void ROI::setTargetScale(float newTargetScale, int newScaleLevel) {
  assert(0.0f < newTargetScale);
  assert(newTargetScale <= 1.0f);
  targetScale = newTargetScale;
  scaleLevel = newScaleLevel;
}

cv::Mat ROI::getPaddedMat() const {
  int l = std::max(0, std::min(frame->rgbMat.cols, int(paddedLoc.l)));
  int t = std::max(0, std::min(frame->rgbMat.rows, int(paddedLoc.t)));
  int w = std::max(0, std::min(frame->rgbMat.cols - l, int(paddedLoc.w)));
  int h = std::max(0, std::min(frame->rgbMat.rows - t, int(paddedLoc.h)));
  return frame->rgbMat.operator()(cv::Rect(l, t, w, h));
}

cv::Mat ROI::getResizedMat() const {
  auto[rw, rh] = getResizedMatWidthHeight();
  cv::Mat rgbMat;
  cv::resize(getPaddedMat(), rgbMat, cv::Size(rw, rh));
  return rgbMat;
}

cv::Mat ROI::getBorderMat() const {
  cv::Mat rgbMat = getResizedMat();
  cv::copyMakeBorder(rgbMat, rgbMat,
                     roiBorder, roiBorder, roiBorder, roiBorder,
                     cv::BORDER_CONSTANT, BORDER_COLOR);
  auto[bw, bh] = getBorderMatWidthHeight();
  assert(bw == rgbMat.cols && bh == rgbMat.rows);
  return rgbMat;
}

} // namespace md
