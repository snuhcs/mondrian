#ifndef IMPL_CUSTOM_RESIZE_PROFILER_H
#define IMPL_CUSTOM_RESIZE_PROFILER_H

#include "strm/ResizeProfiler.hpp"

namespace rm {

class CustomResizeProfiler : public ResizeProfiler {
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

} // namespace rm

#endif // IMPL_CUSTOM_RESIZE_PROFILER_H
