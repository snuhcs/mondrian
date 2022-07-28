#ifndef ROI_RESIZER_HPP_
#define ROI_RESIZER_HPP_

#include "strm/Config.hpp"
#include "strm/DataType.hpp"

namespace rm {

class RoIResizer {
 public:
  RoIResizer(const RoIResizerConfig& config);

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

  const RoIResizerConfig mConfig;
  int calibration;
  std::map<idType, int> prevTargetSizeTable; // id, previous target size
};

} // namespace rm

#endif // ROI_RESIZER_HPP_
