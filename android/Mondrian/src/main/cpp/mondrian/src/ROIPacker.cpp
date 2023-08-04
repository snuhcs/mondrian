#include "mondrian/ROIPacker.hpp"

#include <numeric>
#include <sstream>

#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIPrioritizer.hpp"

namespace md {

std::vector<PackedCanvas> ROIPacker::packCanvases(MultiStream streams,
                                                  std::vector<InferenceInfo> inferencePlan,
                                                  const Frame* fullFrameTarget,
                                                  const ExecutionType executionType,
                                                  const int roiSize,
                                                  const ROIPrioritizerType roiPrioritizerType,
                                                  const bool noDownsamplingForLast) {
  time_us startTime = NowMicros();
  std::vector<std::vector<IntRect>> freeRectsVec;
  for (const auto& info : inferencePlan) {
    freeRectsVec.push_back({IntRect(0, 0, info.size, info.size)});
  }

  // Last frames
  for (const auto& [vid, stream] : streams) {
    Frame* lastFrame = *stream.rbegin();
    if (lastFrame == fullFrameTarget) continue;
    if (executionType == ROI_WISE_INFERENCE) {
      for (auto& mergedROI : lastFrame->mergedROIs) {
        if (!freeRectsVec.empty()) {
          mergedROI->setPackInfo({0, 0},
                                 (int) freeRectsVec.size() - 1,
                                 executionType,
                                 roiSize);
          freeRectsVec.erase(freeRectsVec.end() - 1);
        }
      }
    } else { // MONDRIAN, EMULATED_BATCH
      auto [indices, locations] = ROIPacker::pack(freeRectsVec, lastFrame->boxesIfLast,
          /*backward=*/false, executionType, roiSize);
      bool fullyPacked = indices.size() == lastFrame->boxesIfLast.size();
      int maxPackedCanvasIndex = -1;
      for (auto& [packedCanvasIndex, freeRectIndex] : indices) {
        maxPackedCanvasIndex = std::max(maxPackedCanvasIndex, packedCanvasIndex);
      }
      LOGD("XXX == Last Pack Frame %d: %lu / %lu Packed, Last Packed Frame=%d",
           lastFrame->frameIndex,
           indices.size(),
           lastFrame->boxesIfLast.size(),
           maxPackedCanvasIndex);
      if (fullyPacked) {
        ROIPacker::apply(freeRectsVec,
                         lastFrame->boxesIfLast,
                         indices,
                         executionType,
                         roiSize);
      } else {
        indices.resize(lastFrame->boxesIfLast.size());
        locations.resize(lastFrame->boxesIfLast.size());
        ROIPacker::apply(freeRectsVec,
                         lastFrame->boxesIfLast,
                         indices,
                         executionType,
                         roiSize);
      }
      lastFrame->prepareFrameLast(indices,
                                  locations,
                                  executionType,
                                  roiSize,
                                  noDownsamplingForLast);
    }
  }
  time_us packLastTime = NowMicros();

  // Order MergedROIs
  auto orderedMergedROIs = ROIPrioritizer::order(streams,
                                                 roiPrioritizerType);
  time_us orderTime = NowMicros();

  // Pack MergedROIs
  for (MergedROI* mergedROI : orderedMergedROIs) {
    if (executionType == ROI_WISE_INFERENCE) {
      if (!freeRectsVec.empty()) {
        mergedROI->setPackInfo({0, 0},
                               freeRectsVec.size() - 1,
                               executionType,
                               roiSize);
        freeRectsVec.erase(freeRectsVec.end() - 1);
      }
    } else {
      auto [bw, bh] = mergedROI->borderedMatWH();
      auto [indices, locations] = ROIPacker::pack(freeRectsVec, {{bw, bh}},
          /*backward=*/false,
                                                  executionType,
                                                  roiSize);
      if (!indices.empty()) {
        ROIPacker::apply(freeRectsVec,
                         {{bw, bh}},
                         indices,
                         executionType,
                         roiSize);
        mergedROI->setPackInfo(locations[0],
                               indices[0].first,
                               executionType,
                               roiSize);
      }
    }
  }
  time_us packOthersTime = NowMicros();

  LOGD("Packing %d Frames with %lu ROIs | "
       "total: %lld, packLastTime: %lld, orderTime: %lld, packOthersTime: %lld",
       std::accumulate(streams.begin(), streams.end(), 0,
                       [](int sum, const auto& pair) {
                         return sum + pair.second.size();
                       }),
       orderedMergedROIs.size(),
       packOthersTime - startTime,
       packLastTime - startTime,
       orderTime - packLastTime,
       packOthersTime - orderTime);

  std::map<int, std::set<MergedROI*>> groupedMergedROIs;
  for (const auto& [vid, frames] : streams) {
    for (Frame* frame : frames) {
      if (frame == fullFrameTarget) continue;
      for (auto& mergedROI : frame->mergedROIs) {
        if (mergedROI->isPacked()) {
          groupedMergedROIs[mergedROI->relativePackedCanvasIndex()].insert(mergedROI.get());
        }
      }
      for (auto& probeROI : frame->probingROIs) {
        if (probeROI->isPacked()) {
          groupedMergedROIs[probeROI->relativePackedCanvasIndex()].insert(probeROI.get());
        }
      }
    }
  }

  std::vector<PackedCanvas> packedCanvases;
  for (auto& [relativePackedCanvasIndex, mergedROIs] : groupedMergedROIs) {
    assert(relativePackedCanvasIndex < inferencePlan.size());
    const auto& info = inferencePlan[relativePackedCanvasIndex];
    if (!mergedROIs.empty()) {
      packedCanvases.emplace_back(mergedROIs, info.size, info.device);
    }
  }
  return packedCanvases;
}

std::pair<IntPairs, IntPairs> ROIPacker::pack(
    const std::vector<std::vector<IntRect>>& freeRectsVec,
    const IntPairs& boxWHs, bool backward, ExecutionType executionType, int roiSize) {
  auto copiedFreeRectsVec = freeRectsVec;
  IntPairs packIndices;
  IntPairs packLocations;
  for (const auto& [w, h] : boxWHs) {
    int pack_i = -1;
    int pack_j = -1;
    for (int _i = 0; _i < copiedFreeRectsVec.size(); _i++) {
      int i = backward
              ? int(copiedFreeRectsVec.size()) - 1 - _i
              : _i;
      if (executionType == EMULATED_BATCH) {
        pack_j = !copiedFreeRectsVec[i].empty() ? 0 : -1;
      } else {
        pack_j = getBestFitFreeRectIndex(copiedFreeRectsVec[i], w, h);
      }
      if (pack_j != -1) {
        pack_i = i;
        break;
      }
    }
    if (pack_i == -1 || pack_j == -1) {
      assert(pack_i == -1 && pack_j == -1);
      assert(packIndices.size() == packLocations.size());
      return {packIndices, packLocations};
    }
    const IntRect& rect = copiedFreeRectsVec[pack_i][pack_j];
    packIndices.emplace_back(pack_i, pack_j);
    packLocations.emplace_back(rect.l, rect.t);
    if (executionType == EMULATED_BATCH) {
      packBox(copiedFreeRectsVec, roiSize, roiSize, pack_i, pack_j);
    } else {
      packBox(copiedFreeRectsVec, w, h, pack_i, pack_j);
    }
  }
  assert(packIndices.size() == packLocations.size());
  assert(packIndices.size() <= boxWHs.size());
  return {packIndices, packLocations};
}

void ROIPacker::apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                      const IntPairs& boxWH, const IntPairs& indices,
                      ExecutionType executionType, int roiSize) {
  assert(boxWH.size() == indices.size());
  for (int i = 0; i < boxWH.size(); i++) {
    auto [w, h] = boxWH[i];
    auto [pack_i, pack_j] = indices[i];
    if (executionType == EMULATED_BATCH) {
      packBox(freeRectsVec, roiSize, roiSize, pack_i, pack_j);
    } else {
      packBox(freeRectsVec, w, h, pack_i, pack_j);
    }
  }
}

