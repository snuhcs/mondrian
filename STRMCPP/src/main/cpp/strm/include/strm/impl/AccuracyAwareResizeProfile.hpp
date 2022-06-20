#ifndef IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_
#define IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_

#include "strm/ResizeProfile.hpp"
#include "strm/impl/ImplConfig.hpp"

namespace rm {

class AccuracyAwareResizeProfile : public ResizeProfile {
 public:
  AccuracyAwareResizeProfile(int resizeMargin, float resizeSmoothingFactor, int probeStep);

  int getTargetSize(const idType id, const RoI::Features& features) override;

  void updateTable(RoI* roi) override;

  int getProbingStep() override {
    return probeStep;
  }

 private:
  int getSmoothedTargetSize(const idType id, const RoI::Features& features);

  static int getSizeWithFeature(const RoI::Features& features);

  static float getOverlap(Rect& targetRect, Rect& baseRect);

  static bool isUsable(BoundingBox& targetBox, BoundingBox& baseBox);

  const int resizeMargin;
  int probeStep;
  int calibration;
  float resizeSmoothingFactor;
  std::map<idType, int> prevTargetSizeTable; // id, previous target size
};

} // namespace rm

#endif // IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_
