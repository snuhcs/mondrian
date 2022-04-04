#ifndef RESIZE_PROFILE_HPP_
#define RESIZE_PROFILE_HPP_

#include <string>

namespace rm {

class ResizeProfile {
 public:
  virtual float getScale(const std::string& labelName,
                         int width, int height, int minOriginLength) const = 0;

  virtual ~ResizeProfile() {}
};

}

#endif // RESIZE_PROFILE_HPP_
