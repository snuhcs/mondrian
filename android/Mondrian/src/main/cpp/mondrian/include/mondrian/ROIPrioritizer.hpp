#ifndef ROI_PRIORITIZER_HPP_
#define ROI_PRIORITIZER_HPP_

#include <set>

#include "mondrian/Frame.hpp"

namespace md {

class MergedROI;

class ROIPrioritizer {
 public:
  static std::vector<MergedROI*> order(const MultiStream& mergedROIs, int fullFrameVid);
};

} // namespace md

#endif // ROI_PRIORITIZER_HPP_
