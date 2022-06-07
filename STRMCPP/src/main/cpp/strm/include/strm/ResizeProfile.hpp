#ifndef RESIZE_PROFILE_HPP_
#define RESIZE_PROFILE_HPP_

#include <string>

#include "strm/DataType.hpp"

namespace rm {

class ResizeProfile {
 public:
  virtual ~ResizeProfile() {}

  virtual int getTargetSize(const RoI::Features& features) const = 0;
};

}

#endif // RESIZE_PROFILE_HPP_
