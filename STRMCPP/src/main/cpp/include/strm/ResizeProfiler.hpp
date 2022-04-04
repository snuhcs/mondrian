#ifndef RESIZE_PROFILER_HPP_
#define RESIZE_PROFILER_HPP_

#include <string>

namespace rm {

class ResizeProfiler {
 public:
  virtual float getScale(const std::string& labelName,
                         int width, int height, int minOriginLength) const = 0;
};

}

#endif // RESIZE_PROFILER_HPP_
