#include "strm/PatchMixer.hpp"

#include <numeric>

#include "strm/Test.hpp"

namespace rm {

const float PatchMixer::HIGH_PRIORITY = 1e9;

PatchMixer::PatchMixer(const PatchMixerConfig& config)
    : mConfig(config) {}

std::vector<RoI*> PatchMixer::prepareRoIs(std::map<std::string, SortedFrames>& frames,
                                          Frame* fullFrameTarget, RoIResizer* roiResizer,
                                          int maxRoISize, bool probe, int numProbeSteps,
                                          int probeStepSize, bool roiWiseInference) const {
  time_us resizeStartTime = NowMicros();
  if (!mConfig.EMULATED_BATCH && !roiWiseInference) {
    resizeRoIs(frames, roiResizer);
  }
  time_us resizeEndTime = NowMicros();
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      frame->resizeStartTime = resizeStartTime;
      frame->resizeEndTime = resizeEndTime;
    }
  }

  time_us mergeStartTime = NowMicros();
  initParentRoIs(frames);
  if (mConfig.MERGE) {
    mergeRoIs(frames, maxRoISize, mConfig.MERGE_THRESHOLD);
  }
  time_us mergeEndTime = NowMicros();
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      frame->mergeRoIStartTime = mergeStartTime;
      frame->mergeRoIEndTime = mergeEndTime;
    }
  }

  if (probe && !mConfig.EMULATED_BATCH) {
    addProbeRoIs(frames, fullFrameTarget, numProbeSteps, probeStepSize);
  }

  std::vector<RoI*> packingCandidates = collectRoIs(frames, fullFrameTarget);

  prioritizeRoIs(frames, fullFrameTarget);

  if (mConfig.PRIORITY_MIXING) {
    std::sort(packingCandidates.begin(), packingCandidates.end(),
              [](const RoI* l, const RoI* r) { return l->priority > r->priority; });
  }

  return packingCandidates;
}

void PatchMixer::resizeRoIs(std::map<std::string, SortedFrames>& frames, RoIResizer* roiResizer) {
  // Resize OF RoIs
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      for (auto& cRoI : frame->childRoIs) {
        if (cRoI->type == RoI::Type::OF) {
          cRoI->targetSize = std::min(cRoI->maxEdgeLength,
                                      (int) roiResizer->getTargetSize(cRoI->id, cRoI->features));
        }
      }
    }
  }

  // Resize PD RoIs with next OF RoI if exists
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      for (auto& cRoI : frame->childRoIs) {
        if (cRoI->type == RoI::Type::PD && cRoI->nextRoI != nullptr) {
          cRoI->targetSize = std::min(cRoI->maxEdgeLength, cRoI->nextRoI->targetSize);
        }
      }
    }
  }
}

void PatchMixer::initParentRoIs(std::map<std::string, SortedFrames>& frames) {
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      frame->initParentRoIs();
    }
  }
}

void PatchMixer::mergeRoIs(std::map<std::string, SortedFrames>& frames, int maxRoISize,
                           float mergeThreshold) {
  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      frame->mergeRoIs(mergeThreshold, maxRoISize);
    }
  }

  for (auto&[_, aStreamFrames] : frames) {
    for (Frame* frame : aStreamFrames) {
      testAssignedUniqueRoIID(frame->childRoIs);
      testParentChildrenIDsAndChildIDsSame(frame->childRoIs, frame->parentRoIs);
      testChildRoIsFrameRelation(frame->childRoIs);
      testParentRoIsFrameRelation(frame->parentRoIs);
    }
  }
}

void PatchMixer::addProbeRoIs(std::map<std::string, SortedFrames>& frames, const Frame* fullFrameTarget,
                              int numProbeSteps, int probeStepSize) {
  std::set<Frame*> lastFrames = filterLastFrames(frames);
  for (Frame* lastFrame : lastFrames) {
    if (lastFrame != fullFrameTarget) {
      lastFrame->addProbeRoIs(numProbeSteps, probeStepSize);
    }
  }
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
        if (pRoI->prevRoI != nullptr) {
          auto&[prevX, prevY] = pRoI->prevRoI->features.ofFeatures.avgShift;
          auto&[currX, currY] = pRoI->features.ofFeatures.avgShift;
          float diffX = currX - prevX;
          float diffY = currY - prevY;
          pRoI->priority = pRoI->features.ofFeatures.avgErr + (diffX * diffX + diffY * diffY);
        } else {
          pRoI->priority = HIGH_PRIORITY;
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
        pRoI->priority = HIGH_PRIORITY;
      }
    }
  }
}

