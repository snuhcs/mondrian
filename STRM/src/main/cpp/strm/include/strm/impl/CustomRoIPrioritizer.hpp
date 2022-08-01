#ifndef CUSTOM_ROI_PRIORITIZER_H
#define CUSTOM_ROI_PRIORITIZER_H

#include "strm/RoIPrioritizer.hpp"

namespace rm {

class CustomRoIPrioritizer : public RoIPrioritizer {
 public:
  int priority(const RoI& roi) const {
    return roi.location.area();
  }
};

} // namespace rm

#endif // CUSTOM_ROI_PRIORITIZER_H
