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

 private:
  static inline std::atomic<RID> nextRID_ = 0;
  static inline std::atomic<OID> nextOID_ = 0;

 public:
  Frame* const frame;
  const RID rid;
  OID oid;
  const cv::Rect2f origLoc;
  cv::Rect2f paddedLoc;
  Features features;

 private:
  std::map<Device, float> targetScaleTable_;
  std::map<Device, int> scaleLevelTable_;

 public:
  std::map<Device, std::vector<float>> probeScalesTable;
  std::map<Device, std::vector<MergedROI*>> roisForProbingTable;
  MergedROI* mergedROI;

 private:
  BoundingBox* box_;
  BID boxID_;

 public:
  void setBox(BoundingBox* box);

  BoundingBox* box() const { return box_; }

  ROI(const OID oid,
      Frame* frame,
      const cv::Rect2f& origLoc,
      const ROIType type,
      const int label,
      const OFFeatures ofFeatures,
      const float confidence,
      const float padding);

  void setPaddedLoc(const cv::Rect2f& newOrigLoc);

  void eatPD(const cv::Rect2f& PDRect);

  static std::pair<OID, OID> getNewOIDs(unsigned long num) {
    OID minOID = nextOID_.fetch_add(num);
    OID maxOID = minOID + num;
    return {minOID, maxOID};
  }

  std::map<Device, float> targetScaleTable() const {
    return targetScaleTable_;
  }

  int scaleLevel() const {
    return scaleLevelTable_.at(PROBING_DEVICE);
  }

  ROIType type() const {
    return features.type;
  }

  void setLabel(int label) {
    features.label = label;
  }

  int label() const {
    return features.label;
  }

  void scaleTo(float newTargetScale, int newScaleLevel, Device device);

  static std::string header();

  std::string str(time_us baseTime) const;
};

} // namespace md

#endif // ROI_HPP_
