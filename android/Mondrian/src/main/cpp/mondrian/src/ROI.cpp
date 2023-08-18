#include "mondrian/ROI.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

ROI::ROI(const OID oid,
         Frame* frame,
         const Rect& origLoc,
         const ROIType type,
         const Origin origin,
         const int label,
         const OFFeatures ofFeatures,
         const float confidence,
         const float padding)
    : rid(ROI::nextRID_++),
      oid(oid),
      frame(frame),
      origLoc(origLoc),
      features{
          -1,         // width
          -1,         // height
          label,      // label
          type,       // type
          origin,     // origin
          -1,         // xyRatio
          confidence, // confidence
          ofFeatures  // OFFeatures
      },
      targetScale_(1.0f), // TODO: Start with targetScale_(-1) and assert
      scaleLevel_(ROIResizer::INVALID_LEVEL),
      mergedROI(nullptr),
      box_(nullptr),
      boxID_(-1) {
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

std::string ROI::header() {
  std::stringstream ss;
  ss << "vid" << DELIM
     << "fid" << DELIM
     << "rid" << DELIM
     << "oid" << DELIM
     << Rect::header("origLoc") << DELIM
     << Rect::header("paddedLoc") << DELIM
     << Features::header() << DELIM
     << "targetScale" << DELIM
     << "scaleLevel" << DELIM
     << "numProbingScales" << DELIM
     << "numRoisForProbing" << DELIM
     << MergedROI::header() << DELIM
     << "boxID";
  return ss.str();
}

std::string ROI::str() const {
  std::stringstream ss;
  ss << frame->vid << DELIM
     << frame->fid << DELIM
     << rid << DELIM
     << oid << DELIM
     << origLoc.str() << DELIM
     << paddedLoc.str() << DELIM
     << features.str() << DELIM
     << targetScale_ << DELIM
     << scaleLevel_ << DELIM
     << probeScales.size() << DELIM
     << roisForProbing.size() << DELIM
     << mergedROI->str() << DELIM // TODO: Remove redundant mergedROI
     << boxID_;
  return ss.str();
}

void ROI::setBox(BoundingBox* box) {
  box_ = box;
  boxID_ = box != nullptr ? box->bid : -1;
}

} // namespace md
