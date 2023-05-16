#include "mondrian/ROIPrioritizer.hpp"

#include <map>
#include <set>

#include "mondrian/Frame.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"

namespace md {

bool MergedROIComp::operator()(const MergedROI* m0, const MergedROI* m1) const {
  return m0->priority() > m1->priority();
}

struct ROIComp {
  bool operator()(const ROI* r0, const ROI* r1) const {
    return r0->frame->frameIndex < r1->frame->frameIndex;
  }
};

std::vector<MergedROI*> ROIPrioritizer::sort(const std::vector<MergedROI*>& mergedROIs) {
  std::map<std::pair<int, int>, std::set<ROI*, ROIComp>> roiStreams;
  for (auto* mergedROI: mergedROIs) {
    for (auto* roi: mergedROI->rois()) {
      roiStreams[{roi->frame->vid, roi->id}].insert(roi);
    }
  }

  for (const auto&[vfid, rois]: roiStreams) {
    assert(std::all_of(rois.begin(), rois.end(),
                       [](const ROI* roi) { return roi->priority == ROI::INVALID_PRIORITY; }));
    ROI* firstROI = *rois.begin();
    if (firstROI->prevROI != nullptr && firstROI->prevROI->priority == 0) {
      for (auto* roi: rois) {
        roi->priority = roi->prevROI->priority + 1;
      }
    }
    ROI* lastROI = *rois.rbegin();
    if (lastROI->nextROI != nullptr && lastROI->nextROI->priority == 0) {
      lastROI->priority = 1;
    } else {
      lastROI->priority = 100000;
    }
    for (auto it = rois.rbegin(); it != rois.rend(); it++) {
      ROI* roi = *it;
      roi->priority = std::min(roi->priority, roi->nextROI->priority + 1);
    }
  }
  for (auto mergedROI : mergedROIs) {
    mergedROI->syncPriority();
  }

  std::set<MergedROI*, MergedROIComp> remainingMergedROIs;
  remainingMergedROIs.insert(mergedROIs.begin(), mergedROIs.end());
  std::vector<MergedROI*> sortedMergedROIs;
  sortedMergedROIs.reserve(mergedROIs.size());

  while (!remainingMergedROIs.empty()) {
    MergedROI* highestPriorityMergedROI = *remainingMergedROIs.begin();
    remainingMergedROIs.erase(remainingMergedROIs.begin());
    sortedMergedROIs.push_back(highestPriorityMergedROI);

    std::vector<MergedROI*> mergedROIsToUpdate;
    auto& highestROIs = highestPriorityMergedROI->rois();
    for (auto* highestROI: highestROIs) {
      highestROI->priority = 0;
      auto& roiStream = roiStreams[{highestROI->frame->vid, highestROI->id}];
      bool start = false;
      for (auto roi : roiStream) {
        if (roi == highestROI) {
          start = true;
          continue;
        }
        if (start) {
          roi->priority = std::min(roi->priority, roi->prevROI->priority + 1);
        }
      }
      start = false;
      for (auto it = roiStream.rbegin(); it != roiStream.rend(); it++) {
        ROI* roi = *it;
        if (roi == highestROI) {
          start = true;
          continue;
        }
        if (start) {
          roi->priority = std::min(roi->priority, roi->nextROI->priority + 1);
        }
      }
      for (auto roi: roiStream) {
        mergedROIsToUpdate.push_back(roi->mergedROI);
      }
    }

    for (auto* mergedROI : mergedROIsToUpdate) {
      remainingMergedROIs.erase(mergedROI);
      mergedROI->syncPriority();
      remainingMergedROIs.insert(mergedROI);
    }
  }
  return sortedMergedROIs;
}

} // namespace md