int ROIPacker::getBestFitFreeRectIndex(const std::vector<IntRect>& freeRects, int w, int h) {
  int minRemainingArea = INT_MAX / 2;
  int best_index = -1;
  for (int i = 0; i < freeRects.size(); i++) {
    const IntRect& freeRect = freeRects[i];
    if (canFit(w, h, freeRect)) {
      int remainingArea = freeRect.area - w * h;
      if (minRemainingArea > remainingArea) {
        minRemainingArea = remainingArea;
        best_index = i;
      }
    }
  }
  return best_index;
}

void ROIPacker::packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                        int w, int h, int pack_i, int pack_j) {
  assert(pack_i < freeRectsVec.size() && pack_j < freeRectsVec[pack_i].size());
  IntRect freeRectToPack = freeRectsVec[pack_i][pack_j];
  freeRectsVec[pack_i].erase(freeRectsVec[pack_i].begin() + pack_j);
  auto [rect0, rect1] = splitFreeRect(w, h, freeRectToPack);
  if (rect0.area > 0) {
    freeRectsVec[pack_i].push_back(rect0);
  }
  if (rect1.area > 0) {
    freeRectsVec[pack_i].push_back(rect1);
  }
}

bool ROIPacker::canFit(int w, int h, const IntRect& freeRect) {
  return w <= freeRect.w && h <= freeRect.h;
}

std::pair<IntRect, IntRect> ROIPacker::splitFreeRect(int w, int h, const IntRect& freeRect) {
  if (freeRect.w > freeRect.h) {
    return {IntRect(freeRect.l + w, freeRect.t, freeRect.r, freeRect.b),
            IntRect(freeRect.l, freeRect.t + h, freeRect.l + w, freeRect.b)};
  } else {
    return {IntRect(freeRect.l, freeRect.t + h, freeRect.r, freeRect.b),
            IntRect(freeRect.l + w, freeRect.t, freeRect.r, freeRect.t + h)};
  }
}

} // namespace md
