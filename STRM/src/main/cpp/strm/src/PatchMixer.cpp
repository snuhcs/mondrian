#include "strm/PatchMixer.hpp"

#include <numeric>

#include "strm/Test.hpp"

namespace rm {

const float PatchMixer::HIGHEST_PRIORITY = 103;
const float PatchMixer::HIGHER_PRIORITY = 102;
const float PatchMixer::HIGH_PRIORITY = 101;

PatchMixer::PatchMixer(const PatchMixerConfig& config)
    : mConfig(config) {}

std::vector<Frame*> PatchMixer::addProbeRoIs(std::map<std::string, SortedFrames>& frames,
                                             const Frame* fullFrameTarget,
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

std::vector<RoI*> PatchMixer::collectRoIs(std::map<std::string, SortedFrames>& frames,
                                          const Frame* fullFrameTarget) {
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

void PatchMixer::prioritizeRoIs(std::map<std::string, SortedFrames>& frames,
                                const Frame* fullFrameTarget) {
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

std::tuple<std::vector<MixedFrame>, Frame*, std::map<std::string, SortedFrames>, SortedFrames>
PatchMixer::packRoIs(std::map<std::string, SortedFrames>& frames, int fullFrameStreamIndex,
                     int frameSize, int numFrames, bool allowInterpolation, bool roiWiseInference,
                     bool probe, int numProbeSteps, float probeStepSize) {
  time_us mixingStartTime = NowMicros();
  std::vector<MixedFrame> mixedFrames;
  Frame* fullFrameTarget;
  std::map<std::string, SortedFrames> selectedFrames = frames;
  SortedFrames droppedFrames;
  if (allowInterpolation) {
    fullFrameTarget = getFullFrameTarget(selectedFrames, fullFrameStreamIndex);
    if (probe && !mConfig.EMULATED_BATCH) {
      addProbeRoIs(frames, fullFrameTarget, numProbeSteps, probeStepSize);
    }
    std::vector<RoI*> candidateRoIs = collectRoIs(frames, fullFrameTarget);
    prioritizeRoIs(frames, fullFrameTarget);
    if (mConfig.PRIORITY_MIXING) {
      std::sort(candidateRoIs.begin(), candidateRoIs.end(),
                [](RoI* l, RoI* r) { return l->priority > r->priority; });
    }

    if (roiWiseInference) { // allowInterpolation == true && roiWiseInference == true
      for (int i = 0; i < std::min(numFrames, (int) candidateRoIs.size()); i++) {
        RoI* pRoI = candidateRoIs[i];
        pRoI->setTargetSize(float(frameSize));
        auto[resizedWidth, resizedHeight] = pRoI->getResizedWidthHeight();
        float x = float(frameSize - resizedWidth) / 2;
        float y = float(frameSize - resizedHeight) / 2;
        pRoI->packedLocation = {x, y};
        mixedFrames.push_back(MixedFrame({pRoI}, frameSize));
      }
    } else { // allowInterpolation == true && roiWiseInference == false
      // Init data structures
      std::map<int, std::set<RoI*>> packedRoIsMap;
      std::map<int, std::vector<Rect>> freeRectsMap;
      for (int mixedFrameIndex = 0; mixedFrameIndex < numFrames; mixedFrameIndex++) {
        freeRectsMap[mixedFrameIndex] = {Rect(0, 0, frameSize, frameSize)};
      }

      // Pack RoIs
      float batchedRoISize = float(frameSize) / std::ceil(std::sqrt(mConfig.BATCH_SIZE));
      for (RoI* pRoI : candidateRoIs) {
        std::pair<float, float> resizedWH = mConfig.EMULATED_BATCH ?
                                            std::make_pair(batchedRoISize, batchedRoISize) :
                                            pRoI->getResizedWidthHeight();
        tryPackRoI(
            resizedWH, freeRectsMap,
            pRoI->priority == HIGH_PRIORITY || mConfig.EMULATED_BATCH || !mConfig.N_WAY_MIXING,
            pRoI, &packedRoIsMap, mConfig.EMULATED_BATCH);
      }
      for (auto&[mixedFrameIndex, aMixedFrameRoIs] : packedRoIsMap) {
        for (RoI* pRoI : aMixedFrameRoIs) {
          assert(pRoI->isPacked());
          assert(pRoI->packedMixedFrameIndex == mixedFrameIndex);
        }
      }
      for (auto&[_, aMixedFrameRoIs] : packedRoIsMap) {
        if (!aMixedFrameRoIs.empty()) {
          mixedFrames.emplace_back(aMixedFrameRoIs, frameSize);
        }
      }

      time_us mixingEndTime = NowMicros();
      for (RoI* roi : candidateRoIs) {
        roi->frame->mixingStartTime = mixingStartTime;
        roi->frame->mixingEndTime = mixingEndTime;
      }
    }
  } else {
    if (roiWiseInference) { // allowInterpolation == false && roiWiseInference == true
      while (true) {
        fullFrameTarget = getFullFrameTarget(selectedFrames, fullFrameStreamIndex);
        std::vector<Frame*> probeFrames;
        if (probe && !mConfig.EMULATED_BATCH) {
          probeFrames = addProbeRoIs(selectedFrames, fullFrameTarget, numProbeSteps, probeStepSize);
        }
        std::vector<RoI*> candidateRoIs = collectRoIs(selectedFrames, fullFrameTarget);

        int numSelectedFrames = std::accumulate(
            selectedFrames.begin(), selectedFrames.end(), 0,
            [](int cnt, auto& it) {
              return cnt + it.second.size();
            });
        LOGD("PatchMixer::packRoIs: Try pack %-4d Frames => %-4lu / %-4d RoIs can be inference",
             numSelectedFrames, candidateRoIs.size(), numFrames);

        int numPackableFrames = numSelectedFrames - 1;
        if ((fullFrameTarget == nullptr ? numPackableFrames > 0 : numPackableFrames > 1) &&
            candidateRoIs.size() > numFrames) {
          SortedFrames allSelectedFrames;
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
          for (Frame* frame : probeFrames) {
            frame->resetProbeRoIs();
          }
          continue;
        }

        prioritizeRoIs(selectedFrames, fullFrameTarget);
        if (mConfig.PRIORITY_MIXING) {
          // Descending order
          std::sort(candidateRoIs.begin(), candidateRoIs.end(),
                    [](RoI* l, RoI* r) { return l->priority > r->priority; });
        }
        for (int i = 0; i < std::min(numFrames, (int) candidateRoIs.size()); i++) {
          RoI* pRoI = candidateRoIs[i];
          pRoI->setTargetSize(float(frameSize));
          auto[resizedWidth, resizedHeight] = pRoI->getResizedWidthHeight();
          float x = float(frameSize - resizedWidth) / 2;
          float y = float(frameSize - resizedHeight) / 2;
          pRoI->packedLocation = {x, y};
          mixedFrames.push_back(MixedFrame({pRoI}, frameSize));
        }
        break;
      }
    } else { // allowInterpolation == false && roiWiseInference == false
      while (true) {
        fullFrameTarget = getFullFrameTarget(selectedFrames, fullFrameStreamIndex);
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

        std::map<int, std::set<RoI*>> packedRoIsMap;
        std::map<int, std::vector<Rect>> freeRectsMap;
        for (int mixedFrameIndex = 0; mixedFrameIndex < numFrames; mixedFrameIndex++) {
          freeRectsMap[mixedFrameIndex] = {Rect(0, 0, frameSize, frameSize)};
        }

        // Pack RoIs
        int numPackedChildRoIs = 0;
        int numPackedProbeRoIs = 0;
        float batchedRoISize = float(frameSize) / std::ceil(std::sqrt(mConfig.BATCH_SIZE));
        bool isAllPacked = true;
        for (RoI* pRoI : candidateRoIs) {
          std::pair<float, float> resizedWH = mConfig.EMULATED_BATCH ?
                                              std::make_pair(batchedRoISize, batchedRoISize) :
                                              pRoI->getResizedWidthHeight();
          if (!tryPackRoI(
              resizedWH, freeRectsMap,
              pRoI->priority == HIGH_PRIORITY || mConfig.EMULATED_BATCH || !mConfig.N_WAY_MIXING,
              pRoI, &packedRoIsMap, mConfig.EMULATED_BATCH)) {
            isAllPacked = false;
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
//        int numPackableFrames = std::max(1, numSelectedFrames * numPackedChildRoIs / numChildRoIs);
        int numPackableFrames = numSelectedFrames - 1;
        if ((fullFrameTarget == nullptr ? numPackableFrames > 0 : numPackableFrames > 1) &&
            !isAllPacked) {
          SortedFrames allSelectedFrames;
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
          for (Frame* frame : probeFrames) {
            frame->resetProbeRoIs();
          }
          continue;
        }
        for (auto&[mixedFrameIndex, aMixedFrameRoIs] : packedRoIsMap) {
          for (RoI* pRoI : aMixedFrameRoIs) {
            assert(pRoI->isPacked());
            assert(pRoI->packedMixedFrameIndex == mixedFrameIndex);
          }
        }
        for (auto&[_, aMixedFrameRoIs] : packedRoIsMap) {
          if (!aMixedFrameRoIs.empty()) {
            mixedFrames.emplace_back(aMixedFrameRoIs, frameSize);
          }
        }

        time_us mixingEndTime = NowMicros();
        for (RoI* roi : candidateRoIs) {
          roi->frame->mixingStartTime = mixingStartTime;
          roi->frame->mixingEndTime = mixingEndTime;
        }
        break;
      }
    }
  }
  fullFrameTarget->isFullFrameTarget = true;
  return {mixedFrames, fullFrameTarget, selectedFrames, droppedFrames};
}

bool PatchMixer::tryPackRoI(const std::pair<float, float>& resizedWH,
                            std::map<int, std::vector<Rect>>& freeRectsMap,
                            bool firstMatch, RoI* pRoI,
                            std::map<int, std::set<RoI*>>* packedRoIsMap, bool emulatedBatch) {
  const float roiArea = resizedWH.first * resizedWH.second;
  int minPackedIndex = -1;
  int minFreeRectIndex = -1;
  float minDiffArea = 1e10;
  if (firstMatch) {
    for (auto&[mixedFrameIndex, freeRects] : freeRectsMap) {
      minDiffArea = 1e10;
      for (int freeRectIndex = 0; freeRectIndex < freeRects.size(); freeRectIndex++) {
        const Rect& freeRect = freeRects[freeRectIndex];
        if (canFit(resizedWH, freeRect)) {
          float diffArea = freeRect.area() - roiArea;
          if (diffArea < minDiffArea) {
            minDiffArea = diffArea;
            minPackedIndex = mixedFrameIndex;
            minFreeRectIndex = freeRectIndex;
          }
        }
      }
      if (minFreeRectIndex != -1) {
        break;
      }
    }
  } else {
    for (auto&[mixedFrameIndex, freeRects] : freeRectsMap) {
      for (int freeRectIndex = 0; freeRectIndex < freeRects.size(); freeRectIndex++) {
        const Rect& freeRect = freeRects[freeRectIndex];
        if (canFit(resizedWH, freeRect)) {
          float diffArea = freeRect.area() - roiArea;
          if (diffArea < minDiffArea) {
            minDiffArea = diffArea;
            minPackedIndex = mixedFrameIndex;
            minFreeRectIndex = freeRectIndex;
          }
        }
      }
    }
  }
  if (minFreeRectIndex != -1) {
    assert(minPackedIndex != -1);
    const Rect freeRect = freeRectsMap[minPackedIndex][minFreeRectIndex];
    freeRectsMap[minPackedIndex].erase(freeRectsMap[minPackedIndex].begin() + minFreeRectIndex);
    std::pair<Rect, Rect> newFreeRectPair = splitFreeRect(resizedWH, freeRect);
    freeRectsMap[minPackedIndex].push_back(newFreeRectPair.first);
    freeRectsMap[minPackedIndex].push_back(newFreeRectPair.second);
    if (pRoI != nullptr) {
      (*packedRoIsMap)[minPackedIndex].insert(pRoI);
      pRoI->packedMixedFrameIndex = minPackedIndex;

      if (!emulatedBatch) {
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

Frame* PatchMixer::getFullFrameTarget(
    const std::map<std::string, SortedFrames>& selectedFrames, int fullFrameStreamIndex) {
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
