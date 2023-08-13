#include "mondrian/BoundingBox.hpp"

namespace md {

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
