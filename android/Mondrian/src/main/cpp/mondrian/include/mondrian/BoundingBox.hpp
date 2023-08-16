#ifndef BOUNDING_BOX_HPP_
#define BOUNDING_BOX_HPP_

#include <atomic>

#include "mondrian/DataType.hpp"

namespace md {

class ROI;

struct BoundingBox {
 public:
  static inline std::atomic<BID> nextBID_ = 0;

  const BID bid;
  OID id;
  Rect loc;
  float confidence;
  int label;
  ROI* srcROI;
  Origin origin;
  OID choiceOfBox;

  BoundingBox(OID id,
              const Rect& loc,
              const float confidence,
              int label,
              Origin origin);

  static std::string header();

  std::string str() const;
};

} // namespace md

#endif // BOUNDING_BOX_HPP_
