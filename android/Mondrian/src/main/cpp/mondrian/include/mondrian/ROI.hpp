#ifndef ROI_HPP_
#define ROI_HPP_

#include <cmath>

#include "opencv2/core/hal/interface.h"
#include "opencv2/imgproc.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/Features.hpp"
#include "mondrian/Log.hpp"

namespace md {

class Frame;
class MergedROI;

class ROI {
 public:
  static float PADDING;
  static const float INVALID_CONF;

  Frame* const frame;
  const Rect origLoc;
  Rect paddedLoc;

  Type type;
  Origin origin;
  int label;
  Features features;
  std::vector<float> probeScales;
  std::vector<MergedROI*> roisForProbing;

  inline static std::atomic<ID> lastId = 0;
  ID id;
  ROI* prevROI; // only valid with rois_
  ROI* nextROI; // only valid with rois_
  MergedROI* mergedROI;

  float maxEdgeLength;

 private:
  float targetScale_;
  int scaleLevel_;

 public:
  BoundingBox* box;

  ROI(ROI* prevROI,
      const ID id,
      Frame* frame,
      const Rect& origLoc,
      const Type type,
      const Origin origin,
      const int label,
      const OFFeatures ofFeatures,
      const float confidence);

  void setPaddedLoc(const Rect& newOrigLoc);

  void eatPD(const Rect& PDRect);

  static std::pair<ID, ID> getNewIds(unsigned long num) {
    ID minId = lastId.fetch_add(num);
    ID maxId = minId + num;
    return std::pair<ID, ID>(minId, maxId); // [minId, maxId)
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
};

} // namespace md

#endif // ROI_HPP_
