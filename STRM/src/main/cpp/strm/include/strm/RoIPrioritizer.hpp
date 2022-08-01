#ifndef ROI_PRIORITIZER_HPP_
#define ROI_PRIORITIZER_HPP_

#include "DataType.hpp"

namespace rm {

class RoIPrioritizer {
 public:
  virtual ~RoIPrioritizer() {}

  virtual int priority(const RoI& roi) const = 0;
};

}

#endif // ROI_PRIORITIZER_HPP_
