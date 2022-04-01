#ifndef CUSTOM_RESIZE_PROFILE_H
#define CUSTOM_RESIZE_PROFILE_H

#include <jni.h>

#include "strm/ResizeProfile.hpp"

namespace rm {

class CustomResizeProfile : public ResizeProfile {
 public:
  float getScale(const std::string& labelName, int width, int height, int minOriginLength) {
    return (float) std::max(width, height) / 160;
  }
};

}

#endif // CUSTOM_RESIZE_PROFILE_H
