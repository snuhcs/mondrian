#ifndef ROI_PRIORITIZER_HPP_
#define ROI_PRIORITIZER_HPP_

#include <set>
#include <vector>

namespace md {

class MergedROI;

struct MergedROIComp {
  bool operator()(const MergedROI* m0, const MergedROI* m1) const;
};

class ROIPrioritizer {
 public:
  static std::vector<MergedROI*> sort(const std::vector<MergedROI*>& mergedROIs);
};

} // namespace md

#endif // ROI_PRIORITIZER_HPP_
