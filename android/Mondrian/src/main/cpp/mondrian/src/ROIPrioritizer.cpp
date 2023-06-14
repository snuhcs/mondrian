#include "mondrian/ROIPrioritizer.hpp"

#include <map>
#include <set>

#include "mondrian/Frame.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"

namespace md {

std::vector<MergedROI*> ROIPrioritizer::order(const MultiStream& packedFrames, int fullFrameVid) {
  // roiMap[vid][roiID][frameIndex] = roi
  std::map<int, std::map<int, std::map<int, ROI*>>> roiMap;
  for (const auto& [vid, frames]: packedFrames) {
    for (Frame* frame: frames) {
      // Skip last frame for not full frame vid.
      // Last frame of full frame vid is already excluded.
      if (vid != fullFrameVid && frame == *frames.rbegin()) {
        continue;
      }
      for (auto& roi: frame->rois) {
        roiMap[vid][roi->id][frame->frameIndex] = roi.get();
      }
    }
  }

  std::set<StartEndLength> startEndLengths;
  for (const auto& [vid, roiIDMap]: roiMap) {
    for (const auto& [roiID, frameIndexMap]: roiIDMap) {
      int start = (*frameIndexMap.begin()).first;
      int end = (*frameIndexMap.rbegin()).first + 1;
      startEndLengths.emplace(vid, roiID, start, end);
    }
  }

  std::vector<MergedROI*> orderedMergedROIs;
  while (!startEndLengths.empty()) {
    auto longest = startEndLengths.begin();
    int mid = longest->mid();

    MergedROI* mergedROI = roiMap[longest->vid_][longest->roiID_][mid]->mergedROI;
    orderedMergedROIs.push_back(mergedROI);

    for (auto& roi: mergedROI->rois()) {
      for (auto it = startEndLengths.begin(); it != startEndLengths.end(); it++) {
        if (it->contains(roi->frame->vid, roi->id, mid)) {
          StartEndLength newStartEndLength1(it->vid_, it->roiID_, it->start_, mid);
          StartEndLength newStartEndLength2(it->vid_, it->roiID_, mid + 1, it->end_);
          startEndLengths.erase(it);
          if (newStartEndLength1.length_ > 0) {
            startEndLengths.insert(newStartEndLength1);
          }
          if (newStartEndLength2.length_ > 0) {
            startEndLengths.insert(newStartEndLength2);
          }
          break;
        }
      }
    }
  }

  return orderedMergedROIs;
}

} // namespace md
