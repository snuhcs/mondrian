#include "mondrian/ROIPrioritizer.hpp"

#include <map>
#include <set>

#include "mondrian/Frame.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"

namespace md {

std::vector<MergedROI*> ROIPrioritizer::order(const MultiStream& packedFrames, int fullFrameVid) {
  // roiMap[vid, roiID][frameIndex] = roi
  std::map<std::pair<int, int>, std::map<int, ROI*>> roiMap;
  for (const auto& [vid, frames]: packedFrames) {
    for (Frame* frame: frames) {
      // Skip last frame for not full frame vid.
      // Last frame of full frame vid is already excluded.
      if (vid != fullFrameVid && frame == *frames.rbegin()) {
        continue;
      }
      for (auto& roi: frame->rois) {
        roiMap[{vid, roi->id}][frame->frameIndex] = roi.get();
      }
    }
  }

  std::set<StartEndLength> startEndLengths;
  for (const auto& [key, frameIndexMap]: roiMap) {
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

    for (auto& roi: mergedROI->rois()) {
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

} // namespace md