std::vector<MixedFrame> PatchMixer::packRoIs(
    std::vector<RoI*>& candidateRoIs, int mixedFrameSize, int maxNumMixedFrames) const {
  time_us mixingStartTime = NowMicros();

  // Init data structures
  std::map<int, std::set<RoI*>> packedRoIsMap;
  std::map<int, std::vector<Rect>> freeRectsMap;
  for (int mixedFrameIndex = 0; mixedFrameIndex < maxNumMixedFrames; mixedFrameIndex++) {
    freeRectsMap[mixedFrameIndex] = {Rect(0, 0, mixedFrameSize, mixedFrameSize)};
  }

  // Pack RoIs
  int batchedRoISize = mixedFrameSize / std::ceil(std::sqrt(mConfig.BATCH_SIZE));
  for (RoI* pRoI : candidateRoIs) {
    std::pair<int, int> resizedWH = mConfig.EMULATED_BATCH ?
                                    std::make_pair(batchedRoISize, batchedRoISize) :
                                    pRoI->getResizedWidthHeight();
    const int roiArea = resizedWH.first * resizedWH.second;
    auto indices = std::make_pair(-1, -1);
    // N-way packing : find the best matching freeRect
    if (mConfig.N_WAY_MIXING && !mConfig.EMULATED_BATCH) {
      int minDiffArea = INT_MAX;
      for (auto&[mixedFrameIndex, freeRects] : freeRectsMap) {
        for (int freeRectIndex = 0; freeRectIndex < freeRects.size(); freeRectIndex++) {
          const Rect& freeRect = freeRects[freeRectIndex];
          if (canFit(resizedWH, freeRect)) {
            int diffArea = freeRect.area() - roiArea;
            if (diffArea < minDiffArea) {
              minDiffArea = diffArea;
              indices = std::make_pair(mixedFrameIndex, freeRectIndex);
            }
          }
          // For High priority RoI, use the first possible mixed frame.
          if (pRoI->priority == HIGH_PRIORITY && minDiffArea != INT_MAX) {
            break;
          }
        }
      }
    }
    // 1-way packing : find the first matching freeRect
    else {
      for (auto&[mixedFrameIndex, freeRects] : freeRectsMap) {
        for (int freeRectIndex = 0; freeRectIndex < freeRects.size(); freeRectIndex++) {
          const Rect& freeRect = freeRects[freeRectIndex];
          if (canFit(resizedWH, freeRect)) {
            indices = std::make_pair(mixedFrameIndex, freeRectIndex);
            break;
          }
        }
      }
    }
    auto&[packedIndex, freeRectIndex] = indices;
    if (packedIndex != -1) {
      assert(freeRectIndex != -1);
      assert(pRoI->packedMixedFrameIndex == INT_MAX);

      packedRoIsMap[packedIndex].insert(pRoI);
      pRoI->packedMixedFrameIndex = packedIndex;

      const Rect freeRect = freeRectsMap[packedIndex][freeRectIndex];
      if (!mConfig.EMULATED_BATCH) {
        pRoI->packedLocation = std::make_pair(freeRect.left, freeRect.top);
      } else {
        int width = pRoI->paddedLoc.width();
        int height = pRoI->paddedLoc.height();
        if (pRoI->maxEdgeLength > batchedRoISize) {
          int resizedWidth = width > height ? batchedRoISize : width * batchedRoISize / height;
          int resizedHeight = width > height ? height * batchedRoISize / width : batchedRoISize;
          pRoI->targetSize = batchedRoISize;
          pRoI->packedLocation = std::make_pair(
              freeRect.left + (batchedRoISize - resizedWidth) / 2,
              freeRect.top + (batchedRoISize - resizedHeight) / 2);
        } else {
          pRoI->targetSize = pRoI->maxEdgeLength;
          pRoI->packedLocation = std::make_pair(
              freeRect.left + (batchedRoISize - width) / 2,
              freeRect.top + (batchedRoISize - height) / 2);
        }
      }
      freeRectsMap[packedIndex].erase(freeRectsMap[packedIndex].begin() + freeRectIndex);

      std::pair<Rect, Rect> newFreeRectPair = splitFreeRect(resizedWH, freeRect);
      freeRectsMap[packedIndex].push_back(newFreeRectPair.first);
      freeRectsMap[packedIndex].push_back(newFreeRectPair.second);
    }
  }
  for (auto&[mixedFrameIndex, aMixedFrameRoIs] : packedRoIsMap) {
    for (RoI* pRoI : aMixedFrameRoIs) {
      assert(pRoI->isPacked());
      assert(pRoI->packedMixedFrameIndex == mixedFrameIndex);
    }
  }
  std::vector<MixedFrame> mixedFrames;
  for (auto&[_, aMixedFrameRoIs] : packedRoIsMap) {
    if (!aMixedFrameRoIs.empty()) {
      mixedFrames.emplace_back(aMixedFrameRoIs, mixedFrameSize);
    }
  }

  time_us mixingEndTime = NowMicros();
  for (RoI* roi : candidateRoIs) {
    roi->frame->mixingStartTime = mixingStartTime;
    roi->frame->mixingEndTime = mixingEndTime;
  }
  return mixedFrames;
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
