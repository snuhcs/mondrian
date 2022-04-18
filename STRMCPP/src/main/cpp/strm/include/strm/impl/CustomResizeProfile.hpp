#ifndef IMPL_CUSTOM_RESIZE_PROFILE_H
#define IMPL_CUSTOM_RESIZE_PROFILE_H

#include "strm/ResizeProfile.hpp"
#include "strm/impl/ImplConfig.hpp"

namespace rm {

class CustomResizeProfile : public ResizeProfile {
 public:
  CustomResizeProfile(const ResizeProfileConfig& config)
      : mConfig(config) {}

  float getScale(const std::string& labelName,
                 int width, int height, int minOriginLength) const {
    int lengthThreshold = labelName == "person" ? mConfig.PERSON_THRESHOLD : mConfig.CLASS_AGNOSTIC_THRESHOLD;
    int maxWidthHeight = mConfig.MERGED_RESIZE ? minOriginLength : std::max(width, height);
    if (mConfig.FIT_RESIZE || maxWidthHeight > lengthThreshold) {
      return (float) lengthThreshold / maxWidthHeight;
    }
    return 1.0;
  }

 private:
  const ResizeProfileConfig mConfig;
};

} // namespace rm

#endif // IMPL_CUSTOM_RESIZE_PROFILE_H
