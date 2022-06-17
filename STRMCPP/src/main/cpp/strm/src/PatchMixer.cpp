#include "strm/PatchMixer.hpp"

namespace rm {

int PatchMixer::mMixedFrameIndex = 0;

std::vector<MixedFrame> PatchMixer::pack(const std::set<Frame*>& frames,
                                         const std::set<Frame*>& lastFrames,
                                         int mixedFrameSize, int numMixedFrames) {
  // Collect RoIs. Later frame RoIs come first.
  std::vector<RoI*> rois;

  // 1. Insert probe RoIs
  int probeStep = 4;
  int probeRoINum = 1; // total 2 * probeRoINum + 1 number of probeRoIs
  for (auto lastFrame : lastFrames) {
    for (RoI& roi : lastFrame->origRoIs) {
      for (int i = 0; i < 2 * probeRoINum + 1; i++) {
        roi.roisForProbing.emplace_back(roi.id, roi.frame, roi.location, roi.type, roi.labelName,
                                        roi.features.shift, roi.features.err,
                                        roi.features.diffAreaRatio);
      }
      int probe = -probeStep * probeRoINum;
      for (RoI& probeRoI : roi.roisForProbing) {
        probeRoI.targetSize = roi.targetSize + probe;
        probe += probeStep;
        rois.push_back(&probeRoI);
      }
      std::sort(roi.roisForProbing.begin(), roi.roisForProbing.end());
    }
  }
  // 2. Insert last frame RoIs
  for (Frame* frame : lastFrames) {
    for (RoI& roi : frame->rois) {
      rois.push_back(&roi);
    }
  }
  // 3. Insert others
  for (Frame* frame : frames) {
    if (lastFrames.find(frame) == lastFrames.cend()) {
      for (RoI& roi : frame->rois) {
        rois.push_back(&roi);
      }
    }
  }

  std::vector<RoI*> droppedRoIs;
  std::map<int, std::vector<RoI*>> packedRoIs;
  std::map<int, std::vector<Rect>> freeRectsMap;
  for (int i = 0; i < numMixedFrames; i++) {
    freeRectsMap[i].emplace_back(0, 0, mixedFrameSize, mixedFrameSize);
  }
  time_us mixingStartTime = NowMicros();
  for (RoI* roi : rois) {
    tryPackRoI(roi, freeRectsMap, packedRoIs, droppedRoIs);
  }
  time_us mixingEndTime = NowMicros();
  assert(std::all_of(lastFrames.cbegin(), lastFrames.cend(),
                     [](const Frame* frame) { return frame->isAllRoIPacked(); }));

  time_us mixedFrameCreateStartTime = NowMicros();
  std::vector<MixedFrame> mixedFrames;
  mixedFrames.reserve(numMixedFrames);
  for (int i = 0; i < numMixedFrames; i++) {
    mixedFrames.emplace_back(mMixedFrameIndex++, packedRoIs[i], mixedFrameSize);
  }
  time_us mixedFrameCreateEndTime = NowMicros();

  for (auto& frame : frames) {
    frame->mat.release();
    frame->mixingStartTime = mixingStartTime;
    frame->mixingEndTime = mixingEndTime;
    frame->mixedFrameCreateStartTime = mixedFrameCreateStartTime;
    frame->mixedFrameCreateEndTime = mixedFrameCreateEndTime;
  }
  LOGD("PatchMixer::pack(%lu, %d, %d) took %lu and %lu us : %lu / %lu dropped", frames.size(),
       mixedFrameSize, numMixedFrames, mixingEndTime - mixingStartTime,
       mixedFrameCreateEndTime - mixedFrameCreateStartTime, rois.size(), droppedRoIs.size());
  return mixedFrames;
}

void PatchMixer::tryPackRoI(RoI* roi,
                            std::map<int, std::vector<Rect>>& freeRectsMap,
                            std::map<int, std::vector<RoI*>>& packedRoIs,
                            std::vector<RoI*>& droppedRoIs) {
  std::pair<int, int> wh = roi->getResizedWidthHeight();
  for (auto& indexAndFreeRects : freeRectsMap) {
    int index = indexAndFreeRects.first;
    std::vector<Rect>& freeRects = indexAndFreeRects.second;
    for (auto it = freeRects.begin(); it != freeRects.end(); it++) {
      const Rect freeRect = *it;
      if (canFit(wh, freeRect)) {
        freeRects.erase(it);
        roi->packedLocation = std::make_pair(freeRect.left, freeRect.top);
        packedRoIs[index].push_back(roi);
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
