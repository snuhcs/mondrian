#ifndef IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_
#define IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_

#include "strm/ResizeProfile.hpp"
#include "strm/impl/ImplConfig.hpp"
#include "strm/impl/CustomTreeOF.hpp"
#include "strm/impl/CustomTreePD.hpp"

namespace rm {

class AccuracyAwareResizeProfile : public ResizeProfile {
 public:
  AccuracyAwareResizeProfile(int resizeMargin)
  : RESIZE_MARGIN(resizeMargin) {}

  int getTargetSize(const RoI::Features& features) const {
    if (features.type == RoI::OF) {
      return (int) OFTree(features.xyRatio, features.shift, features.err) + RESIZE_MARGIN;
    } else if (features.type == RoI::PD) {
      return (int) PDTree(features.xyRatio, features.diffAreaRatio) + RESIZE_MARGIN;
    }
    return INT_MAX;
  }

 private:
  const int RESIZE_MARGIN;
};

} // namespace rm

#endif // IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_
