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
  OID oid;
  PID pid;
  Rect loc;
  float confidence;
  int label;
  Origin origin;
  OID choiceOfBox;

  BoundingBox(OID oid,
              PID pid,
              const Rect& loc,
              const float confidence,
              int label,
              Origin origin);

  void setSrcROI(ROI* roi);

  ROI* srcROI() const { return srcROI_; }

  static std::string header();

  std::string str() const;

 private:
  ROI* srcROI_;
  RID srcROIID_;
};

} // namespace md

#endif // BOUNDING_BOX_HPP_
