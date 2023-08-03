#include "mondrian/ROIPacker.hpp"

#include <numeric>
#include <sstream>

#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/ROIPrioritizer.hpp"

namespace md {

ROIPacker::ROIPacker(const ROIPackerConfig& config,
                     ExecutionType executionType,
                     int roiSize,
                     ROIResizer* roiResizer)
    : config_(config),
      executionType_(executionType),
      roiSize_(roiSize),
      roiResizer_(roiResizer) {}

std::vector<PackedCanvas> ROIPacker::packROIs(MultiStream& frames,
                                              const std::vector<InferenceInfo>& inferencePlan,
                                              const Frame* fullFrame) {
  time_us startTime = NowMicros();

  std::vector<std::vector<IntRect>> freeRectsVec;
  for (const auto& info : inferencePlan) {
    freeRectsVec.push_back({{0, 0, info.size, info.size}});
  }

  // Last frames
  for (const auto& [vid, aStreamFrames] : frames) {
    if (fullFrame != nullptr && vid == fullFrame->vid) {
      continue;
    }
    Frame* lastFrame = *aStreamFrames.rbegin();
    IntPairs boxesIfLast = lastFrame->boxesIfLast(
        /*roiResizer=*/roiResizer_,
        /*executionType=*/executionType_,
        /*noDownsample=*/config_.NO_DOWNSAMPLING_FOR_LAST_FRAME);
    if (executionType_ == ROI_WISE_INFERENCE) {
      for (auto& mergedROI : lastFrame->mergedROIs) {
        auto [bw, bh] = mergedROI->borderedMatWH();
        if (!freeRectsVec.empty()) {
          mergedROI->setPackInfo(
              /*xy=*/{0, 0},
              /*relativePackedCanvasIndex=*/(int) freeRectsVec.size() - 1,
              /*executionType=*/executionType_,
              /*roiSize=*/roiSize_);
          freeRectsVec.erase(freeRectsVec.end() - 1);
        }
      }
    } else { // MONDRIAN, EMULATED_BATCH
      auto [indices, locations] = ROIPacker::pack(
          /*freeRectsVec=*/freeRectsVec,
          /*boxWHs=*/boxesIfLast,
          /*backward=*/false);
      bool fullyPacked = indices.size() == boxesIfLast.size();
      int maxPackedCanvasIndex = -1;
      for (auto& [packedCanvasIndex, freeRectIndex] : indices) {
        maxPackedCanvasIndex = std::max(maxPackedCanvasIndex, packedCanvasIndex);
      }
      if (fullyPacked) {
        ROIPacker::apply(freeRectsVec, boxesIfLast, indices);
      } else {
        indices.resize(boxesIfLast.size());
        locations.resize(boxesIfLast.size());
        ROIPacker::apply(freeRectsVec, boxesIfLast, indices);
      }
      lastFrame->prepareFrameLast(
          /*indices=*/indices,
          /*locations=*/locations,
          /*executionType=*/executionType_,
          /*roiSize=*/roiSize_,
          /*noDownsample=*/config_.NO_DOWNSAMPLING_FOR_LAST_FRAME);
    }
  }
  time_us packLastTime = NowMicros();

  // Order MergedROIs
  auto orderedMergedROIs = ROIPrioritizer::order(frames, fullFrame, config_.TYPE);
  time_us orderTime = NowMicros();

  // Pack MergedROIs
  for (MergedROI* mergedROI : orderedMergedROIs) {
    if (executionType_ == ROI_WISE_INFERENCE) {
      if (!freeRectsVec.empty()) {
        mergedROI->setPackInfo(
            /*xy=*/{0, 0},
            /*relativePackedCanvasIndex=*/(int) freeRectsVec.size() - 1,
            /*executionType=*/executionType_,
            /*roiSize=*/roiSize_);
        freeRectsVec.erase(freeRectsVec.end() - 1);
      }
    } else {
      auto [bw, bh] = mergedROI->borderedMatWH();
      auto [indices, locations] = ROIPacker::pack(
          /*freeRectsVec=*/freeRectsVec,
          /*boxWHs=*/{{bw, bh}},
          /*backward=*/false);
      if (!indices.empty()) {
        ROIPacker::apply(freeRectsVec, {{bw, bh}}, indices);
        mergedROI->setPackInfo(
            /*xy=*/locations[0],
            /*relativePackedCanvasIndex=*/indices[0].first,
            /*executionType=*/executionType_,
            /*roiSize=*/roiSize_);
      }
    }
  }
  time_us packOthersTime = NowMicros();

  LOGD("Packing %d Frames with %lu ROIs | "
       "total: %lld, packLastTime: %lld, orderTime: %lld, packOthersTime: %lld",
       std::accumulate(frames.begin(), frames.end(), 0,
                       [](int sum, const auto& kv) {
                         return sum + kv.second.size();
                       }),
       orderedMergedROIs.size(),
       packOthersTime - startTime,
       packLastTime - startTime,
       orderTime - packLastTime,
       packOthersTime - orderTime);
}

std::pair<IntPairs, IntPairs> ROIPacker::pack(
    const std::vector<std::vector<IntRect>>& freeRectsVec,
    const IntPairs& boxWHs,
    bool backward) const {
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

void ROIPacker::apply(
    std::vector<std::vector<IntRect>>& freeRectsVec,
    const IntPairs& boxWH,
    const IntPairs& indices) const {
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
