#ifndef RESIZE_PROFILE_HPP_
#define RESIZE_PROFILE_HPP_

#include <string>

#include "strm/DataType.hpp"

namespace rm {

class ResizeProfile {
 public:
  virtual ~ResizeProfile() {}

  virtual int getTargetSize(const idType id, const RoI::Features& features) = 0;

  virtual void updateTable(RoI* roi) = 0;

  virtual int getProbingStep() = 0;
};

}

#endif // RESIZE_PROFILE_HPP_
