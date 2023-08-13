#include "mondrian/ROIPrioritizer.hpp"

#include <map>
#include <numeric>
#include <set>

#include "mondrian/Frame.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"

namespace md {

std::vector<MergedROI*> ROIPrioritizer::order(const MultiStream& streams,
                                              const ROIPackerType type) {
  switch (type) {
    case MIN_CONSECUTIVE_DROP:return minConsecutiveDrop(streams);
    case OF_CONFIDENCE:return ofConfidence(streams);
    default:assert(false);
  }
}

std::vector<MergedROI*> ROIPrioritizer::minConsecutiveDrop(const MultiStream& streams) {
  // roiMap[vid, roiID][frameIndex] = roi
  std::map<std::pair<int, int>, std::map<int, ROI*>> roiMap;
  for (const auto& [vid, stream] : streams) {
    for (Frame* frame : stream) {
      // Skip last frame
      if (frame == *stream.rbegin()) {
        continue;
      }
      for (auto& roi : frame->rois) {
        roiMap[{vid, roi->id}][frame->frameIndex] = roi.get();
      }
    }
  }

  std::set<StartEndLength> startEndLengths;
  for (const auto& [key, frameIndexMap] : roiMap) {
    auto& [vid, roiID] = key;
    int start = (*frameIndexMap.begin()).first;
    int end = (*frameIndexMap.rbegin()).first + 1;
    startEndLengths.emplace(vid, roiID, start, end);
  }
  assert(roiMap.size() == startEndLengths.size());

  std::vector<MergedROI*> orderedMergedROIs;
  while (!startEndLengths.empty()) {
    auto longest = startEndLengths.begin();
    int vid = longest->vid_;
    int fid = longest->mid();

    MergedROI* mergedROI = roiMap[{vid, longest->roiID_}][fid]->mergedROI;
    assert(std::find(orderedMergedROIs.begin(), orderedMergedROIs.end(),
                     mergedROI) == orderedMergedROIs.end());
    orderedMergedROIs.push_back(mergedROI);

    for (auto& roi : mergedROI->rois()) {
      auto it = startEndLengths.begin();
      for (; it != startEndLengths.end(); it++) {
        if (it->vid_ == vid && it->roiID_ == roi->id && it->start_ <= fid && fid < it->end_)
          break;
      }
      assert(it != startEndLengths.end());
      StartEndLength newStartEndLength1(it->vid_, it->roiID_, it->start_, fid);
      StartEndLength newStartEndLength2(it->vid_, it->roiID_, fid + 1, it->end_);
      startEndLengths.erase(it);
      if (newStartEndLength1.length_ > 0) {
        startEndLengths.insert(newStartEndLength1);
      }
      if (newStartEndLength2.length_ > 0) {
        startEndLengths.insert(newStartEndLength2);
      }
    }
  }

  return orderedMergedROIs;
}

std::vector<MergedROI*> ROIPrioritizer::ofConfidence(const MultiStream& streams) {
  auto priorityOf = [](const MergedROI* mergedROI) -> float {
    float mergedROIPriority = 0.0f;
    for (const auto& roi : mergedROI->rois()) {
      mergedROIPriority = mergedROIPriority
          + roi->features.ofFeatures.shiftNcc
          + roi->features.ofFeatures.avgErr;
    }
    return mergedROIPriority;
  };

  priorityOf(nullptr); // To suppress unused warning.

  std::set<MergedROI*, MergedROIoFPriorityComparator> orderedMergedROIs;
  for (const auto& [vid, stream] : streams) {
    for (Frame* frame : stream) {
      // Skip last frame
      if (frame == *stream.rbegin()) {
        continue;
      }
      for (auto& roi : frame->rois) {
        orderedMergedROIs.insert(roi->mergedROI);
      }
    }
  }
  std::vector<MergedROI*> mergedROIs(orderedMergedROIs.begin(), orderedMergedROIs.end());
  return mergedROIs;
}

} // namespace md
