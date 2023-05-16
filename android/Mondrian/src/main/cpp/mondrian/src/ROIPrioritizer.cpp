#include "mondrian/ROIPrioritizer.hpp"

#include <map>
#include <set>

#include "mondrian/Frame.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"

namespace md {

struct MergedROIComp {
  bool operator()(const MergedROI* m0, const MergedROI* m1) const {
    return m0->priority() < m1->priority();
  }
};

std::vector<MergedROI*> ROIPrioritizer::sort(const std::vector<MergedROI*>& mergedROIs) {
  auto priorityOf = [](const ROI* roi) -> float {
    return roi->maxEdgeLength / 10.0f
           + roi->features.ofFeatures.shiftNcc
           + roi->features.ofFeatures.avgErr;
  };

  for (auto& mergedROI : mergedROIs) {
    float priority = 0;
    for (auto& roi : mergedROI->rois()) {
      priority += priorityOf(roi);
    }
    mergedROI->setPriority(priority);
  }
  std::vector<MergedROI*> sortedMergedROI(mergedROIs);
  std::sort(sortedMergedROI.begin(), sortedMergedROI.end(), MergedROIComp());
  return sortedMergedROI;
}

} // namespace md
