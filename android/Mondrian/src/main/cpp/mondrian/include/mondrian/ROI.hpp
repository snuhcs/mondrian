#ifndef ROI_HPP_
#define ROI_HPP_

#include <atomic>
#include <cmath>

#include "opencv2/core/hal/interface.h"
#include "opencv2/imgproc.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/Features.hpp"
#include "mondrian/Log.hpp"

namespace md {

class BoundingBox;
class Frame;
class MergedROI;

class ROI {
 public:
  static inline const float INVALID_CONF = -1.0f;
  static inline std::atomic<UID> nextUID_ = 0;
  static inline std::atomic<OID> lastId = 0;

  const UID uid;
  Frame* const frame;
  const Rect origLoc;
  Rect paddedLoc;

  ROIType type;
  Origin origin;
  int label;
  Features features;
  std::vector<float> probeScales;
  std::vector<MergedROI*> roisForProbing;

  OID id;
  ROI* prevROI; // only valid with rois_
  ROI* nextROI; // only valid with rois_
  MergedROI* mergedROI;

  float maxEdgeLength;

  BoundingBox* box;

  ROI(ROI* prevROI,
      const OID id,
      Frame* frame,
      const Rect& origLoc,
      const ROIType type,
      const Origin origin,
      const int label,
      const OFFeatures ofFeatures,
      const float confidence,
      const float padding);

  void setPaddedLoc(const Rect& newOrigLoc);

  void eatPD(const Rect& PDRect);

  static std::pair<OID, OID> getNewIds(unsigned long num) {
    OID minId = lastId.fetch_add(num);
    OID maxId = minId + num;
    return {minId, maxId};
  }

  float paddedArea() const {
    return paddedLoc.area;
  }

  float targetScale() const {
    return targetScale_;
  }

  int scaleLevel() const {
    return scaleLevel_;
  }

  void scaleTo(float newTargetScale, int newScaleLevel);

  float targetScale_;
  int scaleLevel_;
};

} // namespace md

#endif // ROI_HPP_
