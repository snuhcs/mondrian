#include "strm/PatchMixer.hpp"

#include <numeric>

#include "strm/Test.hpp"

namespace rm {

const float PatchMixer::HIGHEST_PRIORITY = 103;
const float PatchMixer::HIGHER_PRIORITY = 102;
const float PatchMixer::HIGH_PRIORITY = 101;

PatchMixer::PatchMixer(const PatchMixerConfig& config)
    : mConfig(config) {}

std::vector<Frame*> PatchMixer::addProbeRoIs(MultiStream& frames, const Frame* fullFrameTarget,
                                             int numProbeSteps, float probeStepSize) {
  std::vector<Frame*> probeFrames;
  std::set<Frame*> lastFrames = filterLastFrames(frames);
  for (Frame* lastFrame : lastFrames) {
    if (lastFrame != fullFrameTarget) {
      lastFrame->addProbeRoIs(numProbeSteps, probeStepSize);
      probeFrames.push_back(lastFrame);
    }
  }
  return probeFrames;
}

std::vector<RoI*> PatchMixer::collectRoIs(MultiStream& frames, const Frame* fullFrameTarget) {
  std::vector<RoI*> packingCandidates;

  // Add probeRoIs
  std::set<Frame*> lastFrames = filterLastFrames(frames);
  for (Frame* lastFrame : lastFrames) {
    if (lastFrame != fullFrameTarget) {
      for (auto& probeRoI : lastFrame->probingRoIs) {
        packingCandidates.push_back(probeRoI.get());
      }
    }
  }

  // Add parentRoIs
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      if (frame != fullFrameTarget) {
        for (auto& pRoI : frame->parentRoIs) {
          packingCandidates.push_back(pRoI.get());
        }
      }
    }
  }
  return packingCandidates;
}

void PatchMixer::prioritizeRoIs(MultiStream& frames, const Frame* fullFrameTarget) {
  // Set priority with err and acceleration (shift difference)
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      for (auto& pRoI : frame->parentRoIs) {
        // if (pRoI->prevRoI != nullptr) {
        if (std::all_of(pRoI->childRoIs.begin(), pRoI->childRoIs.end(),
                        [](RoI* cRoI) { return cRoI->type == RoI::OF; })) {
          if (std::any_of(pRoI->childRoIs.begin(), pRoI->childRoIs.end(),
                          [](RoI* cRoI) { return cRoI->prevRoI == nullptr; })) {
            pRoI->priority = 0.1234;
            continue;
          }

          float diffNorm = 0;
          for (auto& cRoI : pRoI->childRoIs) {
            const auto&[px, py] = cRoI->prevRoI->features.ofFeatures.avgShift;
            const auto&[cx, cy] = cRoI->features.ofFeatures.avgShift;
            float diffX = cx - px;
            float diffY = cy - py;
            diffNorm += (diffX * diffX + diffY * diffY);
          }
          diffNorm /= float(pRoI->childRoIs.size());

          pRoI->priority = pRoI->features.ofFeatures.avgErr * diffNorm;
        } else {
          pRoI->priority = HIGHER_PRIORITY;
        }
      }
    }
  }

  // Set high priority for probeRoIs
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      for (auto& probeRoI : frame->probingRoIs) {
        probeRoI->priority = HIGH_PRIORITY;
      }
    }
  }

  // Set high priority for lastFrames except a full frame inference target frame
  for (auto&[aStreamKey, aStreamFrames] : frames) {
    if (aStreamFrames.empty()) {
      continue;
    }
    if (fullFrameTarget == nullptr || fullFrameTarget->key != aStreamKey) {
      Frame* lastFrame = *aStreamFrames.rbegin();
      for (auto& pRoI : lastFrame->parentRoIs) {
        pRoI->priority = HIGHEST_PRIORITY;
      }
    }
  }
}

