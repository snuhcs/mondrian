#include "mondrian/BoundingBox.hpp"

#include "mondrian/ROI.hpp"

namespace md {

BoundingBox::BoundingBox(OID oid,
                         const Rect& loc,
                         const float confidence,
                         int label,
                         Origin origin)
    : bid(BoundingBox::nextBID_++),
      oid(oid),
      pid(-1),
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
     << Rect::header("box") << DELIM
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
     << loc.str() << DELIM
     << confidence << DELIM
     << COCO_LABELS[label] << DELIM
     << srcROIID_ << DELIM
     << ::md::str(origin) << DELIM
     << choiceOfBox;
  return ss.str();
}

} // namespace md
