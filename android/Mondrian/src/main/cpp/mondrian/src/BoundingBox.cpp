#include "mondrian/BoundingBox.hpp"

#include "mondrian/ROI.hpp"

namespace md {

BoundingBox::BoundingBox(OID oid,
                         PID pid,
                         const cv::Rect2f& loc,
                         const float confidence,
                         int label,
                         BoxOrigin origin)
    : bid(BoundingBox::nextBID_++),
      oid(oid),
      pid(pid),
      loc(loc),
      confidence(confidence),
      label(label),
      origin(origin),
      srcROI_(nullptr),
      srcROIID_(-1),
      choiceOfBox(INVALID_OID) {}

void BoundingBox::setSrcROI(ROI* roi) {
  srcROI_ = roi;
  srcROIID_ = roi != nullptr ? roi->rid : -1;
}

std::string BoundingBox::header() {
  std::stringstream ss;
  ss << "bid" << DELIM
     << "oid" << DELIM
     << "pid" << DELIM
     << "box_l" << DELIM
     << "box_t" << DELIM
     << "box_r" << DELIM
     << "box_b" << DELIM
     << "confidence" << DELIM
     << "label" << DELIM
     << "srcROI" << DELIM
     << "origin" << DELIM
     << "choiceOfBox";
  return ss.str();
}

std::string BoundingBox::str() const {
  std::stringstream ss;
  ss << bid << DELIM
     << oid << DELIM
     << pid << DELIM
     << loc.x << DELIM
     << loc.y << DELIM
     << loc.x + loc.width << DELIM
     << loc.y + loc.height << DELIM
     << confidence << DELIM
     << COCO_LABELS[label] << DELIM
     << srcROIID_ << DELIM
     << ::md::str(origin) << DELIM
     << choiceOfBox;
  return ss.str();
}

} // namespace md
