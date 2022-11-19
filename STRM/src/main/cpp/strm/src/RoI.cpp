#include "strm/RoI.hpp"

#include "strm/Frame.hpp"
#include "strm/RoIResizer.hpp"

namespace rm {

const IntPair RoI::INVALID_XY{-1, -1};

const int RoI::INVALID_CONF = -1;

RoI::RoI(RoI* prevRoI,
         const idType id,
         Frame* frame,
         const Rect origLoc,
         const Type type,
         const Origin origin,
         const int label,
         const OFFeatures ofFeatures,
         const float confidence,
         float roiPadding,
         bool isProbingRoI)
    : prevRoI(prevRoI), id(id), frame(frame),
      type(type), origin(origin), label(label), features{
        -1,
        -1,
        label,
        type,
        origin,
        -1,
        ofFeatures,
        confidence
    }, roiPadding(roiPadding),
      targetScale(1.0f), scaleLevel(RoIResizer::INVALID_LEVEL), packedXY(INVALID_XY),
      isMatchTried(false), nextRoI(nullptr), parentRoI(nullptr), box(nullptr), probingBox(nullptr),
      packedMixedFrameIndex(INT_MAX), packedAbsMixedFrameIndex(-1),
      isProbingRoI(isProbingRoI), priority(-1) {
  if (prevRoI != nullptr) {
    prevRoI->nextRoI = this;
  }
  setOrigLoc(origLoc);
}

void RoI::setOrigLoc(const Rect& newOrigLoc) {
  origLoc = newOrigLoc;

  paddedLoc = Rect(std::max(0.0f, origLoc.left - roiPadding),
                   std::max(0.0f, origLoc.top - roiPadding),
                   std::min(float(frame->mat.cols), origLoc.right + roiPadding),
                   std::min(float(frame->mat.rows), origLoc.bottom + roiPadding));

  features.width = paddedLoc.width();
  features.height = paddedLoc.height();
  features.xyRatio = (float) paddedLoc.width() / (float) paddedLoc.height();

  maxEdgeLength = std::max(paddedLoc.width(), paddedLoc.height());
}

void RoI::eatPD(const Rect& PDRect) {
  float newLeft = std::min(this->paddedLoc.left, PDRect.left);
  float newTop = std::min(this->paddedLoc.top, PDRect.top);
  float newRight = std::max(this->paddedLoc.right, PDRect.right);
  float newBottom = std::max(this->paddedLoc.bottom, PDRect.bottom);
  setOrigLoc(Rect(newLeft, newTop, newRight, newBottom));
}

std::unique_ptr<RoI> RoI::mergeRoIs(const RoI* pRoI0, const RoI* pRoI1) {
  assert(pRoI0->frame == pRoI1->frame);
  float newLeft = std::min(pRoI0->paddedLoc.left, pRoI1->paddedLoc.left);
  float newTop = std::min(pRoI0->paddedLoc.top, pRoI1->paddedLoc.top);
  float newRight = std::max(pRoI0->paddedLoc.right, pRoI1->paddedLoc.right);
  float newBottom = std::max(pRoI0->paddedLoc.bottom, pRoI1->paddedLoc.bottom);
  Type roiType = pRoI0->type != PD || pRoI1->type != PD
                 ? OF : PD;
  int roiLabel;
  if (pRoI0->label == pRoI1->label) {
    roiLabel = pRoI0->label;
  } else if (pRoI0->label != -1 && pRoI1->label == -1) {
    roiLabel = pRoI0->label;
  } else if (pRoI0->label == -1 && pRoI1->label != -1) {
    roiLabel = pRoI1->label;
  } else {
    roiLabel = -1;
  }
  std::unique_ptr<RoI> mergedRoI = std::make_unique<RoI>(
      nullptr, MERGED_ROI_ID, pRoI0->frame, Rect(newLeft, newTop, newRight, newBottom),
      roiType, origin_Null, roiLabel, OFFeatures({}, {}, {}), RoI::INVALID_CONF, 0, false);
  float scale = std::max(pRoI0->targetScale, pRoI1->targetScale);
  assert(0.0f < scale && scale <= 1.0f);
  mergedRoI->setTargetScale(scale, RoIResizer::INVALID_LEVEL);
  return std::move(mergedRoI);
}

void RoI::setTargetScale(float newTargetScale, int newScaleLevel) {
  assert(0.0f < newTargetScale);
  assert(newTargetScale <= 1.0f);
  float minEdgeLength = std::min(paddedLoc.width(), paddedLoc.height());
  // compare with 1/minEdgeLength to prevent shorter edge being even shorter than 1 after downscaling
  targetScale = std::max(1.0f / minEdgeLength, newTargetScale);
  scaleLevel = newScaleLevel;
}

cv::Mat RoI::getOrigMat() const {
  int left = std::max(0, std::min(frame->mat.cols, int(origLoc.left)));
  int top = std::max(0, std::min(frame->mat.rows, int(origLoc.top)));
  int width = std::max(0, std::min(frame->mat.cols - left, int(origLoc.width())));
  int height = std::max(0, std::min(frame->mat.rows - top, int(origLoc.height())));
  return frame->mat.operator()(cv::Rect(left, top, width, height));
}

cv::Mat RoI::getPaddedMat() const {
  int left = std::max(0, std::min(frame->mat.cols, int(paddedLoc.left)));
  int top = std::max(0, std::min(frame->mat.rows, int(paddedLoc.top)));
  int width = std::max(0, std::min(frame->mat.cols - left, int(paddedLoc.width())));
  int height = std::max(0, std::min(frame->mat.rows - top, int(paddedLoc.height())));
  return frame->mat.operator()(cv::Rect(left, top, width, height));
}

cv::Mat RoI::getResizedMat(bool emulatedBatch, int roiSize) const {
  IntPair rwh = emulatedBatch
                ? getWHForRoISize(roiSize)
                : getResizedMatWidthHeight();
  auto[rw, rh] = rwh;
  cv::Mat resizedMat;
  cv::resize(getPaddedMat(), resizedMat, cv::Size(rw, rh));
  return resizedMat;
}

} // namespace rm
