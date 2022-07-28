#ifndef RESIZE_PROFILE_HPP_
#define RESIZE_PROFILE_HPP_

#include "strm/Config.hpp"
#include "strm/DataType.hpp"

namespace rm {

class ResizeProfile {
 public:
  ResizeProfile(const ResizeProfileConfig& config);

  int getTargetSize(const idType id, const RoI::Features& features);

  void updateTable(RoI* roi);

  int getProbingStep() {
    return mConfig.PROBING_STEP;
  }

 private:
  int getSmoothedTargetSize(const idType id, const RoI::Features& features);

  static int getSizeWithFeature(const RoI::Features& features);

  static float getOverlap(Rect& targetRect, Rect& baseRect);

  static bool isUsable(BoundingBox& targetBox, BoundingBox& baseBox);

  const ResizeProfileConfig mConfig;
  int calibration;
  std::map<idType, int> prevTargetSizeTable; // id, previous target size
};

} // namespace rm

#endif // RESIZE_PROFILE_HPP_
