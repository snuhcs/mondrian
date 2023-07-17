#include "mondrian/ROIPrioritizer.hpp"

#include <map>
#include <set>

#include "mondrian/Frame.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/Order.hpp"
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

  std::map<std::pair<int, int>, int> roiStreamLengths;
  for (const auto& [vid, roiIDMap]: roiMap) {
    for (const auto& [roiID, frameMap]: roiIDMap) {
      roiStreamLengths[{vid, roiID}] = int(frameMap.size());
    }
  }

  std::vector<MergedROI*> orderedMergedROIs;
  for (auto& [vid, roiIDMap]: roiMap) {
    int frameLength = int(packedFrames.at(vid).size());
    int startIndex = (*packedFrames.at(vid).begin())->frameIndex;
    LOGD("XXX frameLength: %d startIndex: %d", frameLength, startIndex);
    for (int i = 0; i < frameLength; i++) {
      LOGD("XXX ===== i=%d =====", i);
      for (auto& [roiID, frameMap]: roiIDMap) {
        int roiLength = roiStreamLengths[{vid, roiID}];
        if (roiLength <= i) {
          continue;
        }
        int packIndex = startIndex + ORDERS.at(roiLength)[i];
        auto it = frameMap.find(packIndex);
        std::stringstream ss;
        ss << "selected: roi=" << roiID << ", frame=" << packIndex;
        if (it != frameMap.end()) {
          MergedROI* mergedROI = (*it).second->mergedROI;
          orderedMergedROIs.push_back(mergedROI);
          ss << " | removes ";
          for (const auto& roi: mergedROI->rois()) {
            assert(roi->frame->frameIndex == packIndex);
            roiIDMap[roi->id].erase(packIndex);
            ss << "(" << roi->id << ", " << roi->frame->frameIndex << "), ";
          }
        } else {
          ss << " | already removed";
        }
        LOGD("XXX %s", ss.str().c_str());
      }
    }
    for (auto& [roiID, frameMap]: roiIDMap) {
      assert(frameMap.empty());
    }
  }

  return orderedMergedROIs;
}

} // namespace md
