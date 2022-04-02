#ifndef ROI_PRIORITIZER_HPP_
#define ROI_PRIORITIZER_HPP_

#include "DataType.hpp"

namespace rm {

class RoIPrioritizer {
 public:
  virtual bool priority(const RoI& roi) = 0;

  virtual ~RoIPrioritizer() {};
};

}

#endif // ROI_PRIORITIZER_HPP_
