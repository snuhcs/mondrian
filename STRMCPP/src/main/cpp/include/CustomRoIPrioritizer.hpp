#ifndef CUSTOM_ROI_PRIORITIZER_H
#define CUSTOM_ROI_PRIORITIZER_H

#include <jni.h>

#include "strm/RoIPrioritizer.hpp"

namespace rm {

class CustomRoIPrioritizer : public RoIPrioritizer {
 public:
  bool priority(const RoI& roi) {
    return roi.location.area();
  }
};

} // namespace rm

#endif // CUSTOM_ROI_PRIORITIZER_H