std::tuple<std::vector<MixedFrame>, Frame*, MultiStream, Stream> PatchMixer::packRoIs(
    MultiStream& frames, int fullFrameStreamIndex, std::vector<InferenceInfo>& inferencePlan,
    bool allowInterpolation, bool roiWiseInference, bool probe, int numProbeSteps,
    float probeStepSize) {
  // TODO
  time_us mixingStartTime = NowMicros();
  std::vector<MixedFrame> mixedFrames;
  Frame* fullFrameTarget;
  MultiStream selectedFrames = frames;
  Stream droppedFrames;

  if (allowInterpolation && roiWiseInference) {
    // Preprocess: getFullFrameTarget & addProbeRoIs & collectRoIs & prioritizeRoIs
    int numFrames = int(inferencePlan.size());
    auto ret = preparePack(selectedFrames, fullFrameStreamIndex, probe, numProbeSteps,
                           probeStepSize);
    fullFrameTarget = std::get<0>(ret);
    std::vector<Frame*>& probeFrames = std::get<1>(ret);
    std::vector<RoI*>& candidateRoIs = std::get<2>(ret);
    // Prepare mixed frames for each RoI
    for (int i = 0; i < std::min(numFrames, (int) candidateRoIs.size()); i++) {
      const InferenceInfo& info = inferencePlan[i];
      auto frameSize = float(info.size);
      RoI* pRoI = candidateRoIs[i];
      pRoI->setTargetSize(frameSize);
      auto[resizedWidth, resizedHeight] = pRoI->getResizedWidthHeight();
      float x = frameSize - resizedWidth / 2;
      float y = frameSize - resizedHeight / 2;
      pRoI->packedLocation = {x, y};
      mixedFrames.push_back(MixedFrame(info.device, {pRoI}, int(frameSize)));
    }
  } else if (!allowInterpolation && roiWiseInference) {
    while (true) {
      // Preprocess: getFullFrameTarget & addProbeRoIs & collectRoIs & prioritizeRoIs
      int numFrames = int(inferencePlan.size());
      auto ret = preparePack(selectedFrames, fullFrameStreamIndex, probe, numProbeSteps,
                             probeStepSize);
      fullFrameTarget = std::get<0>(ret);
      std::vector<Frame*>& probeFrames = std::get<1>(ret);
      std::vector<RoI*>& candidateRoIs = std::get<2>(ret);
      int minFrames = fullFrameTarget == nullptr ? 1 : 2;
      int numSelectedFrames = std::accumulate(
          selectedFrames.begin(), selectedFrames.end(), 0,
          [](int cnt, auto& it) {
            return cnt + it.second.size();
          });
      LOGD("PatchMixer::packRoIs: Try pack %-4d Frames => %-4lu / %-4d RoIs can be inference",
           numSelectedFrames, candidateRoIs.size(), numFrames);
      // If packing failed
      if (candidateRoIs.size() > numFrames && numSelectedFrames > minFrames) {
        int numSelectedRoIs = std::accumulate(
            selectedFrames.begin(), selectedFrames.end(), 0,
            [](int cnt, auto& it) {
              return cnt + std::accumulate(
                  it.second.begin(), it.second.end(), 0,
                  [](int cnt, Frame* frame) { return cnt + frame->parentRoIs.size(); });
            });
        int avgRoIsPerFrame = numSelectedRoIs / numSelectedFrames;
        int numPackableFrames = std::min(numSelectedFrames - 1,
                                         std::max(minFrames, numFrames / avgRoIsPerFrame));
        splitFrames(selectedFrames, droppedFrames, numPackableFrames);
        for (Frame* frame : probeFrames) {
          frame->resetProbeRoIs();
        }
      } else { // If packing success
        for (int i = 0; i < std::min(numFrames, (int) candidateRoIs.size()); i++) {
          const InferenceInfo& info = inferencePlan[i];
          auto frameSize = float(info.size);
          RoI* pRoI = candidateRoIs[i];
          pRoI->setTargetSize(float(frameSize));
          auto[resizedWidth, resizedHeight] = pRoI->getResizedWidthHeight();
          float x = float(frameSize - resizedWidth) / 2;
          float y = float(frameSize - resizedHeight) / 2;
          pRoI->packedLocation = {x, y};
          mixedFrames.push_back(MixedFrame(info.device, {pRoI}, int(frameSize)));
        }
        break;
      }
    }
  } else if (allowInterpolation && !roiWiseInference) {
    // Preprocess: getFullFrameTarget & addProbeRoIs & collectRoIs & prioritizeRoIs
    int numFrames = int(inferencePlan.size());
    auto ret = preparePack(selectedFrames, fullFrameStreamIndex, probe, numProbeSteps,
                           probeStepSize);
    fullFrameTarget = std::get<0>(ret);
    std::vector<Frame*>& probeFrames = std::get<1>(ret);
    std::vector<RoI*>& candidateRoIs = std::get<2>(ret);
    // Init data structures
    std::map<int, std::set<RoI*>> packedRoIsMap;
    std::vector<FreeRects> freeRectsList;
    for (auto& info : inferencePlan) {
      freeRectsList.push_back(
          {info.device, info.size, {Rect(0, 0, float(info.size), float(info.size))}});
    }
    // Pack RoIs into mixed frames
    for (RoI* pRoI : candidateRoIs) {
      tryPackRoI(
          pRoI->getResizedWidthHeight(), freeRectsList,
          pRoI->priority == HIGHEST_PRIORITY || mConfig.EMULATED_BATCH || !mConfig.N_WAY_MIXING,
          pRoI, &packedRoIsMap);
    }
    for (auto&[mixedFrameIndex, aMixedFrameRoIs] : packedRoIsMap) {
      for (RoI* pRoI : aMixedFrameRoIs) {
        assert(pRoI->isPacked());
        assert(pRoI->packedMixedFrameIndex == mixedFrameIndex);
      }
    }
    for (auto&[i, aMixedFrameRoIs] : packedRoIsMap) {
      if (!aMixedFrameRoIs.empty()) {
        const InferenceInfo& info = inferencePlan[i];
        mixedFrames.emplace_back(info.device, aMixedFrameRoIs, info.size);
      }
    }
  } else if (!allowInterpolation && !roiWiseInference) {
    while (true) {
      // Preprocess: getFullFrameTarget & addProbeRoIs & collectRoIs & prioritizeRoIs
      int numFrames = int(inferencePlan.size());
      auto ret = preparePack(selectedFrames, fullFrameStreamIndex, probe, numProbeSteps,
                             probeStepSize);
      fullFrameTarget = std::get<0>(ret);
      std::vector<Frame*>& probeFrames = std::get<1>(ret);
      std::vector<RoI*>& candidateRoIs = std::get<2>(ret);
      int minFrames = fullFrameTarget == nullptr ? 1 : 2;
      // Init data structures
      std::map<int, std::set<RoI*>> packedRoIsMap;
      std::vector<FreeRects> freeRectsList;
      for (auto& info : inferencePlan) {
        freeRectsList.push_back(
            {info.device, info.size, {Rect(0, 0, float(info.size), float(info.size))}});
      }
      // Pack RoIs
      int numPackedChildRoIs = 0;
      int numPackedProbeRoIs = 0;
      bool isAllPacked = true;
      for (RoI* pRoI : candidateRoIs) {
        if (!tryPackRoI(
            pRoI->getResizedWidthHeight(), freeRectsList,
            pRoI->priority == HIGHEST_PRIORITY || mConfig.EMULATED_BATCH || !mConfig.N_WAY_MIXING,
            pRoI, &packedRoIsMap)) {
          isAllPacked = false;
          break;
        } else {
          if (pRoI->isProbingRoI) {
            numPackedProbeRoIs++;
          } else {
            numPackedChildRoIs++;
          }
        }
      }
      int numSelectedFrames = std::accumulate(
          selectedFrames.begin(), selectedFrames.end(), 0,
          [](int cnt, auto& it) {
            return cnt + it.second.size();
          });
      LOGD("PatchMixer::packRoIs "
           " Try pack %-4d Frames => %-4d childRoIs + %-4d ProbeRoIs packed among %-4lu RoIs",
           numSelectedFrames, numPackedChildRoIs, numPackedProbeRoIs, candidateRoIs.size());
      // If packing failed
      if (!isAllPacked && numSelectedFrames > minFrames) {
        int numPackedRoIs = numPackedChildRoIs + numPackedProbeRoIs;
        int numSelectedRoIs = std::accumulate(
            selectedFrames.begin(), selectedFrames.end(), 0,
            [](int cnt, auto& it) {
              return cnt + std::accumulate(
                  it.second.begin(), it.second.end(), 0,
                  [](int cnt, Frame* frame) { return cnt + frame->parentRoIs.size(); });
            });
        int avgRoIsPerFrame = numSelectedRoIs / numSelectedFrames;
        int numPackableFrames = std::min(numSelectedFrames - 1,
                                         std::max(minFrames, numPackedRoIs / avgRoIsPerFrame));
        splitFrames(selectedFrames, droppedFrames, numPackableFrames);
        for (Frame* frame : probeFrames) {
          frame->resetProbeRoIs();
        }
      } else { // If packing success
        for (auto&[mixedFrameIndex, aMixedFrameRoIs] : packedRoIsMap) {
          for (RoI* pRoI : aMixedFrameRoIs) {
            assert(pRoI->isPacked());
            assert(pRoI->packedMixedFrameIndex == mixedFrameIndex);
          }
        }
        for (auto&[i, aMixedFrameRoIs] : packedRoIsMap) {
          if (!aMixedFrameRoIs.empty()) {
            const InferenceInfo& info = inferencePlan[i];
            mixedFrames.emplace_back(info.device, aMixedFrameRoIs, info.size);
          }
        }
        break;
      }
    }
  } else {
    LOGE("No case left");
  }
  time_us mixingEndTime = NowMicros();
  for (auto& it : selectedFrames) {
    for (Frame* frame : it.second) {
      frame->mixingStartTime = mixingStartTime;
      frame->mixingEndTime = mixingEndTime;
    }
  }
  if (fullFrameTarget != nullptr) {
    fullFrameTarget->isFullFrameTarget = true;
  }
  return {mixedFrames, fullFrameTarget, selectedFrames, droppedFrames};
}

