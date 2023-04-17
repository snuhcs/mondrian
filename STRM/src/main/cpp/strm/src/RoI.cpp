#include "strm/RoI.hpp"

#include "strm/Frame.hpp"
#include "strm/RoIResizer.hpp"

namespace rm {

const IntPair RoI::INVALID_XY{-1, -1};

const int RoI::INVALID_CONF = -1;

const cv::Scalar RoI::BORDER_COLOR(255, 255, 255);

RoI::RoI(RoI* prevRoI,
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
         const bool isProbingRoI)
    : prevRoI(prevRoI), id(id), frame(frame), origLoc(origLoc),
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
      scaleLevel(RoIResizer::INVALID_LEVEL), packedXY(INVALID_XY),
      nextRoI(nullptr), parentRoI(nullptr), box(nullptr), probingBox(nullptr),
      packedMixedFrameIndex(INT_MAX), packedAbsMixedFrameIndex(-1), packedMixedFrameSize(-1),
      isProbingRoI(isProbingRoI), priority(-1) {
  if (prevRoI != nullptr) {
    prevRoI->nextRoI = this;
  }
  setPaddedLoc({std::max(0.0f, origLoc.left - roiPadding),
                std::max(0.0f, origLoc.top - roiPadding),
                std::min(float(frame->rgbMat.cols), origLoc.right + roiPadding),
                std::min(float(frame->rgbMat.rows), origLoc.bottom + roiPadding)});
}

void RoI::setPaddedLoc(const Rect& newPaddedLoc) {
  paddedLoc = newPaddedLoc;
  features.width = paddedLoc.width();
  features.height = paddedLoc.height();
  features.xyRatio = (float) paddedLoc.width() / (float) paddedLoc.height();
  maxEdgeLength = std::max(paddedLoc.width(), paddedLoc.height());
}

void RoI::eatPD(const Rect& PDRect) {
  setPaddedLoc(Rect::merge(paddedLoc, PDRect));
}

std::unique_ptr<RoI> RoI::mergeRoIs(const RoI* pRoI0, const RoI* pRoI1) {
  assert(pRoI0->frame == pRoI1->frame);
  assert(pRoI0->roiBorder == pRoI1->roiBorder);
  Frame* frame = pRoI0->frame;
  Rect rect = Rect::merge(pRoI0->paddedLoc, pRoI1->paddedLoc);
  Type roiType = pRoI0->type != PD || pRoI1->type != PD ? OF : PD;
  int roiLabel;
  if (pRoI0->label == -1 && pRoI1->label == -1) {
    roiLabel = -1;
  } else if (pRoI0->label != -1) {
    roiLabel = pRoI0->label;
  } else { // pRoI1->label != -1
    roiLabel = pRoI1->label;
  }
  std::unique_ptr<RoI> mergedRoI = std::make_unique<RoI>(
      nullptr, MERGED_ROI_ID, frame, rect, roiType, origin_Null, roiLabel,
      OFFeatures({}, {}, {}), RoI::INVALID_CONF, 0, pRoI0->roiBorder, false);
  float scale = std::max(pRoI0->targetScale, pRoI1->targetScale);
  assert(0.0f < scale && scale <= 1.0f);
  mergedRoI->setTargetScale(scale, RoIResizer::INVALID_LEVEL);
  return std::move(mergedRoI);
}

void RoI::setTargetScale(float newTargetScale, int newScaleLevel) {
  assert(0.0f < newTargetScale);
  assert(newTargetScale <= 1.0f);
  targetScale = newTargetScale;
  scaleLevel = newScaleLevel;
}

cv::Mat RoI::getPaddedMat() const {
  int left = std::max(0, std::min(frame->rgbMat.cols, int(paddedLoc.left)));
  int top = std::max(0, std::min(frame->rgbMat.rows, int(paddedLoc.top)));
  int width = std::max(0, std::min(frame->rgbMat.cols - left, int(paddedLoc.width())));
  int height = std::max(0, std::min(frame->rgbMat.rows - top, int(paddedLoc.height())));
  return frame->rgbMat.operator()(cv::Rect(left, top, width, height));
}

cv::Mat RoI::getResizedMat() const {
  auto[rw, rh] = getResizedMatWidthHeight();
  cv::Mat rgbMat;
  cv::resize(getPaddedMat(), rgbMat, cv::Size(rw, rh));
  return rgbMat;
}

cv::Mat RoI::getBorderMat() const {
  cv::Mat rgbMat = getResizedMat();
  cv::copyMakeBorder(rgbMat, rgbMat,
                     roiBorder, roiBorder, roiBorder, roiBorder,
                     cv::BORDER_CONSTANT, BORDER_COLOR);
  auto[bw, bh] = getBorderMatWidthHeight();
  assert(bw == rgbMat.cols && bh == rgbMat.rows);
  return rgbMat;
}

} // namespace rm
