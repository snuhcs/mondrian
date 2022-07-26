#include "strm/PatchMixer.hpp"

#define STB_RECT_PACK_IMPLEMENTATION

#include "stb/stb_rect_pack.h"

#include "strm/Test.hpp"

namespace rm {

void PatchMixer::preparePack(std::map<std::string, SortedFrames>& frames,
                             ResizeProfile* resizeProfile, int maxRoISize, float mergeThreshold) {
  time_us resizeStartTime = NowMicros();
  for (auto& it : frames) {
    for (Frame* frame : it.second) {
      for (auto& cRoI : frame->childRoIs) {
        cRoI->targetSize = std::min(cRoI->maxEdgeLength,
                                    resizeProfile->getTargetSize(cRoI->id, cRoI->features));
      }
    }
  }
  time_us resizeEndTime = NowMicros();

  time_us mergeStartTime = NowMicros();
  for (auto& it : frames) {
    for (Frame* frame : it.second) {
      mergeRoIs(frame->childRoIs, frame->parentRoIs, maxRoISize, mergeThreshold);
    }
  }

  for (auto& it : frames) {
    for (Frame* frame : it.second) {
      testAssignedUniqueRoIID(frame->childRoIs);
      testParentChildrenIDsAndChildIDsSame(frame->childRoIs, frame->parentRoIs);
      testChildRoIsFrameRelation(frame->childRoIs);
      testParentRoIsFrameRelation(frame->parentRoIs);
    }
  }
  time_us mergeEndTime = NowMicros();

  for (auto& it : frames) {
    for (Frame* frame : it.second) {
      frame->resizeStartTime = resizeStartTime;
      frame->resizeEndTime = resizeEndTime;
      frame->mergeRoIStartTime = mergeStartTime;
      frame->mergeRoIEndTime = mergeEndTime;
    }
  }
}

void PatchMixer::mergeRoIs(std::vector<std::unique_ptr<RoI>>& childRoIs,
                           std::vector<std::unique_ptr<RoI>>& parentRoIs,
                           int maxSize, float mergeThreshold) {
  assert(parentRoIs.empty());
  for (auto& cRoI : childRoIs) {
    std::unique_ptr<RoI> pRoI = std::make_unique<RoI>(*cRoI);
    assert(cRoI->parentRoI == nullptr && pRoI->parentRoI == nullptr);
    assert(cRoI->childRoIs.empty() && pRoI->childRoIs.empty());
    assert(cRoI->roisForProbing.empty() && pRoI->roisForProbing.empty());
    cRoI->parentRoI = pRoI.get();
    pRoI->childRoIs.push_back(cRoI.get());
    parentRoIs.push_back(std::move(pRoI));
  }

  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < parentRoIs.size(); i++) {
      for (j = i + 1; j < parentRoIs.size(); j++) {
        const std::unique_ptr<RoI>& roi0 = parentRoIs[i];
        const std::unique_ptr<RoI>& roi1 = parentRoIs[j];
        int intersection = roi0->location.intersection(roi1->location);
        if ((float) intersection / (float) roi0->getArea() < mergeThreshold &&
            (float) intersection / (float) roi1->getArea() < mergeThreshold) {
          continue;
        }
        int newLeft = std::min(roi0->location.left, roi1->location.left);
        int newTop = std::min(roi0->location.top, roi1->location.top);
        int newRight = std::max(roi0->location.right, roi1->location.right);
        int newBottom = std::max(roi0->location.bottom, roi1->location.bottom);
        if (newRight - newLeft > maxSize || newBottom - newTop > maxSize) {
          continue;
        }
        int newArea = (newRight - newLeft) * (newBottom - newLeft);
        if (roi0->targetSize * roi1->maxEdgeLength > roi1->targetSize * roi0->maxEdgeLength) {
          // If roi0 resizes conservatively than roi1
          newArea = newArea * roi0->targetSize * roi0->targetSize
                    / roi0->maxEdgeLength / roi0->maxEdgeLength;
        } else {
          // If roi1 resizes conservatively than roi0
          newArea = newArea * roi1->targetSize * roi1->targetSize
                    / roi1->maxEdgeLength / roi1->maxEdgeLength;
        }
        int originalArea = roi0->getResizedArea() + roi1->getResizedArea();
        if (newArea >= originalArea) {
          continue;
        }
        updated = true;
        break;
      }
      if (updated) {
        break;
      }
    }
    if (!updated) {
      break;
    }
    parentRoIs.push_back(std::move(RoI::mergeRoIs(parentRoIs[i].get(), parentRoIs[j].get())));
    // Match child parent
    RoI* mergedRoI = parentRoIs.rbegin()->get();
    mergedRoI->childRoIs.insert(mergedRoI->childRoIs.end(),
                                parentRoIs[i]->childRoIs.begin(), parentRoIs[i]->childRoIs.end());
    mergedRoI->childRoIs.insert(mergedRoI->childRoIs.end(),
                                parentRoIs[j]->childRoIs.begin(), parentRoIs[j]->childRoIs.end());
    for (RoI* cRoI : mergedRoI->childRoIs) {
      cRoI->parentRoI = mergedRoI;
    }
    assert(j > i);
    parentRoIs.erase(parentRoIs.begin() + j);
    parentRoIs.erase(parentRoIs.begin() + i);
  }
}

std::vector<MixedFrame> PatchMixer::pack(std::map<std::string, SortedFrames>& frames,
                                         const Frame* fullFrameTarget,
                                         int mixedFrameSize, int numMixedFrames,
                                         bool probing, const int probeStep) {
  // Collect RoIs. Later frame RoIs come first.
  std::vector<RoI*> packingCandidates;
  const float HIGH_PRIORITY = 1e9;

  // 1. Insert probe RoIs
  int numProbes = 0;
  if (probing) {
    assert(probeStep > 0);
    int numProbSteps = 1; // total 2 * numProbSteps + 1 number of probeRoIs
    for (auto it : frames) {
      if (it.second.empty() || (fullFrameTarget != nullptr && fullFrameTarget->key == it.first)) {
        continue;
      }
      Frame* lastFrame = *it.second.rbegin();
      for (auto& cRoI : lastFrame->childRoIs) {
        int probe = -probeStep * numProbSteps;
        for (int i = 0; i < 2 * numProbSteps + 1; i++) {
          cRoI->frame->probingRoIs.emplace_back(
              new RoI(nullptr, cRoI->id, cRoI->frame, cRoI->location, cRoI->type, cRoI->origin, cRoI->label,
                      cRoI->features.shift, cRoI->features.err, cRoI->features.diffAreaRatio,
                      true));
          RoI* probeRoI = cRoI->frame->probingRoIs.back().get();
          probeRoI->targetSize = cRoI->targetSize + probe;
          probeRoI->priority = HIGH_PRIORITY;
          cRoI->roisForProbing.push_back(probeRoI);
          packingCandidates.push_back(probeRoI);
          probe += probeStep;
          numProbes++;
        }
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