std::tuple<Frame*, std::vector<Frame*>, std::vector<RoI*>> PatchMixer::preparePack(
    MultiStream& selectedFrames, int fullFrameStreamIndex, bool probe, int numProbeSteps,
    float probeStepSize) const {
  Frame* fullFrameTarget = getFullFrameTarget(selectedFrames, fullFrameStreamIndex);
  std::vector<Frame*> probeFrames;
  if (probe && !mConfig.EMULATED_BATCH) {
    probeFrames = addProbeRoIs(selectedFrames, fullFrameTarget, numProbeSteps, probeStepSize);
  }
  std::vector<RoI*> candidateRoIs = collectRoIs(selectedFrames, fullFrameTarget);
  prioritizeRoIs(selectedFrames, fullFrameTarget);
  if (mConfig.PRIORITY_MIXING) {
    // Descending order
    std::sort(candidateRoIs.begin(), candidateRoIs.end(),
              [](RoI* l, RoI* r) { return l->priority > r->priority; });
  }
  return {fullFrameTarget, probeFrames, candidateRoIs};
}

void PatchMixer::splitFrames(MultiStream& selectedFrames, Stream& droppedFrames,
                             int numPackableFrames) {
  Stream allSelectedFrames;
  for (auto&[aStreamKey, aStreamFrames] : selectedFrames) {
    allSelectedFrames.insert(aStreamFrames.begin(), aStreamFrames.end());
  }
  auto it = allSelectedFrames.begin();
  for (int i = 0; i < allSelectedFrames.size(); i++) {
    if (numPackableFrames <= i) {
      Frame* frameToDrop = *it;
      droppedFrames.insert(frameToDrop);
      selectedFrames[frameToDrop->key].erase(frameToDrop);
    }
    it++;
  }
}

