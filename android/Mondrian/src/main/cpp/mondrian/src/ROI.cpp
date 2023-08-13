#include "mondrian/ROI.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

const float ROI::INVALID_CONF = -1.0f;

ROI::ROI(ROI* prevROI,
         const OID id,
         Frame* frame,
         const Rect& origLoc,
         const ROIType type,
         const Origin origin,
         const int label,
         const OFFeatures ofFeatures,
         const float confidence,
         const float padding)
    : prevROI(prevROI), id(id), frame(frame), origLoc(origLoc),
      type(type), origin(origin), label(label), features{
        -1,         // width
        -1,         // height
        label,      // label
        type,       // type
        origin,     // origin
        -1,         // xyRatio
        confidence, // confidence
        ofFeatures  // OFFeatures
    }, targetScale_(1.0f), // TODO: Start with targetScale_(-1) and assert
      scaleLevel_(ROIResizer::INVALID_LEVEL), nextROI(nullptr), mergedROI(nullptr), box(nullptr) {
  if (prevROI != nullptr) {
    prevROI->nextROI = this;
  }
  setPaddedLoc({std::max(0.0f, origLoc.l - padding),
                std::max(0.0f, origLoc.t - padding),
                std::min((float) frame->width(), origLoc.r + padding),
                std::min((float) frame->height(), origLoc.b + padding)});
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

void ROI::scaleTo(float newTargetScale, int newScaleLevel) {
  assert(0.0f < newTargetScale);
  assert(newTargetScale <= 1.0f);
  targetScale_ = newTargetScale;
  scaleLevel_ = newScaleLevel;
}

} // namespace md
