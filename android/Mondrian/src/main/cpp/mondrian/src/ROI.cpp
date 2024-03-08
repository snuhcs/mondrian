#include "mondrian/ROI.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/Frame.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

ROI::ROI(const OID oid,
         Frame* frame,
         const cv::Rect2f& origLoc,
         const ROIType type,
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
          -1,         // xyRatio
          confidence, // confidence
          ofFeatures  // OFFeatures
      },
      mergedROI(nullptr),
      box_(nullptr),
      boxID_(-1) {
  setPaddedLoc({origLoc.x - padding,
                origLoc.y - padding,
                origLoc.width + 2 * padding,
                origLoc.height + 2 * padding});

  for (Device device : DEVICES) {
    targetScaleTable_[device] = 1.0f; // TODO: Start with targetScale_(-1) and assert
    scaleLevelTable_[device] = ROIResizer::INVALID_LEVEL;
  }

}

void ROI::setPaddedLoc(const cv::Rect2f& newPaddedLoc) {
  paddedLoc = newPaddedLoc & frame->rectf();
  features.width = paddedLoc.width;
  features.height = paddedLoc.height;
  features.xyRatio = (float) paddedLoc.width / (float) paddedLoc.height;
}

void ROI::eatPD(const cv::Rect2f& PDRect) {
  setPaddedLoc(paddedLoc | PDRect);
}

void ROI::scaleTo(float newTargetScale, int newScaleLevel, Device device) {
  assert(0.0f < newTargetScale);
  assert(newTargetScale <= 1.0f);
  targetScaleTable_[device] = newTargetScale;
  scaleLevelTable_[device] = newScaleLevel;
}

std::string ROI::header() {
  std::stringstream ss;
  ss << "vid" << DELIM
     << "fid" << DELIM
     << "rid" << DELIM
     << "oid" << DELIM
     << "origLoc_l" << DELIM
     << "origLoc_t" << DELIM
     << "origLoc_r" << DELIM
     << "origLoc_b" << DELIM
     << "paddedLoc_l" << DELIM
     << "paddedLoc_t" << DELIM
     << "paddedLoc_r" << DELIM
     << "paddedLoc_b" << DELIM
     << Features::header() << DELIM
     << "targetScale[GPU]" << DELIM
     << "targetScale[DSP]" << DELIM
     << "scaleLevel[GPU]" << DELIM
     << "scaleLevel[DSP]" << DELIM
     << "numProbingScales[GPU]" << DELIM
     << "numProbingScales[DSP]" << DELIM
     << "numRoisForProbing[GPU]" << DELIM
     << "numRoisForProbing[DSP]" << DELIM
     << MergedROI::header() << DELIM
     << "boxID";
  return ss.str();
}

std::string ROI::str(time_us baseTime) const {
  std::stringstream ss;
  ss << frame->vid << DELIM
     << frame->fid << DELIM
     << rid << DELIM
     << oid << DELIM
     << origLoc.x << DELIM
     << origLoc.y << DELIM
     << origLoc.x + origLoc.width << DELIM
     << origLoc.y + origLoc.height << DELIM
     << paddedLoc.x << DELIM
     << paddedLoc.y << DELIM
     << paddedLoc.x + paddedLoc.width << DELIM
     << paddedLoc.y + paddedLoc.height << DELIM
     << features.str() << DELIM
     << safeGet<float>(targetScaleTable_, Device::GPU) << DELIM
     << safeGet<float>(targetScaleTable_, Device::DSP) << DELIM
     << safeGet<int>(scaleLevelTable_, Device::GPU) << DELIM
     << safeGet<int>(scaleLevelTable_, Device::DSP) << DELIM
     << safeGetSize<float>(probeScalesTable, Device::GPU) << DELIM
     << safeGetSize<float>(probeScalesTable, Device::DSP) << DELIM
     << safeGetSize<MergedROI*>(roisForProbingTable, Device::GPU) << DELIM
     << safeGetSize<MergedROI*>(roisForProbingTable, Device::DSP) << DELIM
     << mergedROI->str(baseTime) << DELIM // TODO: Remove redundant mergedROI
     << boxID_;
  return ss.str();
}

void ROI::setBox(BoundingBox* box) {
  box_ = box;
  boxID_ = box != nullptr ? box->bid : -1;
}

} // namespace md