bool PatchMixer::tryPackRoI(const std::pair<float, float>& resizedWH,
                            std::vector<FreeRects>& freeRectsList,
                            bool firstMatch, RoI* pRoI,
                            std::map<int, std::set<RoI*>>* packedRoIsMap) const {
  // TODO
  const float roiArea = resizedWH.first * resizedWH.second;
  std::pair<int, int> minIndices = {-1, -1};
  float minDiffArea = 1e10;

  int numRoIsPerEdge = std::ceil(std::sqrt(mConfig.BATCH_SIZE));
  std::map<int, std::pair<float, float>> batchedRoISizes;
  for (int i = 0; i < freeRectsList.size(); i++) {
    float edgeLength = float(freeRectsList[i].frameSize) / float(numRoIsPerEdge);
    batchedRoISizes[i] = {edgeLength, edgeLength};
  }
  if (firstMatch) {
    for (int i = 0; i < freeRectsList.size(); i++) {
      FreeRects& freeRects = freeRectsList[i];
      std::pair<float, float> newResizedWH = mConfig.EMULATED_BATCH ? batchedRoISizes[i]
                                                                    : resizedWH;
      minDiffArea = 1e10;
      for (int j = 0; j < freeRects.rects.size(); j++) {
        const Rect& freeRect = freeRects.rects[j];
        if (canFit(newResizedWH, freeRect)) {
          float diffArea = freeRect.area() - roiArea;
          if (diffArea < minDiffArea) {
            minDiffArea = diffArea;
            minIndices = {i, j};
          }
        }
      }
      if (minIndices.first != -1) {
        break;
      }
    }
  } else {
    for (int i = 0; i < freeRectsList.size(); i++) {
      FreeRects& freeRects = freeRectsList[i];
      std::pair<float, float> newResizedWH = mConfig.EMULATED_BATCH ? batchedRoISizes[i]
                                                                    : resizedWH;
      for (int j = 0; j < freeRects.rects.size(); j++) {
        const Rect& freeRect = freeRects.rects[j];
        if (canFit(newResizedWH, freeRect)) {
          float diffArea = freeRect.area() - roiArea;
          if (diffArea < minDiffArea) {
            minDiffArea = diffArea;
            minIndices = {i, j};
          }
        }
      }
    }
  }
  if (minIndices.first != -1) {
    assert(minIndices.second != -1);
    auto[i, j] = minIndices;
    const Rect freeRect = freeRectsList[i].rects[j];
    freeRectsList[i].rects.erase(freeRectsList[i].rects.begin() + j);
    std::pair<Rect, Rect> newFreeRectPair = splitFreeRect(resizedWH, freeRect);
    freeRectsList[i].rects.push_back(newFreeRectPair.first);
    freeRectsList[i].rects.push_back(newFreeRectPair.second);
    if (pRoI != nullptr) {
      (*packedRoIsMap)[i].insert(pRoI);
      pRoI->packedMixedFrameIndex = i;

      if (!mConfig.EMULATED_BATCH) {
        pRoI->packedLocation = std::make_pair(freeRect.left, freeRect.top);
      } else {
        assert(resizedWH.first == resizedWH.second);
        const float& batchedRoISize = resizedWH.first;
        float width = pRoI->paddedLoc.width();
        float height = pRoI->paddedLoc.height();
        if (pRoI->maxEdgeLength > batchedRoISize) {
          float resizedWidth = width > height ? batchedRoISize : width * batchedRoISize / height;
          float resizedHeight = width > height ? height * batchedRoISize / width : batchedRoISize;
          pRoI->setTargetSize(batchedRoISize);
          pRoI->packedLocation = std::make_pair(
              freeRect.left + (batchedRoISize - resizedWidth) / 2,
              freeRect.top + (batchedRoISize - resizedHeight) / 2);
        } else {
          pRoI->setTargetSize(pRoI->maxEdgeLength);
          pRoI->packedLocation = std::make_pair(
              freeRect.left + (batchedRoISize - width) / 2,
              freeRect.top + (batchedRoISize - height) / 2);
        }
      }
    }
    return true;
  } else {
    return false;
  }
}

