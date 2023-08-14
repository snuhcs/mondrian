#include "mondrian/BoundingBox.hpp"

namespace md {

BoundingBox::BoundingBox(OID id,
                         const Rect& loc,
                         const float confidence,
                         int label,
                         Origin origin)
    : bid(BoundingBox::nextBID_++),
      id(id),
      loc(loc),
      confidence(confidence),
      label(label),
      origin(origin),
      srcROI(nullptr),
      choiceOfBox(INVALID_OID) {}

std::string BoundingBox::header() {
  std::stringstream ss;
  ss << "id" << DELIM
     << "left" << DELIM
     << "top" << DELIM
     << "right" << DELIM
     << "bottom" << DELIM
     << "confidence" << DELIM
     << "label" << DELIM
     << "origin" << DELIM
     << "choiceOfBox";
  return ss.str();
}

std::string BoundingBox::str() const {
  std::stringstream ss;
  ss << id << DELIM
     << loc.l << DELIM
     << loc.t << DELIM
     << loc.r << DELIM
     << loc.b << DELIM
     << confidence << DELIM
     << COCO_LABELS[label] << DELIM
     << ::md::str(origin) << DELIM
     << choiceOfBox;
  return ss.str();
}

} // namespace md
