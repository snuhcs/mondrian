#include "strm/PatchMixer.hpp"

#define STB_RECT_PACK_IMPLEMENTATION

#include "stb/stb_rect_pack.h"

namespace rm {

std::vector<MixedFrame> PatchMixer::pack(const std::map<std::string, SortedFrames>& frames,
                                         const Frame* fullFrameTarget,
                                         int mixedFrameSize, int numMixedFrames,
                                         bool probing) {
  // Collect RoIs. Later frame RoIs come first.
  std::vector<RoI*> packingCandidates;
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
      for (RoI& childRoI : (*it.second.rbegin())->childRoIs) {
        for (int i = 0; i < 2 * probeRoINum + 1; i++) {
          childRoI.roisForProbing.emplace_back(nullptr, childRoI.id, childRoI.frame,
                                               childRoI.location, childRoI.type, childRoI.label,
                                               childRoI.features.shift, childRoI.features.err,
                                               childRoI.features.diffAreaRatio, true);
        }
        int probe = -probeStep * probeRoINum;
        for (RoI& probeRoI : childRoI.roisForProbing) {
          numProbes++;
          probeRoI.targetSize = childRoI.targetSize + probe;
          probe += probeStep;
          probeRoI.priority = HIGH_PRIORITY;
          packingCandidates.push_back(&probeRoI);
        }
        std::sort(childRoI.roisForProbing.begin(), childRoI.roisForProbing.end());
      }
    }
  }

  // 3. Set priority & sort packingCandidates
  int numParentRoIs = 0;
  std::map<idType, std::vector<RoI*>> roiStreams;
  for (const auto& it : frames) {
    for (Frame* frame : it.second) {
      if (frame == fullFrameTarget) {
        continue;
      }
      for (auto& pRoI : frame->parentRoIs) {
        numParentRoIs++;
        roiStreams[pRoI->id].push_back(pRoI.get());
        if (pRoI->prevRoI != nullptr) {
          std::pair<int, int> shiftDiff{
              pRoI->features.shift.first - pRoI->prevRoI->features.shift.first,
              pRoI->features.shift.second - pRoI->prevRoI->features.shift.second};
          pRoI->priority = pRoI->features.err + (float) (shiftDiff.first * shiftDiff.first +
                                                         shiftDiff.second * shiftDiff.second);
        } else {
          pRoI->priority = HIGH_PRIORITY;
        }
        packingCandidates.push_back(pRoI.get());
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
    for (auto& pRoI : (*it.second.rbegin())->parentRoIs) {
      numLastFrameRoIs++;
      pRoI->priority = HIGH_PRIORITY;
    }
  }
  std::sort(packingCandidates.begin(), packingCandidates.end(),
            [](const RoI* l, const RoI* r) { return l->priority > r->priority; });

  int numCandidates = (int) packingCandidates.size();
  int numPackedRoIs = 0;
  std::vector<MixedFrame> mixedFrames;
  time_us mixingStartTime = NowMicros();
  while (true) {
    // Packing
    tryPackRoIs(packingCandidates, mixedFrameSize);

    // Divide rois with packing success and failure
    std::set<RoI*> packedRoIs;
    for (auto it = packingCandidates.begin(); it != packingCandidates.end();) {
      if ((*it)->isPacked()) {
        numPackedRoIs++;
        (*it)->packedMixedFrameIndex = (int) mixedFrames.size();
        packedRoIs.insert(*it);
        it = packingCandidates.erase(it);
      } else {
        it++;
      }
    }
    mixedFrames.emplace_back(packedRoIs, mixedFrameSize);
    if (packingCandidates.empty() || mixedFrames.size() == numMixedFrames) {
      break;
    }
  }
  time_us mixingEndTime = NowMicros();

  for (auto& it : frames) {
    for (Frame* frame : it.second) {
      frame->mixingStartTime = mixingStartTime;
      frame->mixingEndTime = mixingEndTime;
    }
  }
  LOGD("PatchMixer::pack(%d) took %lu us  // %4d / %4d packed | %2d from lastFrame | %2d Probes",
       numMixedFrames, mixingEndTime - mixingStartTime, numPackedRoIs, numCandidates,
       numLastFrameRoIs, numProbes);
  return mixedFrames;
}

void PatchMixer::tryPackRoIs(std::vector<RoI*>& parentRoIs, int mixedFrameSize) {
  std::vector<Rect> freeRects{Rect(0, 0, mixedFrameSize, mixedFrameSize)};
  for (RoI* pRoI : parentRoIs) {
    std::pair<int, int> wh = pRoI->getResizedWidthHeight();
    for (auto it = freeRects.begin(); it != freeRects.end(); it++) {
      const Rect freeRect = *it;
      if (canFit(wh, freeRect)) {
        freeRects.erase(it);
        pRoI->packedLocation = std::make_pair(freeRect.left, freeRect.top);
        std::pair<Rect, Rect> newFreeRectPair = splitFreeRect(wh, freeRect);
        freeRects.push_back(newFreeRectPair.first);
        freeRects.push_back(newFreeRectPair.second);
        break;
      }
    }
  }

//  int num_nodes = mixedFrameSize;
//  auto* context = new stbrp_context;
//  auto* nodes = new stbrp_node[num_nodes];
//  stbrp_init_target(context, mixedFrameSize, mixedFrameSize, nodes, num_nodes);
//
//  int num_rects = (int) parentRoIs.size();
//  auto* rects = new stbrp_rect[num_rects];
//
//  for (int i = 0; i < num_rects; i++) {
//    rects[i].id = (int) parentRoIs[i]->id;
//    auto wh = parentRoIs[i]->getResizedWidthHeight();
//    rects[i].w = wh.first;
//    rects[i].h = wh.second;
//  }
//
//  int is_all_packed = stbrp_pack_rects(context, rects, num_rects);
//
//  for (int i = 0; i < num_rects; i++) {
//    if (rects[i].was_packed) {
//      parentRoIs[i]->packedLocation = std::make_pair(rects[i].x, rects[i].y);
//    }
//  }
//
//  delete[] rects;
//  delete[] nodes;
//  delete context;
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