Frame* PatchMixer::getFullFrameTarget(const MultiStream& selectedFrames, int fullFrameStreamIndex) {
  if (fullFrameStreamIndex == -1) {
    return nullptr;
  }
  std::vector<std::string> nonEmptyStreamKeys;
  for (const auto&[aStreamKey, aStreamFrames] : selectedFrames) {
    if (!aStreamFrames.empty()) {
      nonEmptyStreamKeys.push_back(aStreamKey);
    }
  }
  assert(!nonEmptyStreamKeys.empty());
  fullFrameStreamIndex %= (int) nonEmptyStreamKeys.size();
  Frame* fullFrameTarget = *selectedFrames.at(nonEmptyStreamKeys[fullFrameStreamIndex]).rbegin();
  return fullFrameTarget;
}

bool PatchMixer::canFit(std::pair<float, float> wh, const Rect& rect) {
  return wh.first <= rect.width() && wh.second <= rect.height();
}

std::pair<Rect, Rect> PatchMixer::splitFreeRect(std::pair<float, float> wh, const Rect& rect) {
  float w = wh.first;
  float h = wh.second;
  if (rect.width() > rect.height()) {
    return std::make_pair(Rect(rect.left + w, rect.top, rect.right, rect.bottom),
                          Rect(rect.left, rect.top + h, rect.left + w, rect.bottom));
  } else {
    return std::make_pair(Rect(rect.left, rect.top + h, rect.right, rect.bottom),
                          Rect(rect.left + w, rect.top, rect.right, rect.top + h));
  }
}

} // namespace rm
