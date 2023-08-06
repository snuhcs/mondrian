#include "mondrian/ROIPacker.hpp"

#include <numeric>
#include <sstream>

#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIPrioritizer.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

ROIPacker::ROIPacker(
    const ROIPackerConfig& config,
    const ExecutionType executionType,
    const int roiSize,
    ROIResizer* roiResizer)
    : config_(config),
      executionType_(executionType),
      roiSize_(roiSize),
      roiResizer_(roiResizer) {}

std::vector<PackedCanvas> ROIPacker::packCanvases(const int currID,
                                                  const MultiStream& streams,
                                                  const std::vector<InferenceInfo>& inferencePlan,
                                                  const Frame* fullFrameTarget) {
  time_us startTime = NowMicros();

  // Prepare free rects
  std::vector<std::vector<IntRect>> freeRectsVec;
  for (const auto& info : inferencePlan) {
    freeRectsVec.push_back({{0, 0, info.size, info.size}});
  }

  // Pack last frames first
  for (const auto& [vid, stream] : streams) {
    Frame* lastFrame = *stream.rbegin();
    if (lastFrame == fullFrameTarget) continue;
    IntPairs boxesIfLast = lastFrame->boxesIfLast(
        /*roiResizer=*/roiResizer_,
        /*executionType=*/executionType_,
        /*noDownsampling=*/config_.NO_DOWNSAMPLING_FOR_LAST_FRAME);
    if (executionType_ == ROI_WISE_INFERENCE) {
      for (auto& mergedROI : lastFrame->mergedROIs) {
        if (!freeRectsVec.empty()) {
          mergedROI->setPackInfo({0, 0},
                                 (int) freeRectsVec.size() - 1,
                                 executionType_,
                                 roiSize_);
          freeRectsVec.erase(freeRectsVec.end() - 1);
        }
      }
    } else { // MONDRIAN, EMULATED_BATCH
      auto [indices, locations] = ROIPacker::pack(freeRectsVec, boxesIfLast);
      bool fullyPacked = indices.size() == boxesIfLast.size();
      int minPackedCanvasIndex = (int) inferencePlan.size();
      int maxPackedCanvasIndex = -1;
      for (auto& [packedCanvasIndex, freeRectIndex] : indices) {
        minPackedCanvasIndex = std::min(minPackedCanvasIndex, packedCanvasIndex);
        maxPackedCanvasIndex = std::max(maxPackedCanvasIndex, packedCanvasIndex);
      }
      LOGD("[Schedule %d] Last Packed Frame vid=%d fid=%d "
           "// %lu / %lu MergedROIs Packed into %d ~ %d PackedCanvas",
           currID, lastFrame->vid, lastFrame->frameIndex,
           indices.size(), boxesIfLast.size(),
           minPackedCanvasIndex, maxPackedCanvasIndex);
      if (fullyPacked) {
        ROIPacker::apply(freeRectsVec, boxesIfLast, indices);
      } else {
        indices.resize(boxesIfLast.size());
        locations.resize(boxesIfLast.size());
        ROIPacker::apply(freeRectsVec, boxesIfLast, indices);
      }
      lastFrame->prepareFrameLast(indices,
                                  locations,
                                  executionType_,
                                  roiSize_,
                                  config_.NO_DOWNSAMPLING_FOR_LAST_FRAME);
    }
  }
  time_us packLastTime = NowMicros();

  // Order MergedROIs except last frames
  auto orderedMergedROIs = ROIPrioritizer::order(streams, config_.TYPE);
  time_us orderOthersTime = NowMicros();

  // Pack MergedROIs
  for (MergedROI* mergedROI : orderedMergedROIs) {
    if (executionType_ == ROI_WISE_INFERENCE) {
      if (!freeRectsVec.empty()) {
        mergedROI->setPackInfo({0, 0},
                               (int) freeRectsVec.size() - 1,
                               executionType_,
                               roiSize_);
        freeRectsVec.erase(freeRectsVec.end() - 1);
      }
    } else {
      auto [bw, bh] = mergedROI->borderedMatWH();
      auto [indices, locations] = ROIPacker::pack(freeRectsVec, {{bw, bh}});
      if (!indices.empty()) {
        ROIPacker::apply(freeRectsVec, {{bw, bh}}, indices);
        mergedROI->setPackInfo(locations[0],
                               indices[0].first,
                               executionType_,
                               roiSize_);
      }
    }
  }
  time_us packOthersTime = NowMicros();

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
  time_us groupingTime = NowMicros();

  std::vector<PackedCanvas> packedCanvases;
  for (auto& [relativePackedCanvasIndex, mergedROIs] : groupedMergedROIs) {
    assert(relativePackedCanvasIndex < inferencePlan.size());
    const auto& info = inferencePlan[relativePackedCanvasIndex];
    if (!mergedROIs.empty()) {
      packedCanvases.emplace_back(mergedROIs, info.size, info.device);
    }
  }
  time_us generatingTime = NowMicros();

  LOGD("[Schedule %d] Packing %lu MergedROIs "
       "// total=%lld packLastTime=%lld orderOthersTime=%lld packOthersTime=%lld groupingTime=%lld generatingTime=%lld",
       currID, orderedMergedROIs.size(),
  /*total=*/generatingTime - startTime,
  /*packLastTime=*/packLastTime - startTime,
  /*orderOthersTime=*/orderOthersTime - packLastTime,
  /*packOthersTime=*/packOthersTime - orderOthersTime,
  /*groupingTime=*/groupingTime - packOthersTime,
  /*generatingTime=*/generatingTime - groupingTime);
  return packedCanvases;
}

std::pair<IntPairs, IntPairs> ROIPacker::pack(
    const std::vector<std::vector<IntRect>>& freeRectsVec,
    const IntPairs& boxWHs, bool backward) const {
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
      if (executionType_ == EMULATED_BATCH) {
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
    if (executionType_ == EMULATED_BATCH) {
      packBox(copiedFreeRectsVec, roiSize_, roiSize_, pack_i, pack_j);
    } else {
      packBox(copiedFreeRectsVec, w, h, pack_i, pack_j);
    }
  }
  assert(packIndices.size() == packLocations.size());
  assert(packIndices.size() <= boxWHs.size());
  return {packIndices, packLocations};
}

void ROIPacker::apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                      const IntPairs& boxWH, const IntPairs& indices) const {
  assert(boxWH.size() == indices.size());
  for (int i = 0; i < boxWH.size(); i++) {
    auto [w, h] = boxWH[i];
    auto [pack_i, pack_j] = indices[i];
    if (executionType_ == EMULATED_BATCH) {
      packBox(freeRectsVec, roiSize_, roiSize_, pack_i, pack_j);
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
