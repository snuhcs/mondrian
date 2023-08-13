#ifndef BOUNDING_BOX_HPP_
#define BOUNDING_BOX_HPP_

#include "mondrian/DataType.hpp"

namespace md {

class ROI;

struct BoundingBox {
  OID id;
  Rect loc;
  float confidence;
  int label;
  ROI* srcROI;
  Origin origin;
  OID choiceOfBox;

  BoundingBox(OID id, const Rect location, const float confidence, int label, Origin origin)
      : id(id), loc(location), confidence(confidence), label(label), origin(origin),
        srcROI(nullptr), choiceOfBox(INVALID_OID) {}

  static std::string header();

  std::string str() const;
};

} // namespace md

#endif // BOUNDING_BOX_HPP_
