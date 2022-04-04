#ifndef CUSTOM_RESIZE_PROFILE_H
#define CUSTOM_RESIZE_PROFILE_H

#include <jni.h>

#include "strm/ResizeProfile.hpp"

namespace rm {

class CustomResizeProfile : public ResizeProfile {
 public:
  float getScale(const std::string& labelName,
                 int width, int height, int minOriginLength) const {
    int maxLength = std::max(width, height);
    if (maxLength > 160) {
      return 160.0 / (float) maxLength;
    } else {
      return 1.0;
    }
  }
};

}

#endif // CUSTOM_RESIZE_PROFILE_H
