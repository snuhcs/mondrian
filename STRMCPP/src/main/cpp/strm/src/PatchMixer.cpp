#include "strm/PatchMixer.hpp"

namespace rm {

int PatchMixer::mMixedFrameIndex = 0;

std::vector<MixedFrame> PatchMixer::pack(const std::map<std::string, SortedFrames>& frames,
                                         const Frame* fullFrameTarget,
                                         int mixedFrameSize, int numMixedFrames,
                                         bool probing) {
  // Collect RoIs. Later frame RoIs come first.
  std::vector<RoI*> rois;
  const float HIGH_PRIORITY = 1e9;

  // 1. Insert probe RoIs
  int numProbes = 0;
  if (probing) {
    int probeStep = 4;
    int probeRoINum = 1; // total 2 * probeRoINum + 1 number of probeRoIs
    for (auto it : frames) {
      if (it.second.empty() || (fullFrameTarget != nullptr && fullFrameTarget->key == it.first)) {
        continue;
      }
      for (RoI& roi : (*it.second.rbegin())->childRoIs) {
        for (int i = 0; i < 2 * probeRoINum + 1; i++) {
          roi.roisForProbing.emplace_back(nullptr, roi.id, roi.frame, roi.location, roi.type,
                                          roi.label,
                                          roi.features.shift, roi.features.err,
                                          roi.features.diffAreaRatio);
        }
        int probe = -probeStep * probeRoINum;
        for (RoI& probeRoI : roi.roisForProbing) {
          numProbes++;
          probeRoI.targetSize = roi.targetSize + probe;
          probe += probeStep;
          probeRoI.priority = HIGH_PRIORITY;
          rois.push_back(&probeRoI);
        }
        std::sort(roi.roisForProbing.begin(), roi.roisForProbing.end());
      }
    }
  }

  // 3. Set priority & sort rois
  int numRoIs = 0;
  std::map<idType, std::vector<RoI*>> roiStreams;
  for (const auto& it : frames) {
    for (Frame* frame : it.second) {
      for (RoI& roi : frame->parentRoIs) {
        numRoIs++;
        roiStreams[roi.id].push_back(&roi);
        if (roi.prevRoI != nullptr) {
          std::pair<int, int> shiftDiff{roi.features.shift.first - roi.prevRoI->features.shift.first,
                                        roi.features.shift.second - roi.prevRoI->features.shift.second};
          roi.priority = roi.features.err + (float) (shiftDiff.first * shiftDiff.first + shiftDiff.second * shiftDiff.second);
        } else {
          roi.priority = HIGH_PRIORITY;
        }
        rois.push_back(&roi);
      }
    }
  }

  // insert lastFrames first
  int numLastFrameRoIs = 0;
  SortedFrames lastFrames;
  for (const auto& it : frames) {
    if (it.second.empty() || (fullFrameTarget != nullptr && fullFrameTarget->key == it.first)) {
      continue;
    }
    lastFrames.insert(*it.second.rbegin());
    for (RoI& roi : (*it.second.rbegin())->parentRoIs) {
      numLastFrameRoIs++;
      roi.priority = HIGH_PRIORITY;
    }
  }
  std::sort(rois.begin(), rois.end(), [](const RoI* l, const RoI* r) { return l->priority > r->priority; });

  std::vector<RoI*> droppedRoIs;
  std::vector<std::set<RoI*>> packedRoIs;
  std::vector<std::vector<Rect>> freeRectsList;
  for (int i = 0; i < numMixedFrames; i++) {
    freeRectsList.push_back({Rect(0, 0, mixedFrameSize, mixedFrameSize)});
    packedRoIs.emplace_back();
  }
  time_us mixingStartTime = NowMicros();
  for (RoI* roi : rois) {
    tryPackRoI(roi, freeRectsList, packedRoIs, droppedRoIs);
  }
  time_us mixingEndTime = NowMicros();

  if (false) {
    for (int i = 0; i < numMixedFrames; i++) {
      std::stringstream ss;
      ss << "packedRoIs[" << i << "]: ";
      for (RoI* roi : packedRoIs[i]) {
        if (lastFrames.find(roi->frame) != lastFrames.end()) {
          ss << "(" << roi->frame->shortKey << ","
             << roi->frame->frameIndex << ","
             << roi->id << ") ";
        }
      }
      LOGD("%s", ss.str().c_str());
      std::stringstream ss2;
      ss2 << "droppedRoIs[" << i << "]: ";
      for (RoI* roi : droppedRoIs) {
        if (lastFrames.find(roi->frame) != lastFrames.end()) {
          ss2 << "(" << roi->frame->shortKey << ","
              << roi->frame->frameIndex << ","
              << roi->id << ") ";
        }
      }
      LOGD("%s", ss2.str().c_str());
    }

    std::stringstream ss;
    ss << "lastRoIs: ";
    for (Frame* lastFrame : lastFrames) {
      for (RoI& roi : lastFrame->parentRoIs) {
        ss << roi.id << ", ";
      }
    }
    LOGD("%s", ss.str().c_str());
  }
  LOGD("%lu rois %d parentRoIs %d probes %lu droppedRoIs %d lastFrameRoIs", rois.size(), numRoIs,
       numProbes, droppedRoIs.size(), numLastFrameRoIs);
  assert(std::all_of(frames.cbegin(), frames.cend(),
                     [&fullFrameTarget](const std::pair<std::string, SortedFrames>& it) {
                       return (fullFrameTarget != nullptr && fullFrameTarget->key == it.first) ||
                              it.second.empty() || (*it.second.crbegin())->isAllRoIPacked();
                     }));

  time_us mixedFrameCreateStartTime = NowMicros();
  std::vector<MixedFrame> mixedFrames;
  mixedFrames.reserve(numMixedFrames);
  for (int i = 0; i < numMixedFrames; i++) {
    if (!packedRoIs[i].empty()) {
      mixedFrames.emplace_back(mMixedFrameIndex++, packedRoIs[i], mixedFrameSize);
    }
  }
  time_us mixedFrameCreateEndTime = NowMicros();

  for (auto& it : frames) {
    for (Frame* frame : it.second) {
      frame->mixingStartTime = mixingStartTime;
      frame->mixingEndTime = mixingEndTime;
      frame->mixedFrameCreateStartTime = mixedFrameCreateStartTime;
      frame->mixedFrameCreateEndTime = mixedFrameCreateEndTime;
    }
  }
  LOGD("PatchMixer::pack(%lu, %d, %d) took %lu and %lu us : %lu / %lu dropped", frames.size(),
       mixedFrameSize, numMixedFrames, mixingEndTime - mixingStartTime,
       mixedFrameCreateEndTime - mixedFrameCreateStartTime, droppedRoIs.size(), rois.size());
  return mixedFrames;
}

void PatchMixer::tryPackRoI(RoI* roi, std::vector<std::vector<Rect>>& freeRectsList,
                            std::vector<std::set<RoI*>>& packedRoIs,
                            std::vector<RoI*>& droppedRoIs) {
  std::pair<int, int> wh = roi->getResizedWidthHeight();
  for (int mixedFrameIndex = 0; mixedFrameIndex < freeRectsList.size(); mixedFrameIndex++) {
    std::vector<Rect>& freeRects = freeRectsList[mixedFrameIndex];
    for (auto it = freeRects.begin(); it != freeRects.end(); it++) {
      const Rect freeRect = *it;
      if (canFit(wh, freeRect)) {
        freeRects.erase(it);
        roi->packedLocation = std::make_pair(freeRect.left, freeRect.top);
        packedRoIs[mixedFrameIndex].insert(roi);
        std::pair<Rect, Rect> newFreeRectPair = splitFreeRect(wh, freeRect);
        freeRects.push_back(newFreeRectPair.first);
        freeRects.push_back(newFreeRectPair.second);
        break;
      }
    }
    if (roi->isPacked()) {
      break;
    }
  }
  if (!roi->isPacked()) {
    droppedRoIs.push_back(roi);
  }
}

bool PatchMixer::canFit(std::pair<int, int> wh, const Rect& rect) {
  return wh.first <= rect.width() && wh.second <= rect.height();
}

std::pair<Rect, Rect> PatchMixer::splitFreeRect(std::pair<int, int> wh, const Rect& rect) {
  int w = wh.first;
  int h = wh.second;
  if (rect.width() > rect.height()) {
    return std::make_pair(Rect(rect.left + w, rect.top, rect.right, rect.bottom),
                          Rect(rect.left, rect.top + h, rect.left + w, rect.bottom));
  } else {
    return std::make_pair(Rect(rect.left, rect.top + h, rect.right, rect.bottom),
                          Rect(rect.left + w, rect.top, rect.right, rect.top + h));
  }
}

} // namespace rm
