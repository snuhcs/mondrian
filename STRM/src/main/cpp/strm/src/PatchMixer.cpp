#include "strm/PatchMixer.hpp"

#include <sstream>

namespace rm {

std::tuple<IntPairs, IntPairs> PatchMixer::pack(
    const std::vector<std::vector<IntRect>>& freeRectsVec,
    const IntPairs& boxWHs, bool backward, bool emulatedBatch, int singleInputSize) {
  auto copiedFreeRectsVec = freeRectsVec;
  IntPairs packIndices;
  IntPairs packLocations;
  for (const auto&[w, h]: boxWHs) {
    int pack_i = -1;
    int pack_j = -1;
    for (int _i = 0; _i < copiedFreeRectsVec.size(); _i++) {
      int i = backward
              ? int(copiedFreeRectsVec.size()) - 1 - _i
              : _i;
      if (emulatedBatch) {
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
    packLocations.emplace_back(rect.left, rect.top);
    if (emulatedBatch) {
      packBox(copiedFreeRectsVec, singleInputSize, singleInputSize, pack_i, pack_j);
    } else {
      packBox(copiedFreeRectsVec, w, h, pack_i, pack_j);
    }
  }
  assert(packIndices.size() == packLocations.size());
  return {packIndices, packLocations};
}

void PatchMixer::apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                       const WHs& boxWH, const Indices& indices,
                       bool emulatedBatch, int singleInputSize) {
  assert(boxWH.size() == indices.size());
  for (int i = 0; i < boxWH.size(); i++) {
    auto[w, h] = boxWH[i];
    auto[pack_i, pack_j] = indices[i];
    if (emulatedBatch) {
      packBox(freeRectsVec, singleInputSize, singleInputSize, pack_i, pack_j);
    } else {
      packBox(freeRectsVec, w, h, pack_i, pack_j);
    }
  }
}

int PatchMixer::getBestFitFreeRectIndex(const std::vector<IntRect>& freeRects, int w, int h) {
  int minRemainingArea = INT_MAX / 2;
  int best_index = -1;
  for (int i = 0; i < freeRects.size(); i++) {
    const IntRect& freeRect = freeRects[i];
    if (canFit(w, h, freeRect)) {
      int remainingArea = freeRect.area() - w * h;
      if (minRemainingArea > remainingArea) {
        minRemainingArea = remainingArea;
        best_index = i;
      }
    }
  }
  return best_index;
}

void PatchMixer::packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                        int w, int h, int pack_i, int pack_j) {
  assert(pack_i < freeRectsVec.size() && pack_j < freeRectsVec[pack_i].size());
  IntRect freeRectToPack = freeRectsVec[pack_i][pack_j];
  freeRectsVec[pack_i].erase(freeRectsVec[pack_i].begin() + pack_j);
  auto[rect0, rect1] = splitFreeRect(w, h, freeRectToPack);
  if (rect0.area() > 0) {
    freeRectsVec[pack_i].push_back(rect0);
  }
  if (rect1.area() > 0) {
    freeRectsVec[pack_i].push_back(rect1);
  }
}

bool PatchMixer::canFit(int w, int h, const IntRect& freeRect) {
  return w <= freeRect.width() && h <= freeRect.height();
}

std::pair<IntRect, IntRect> PatchMixer::splitFreeRect(int w, int h, const IntRect& freeRect) {
  if (freeRect.width() > freeRect.height()) {
    return {IntRect(freeRect.left + w, freeRect.top, freeRect.right, freeRect.bottom),
            IntRect(freeRect.left, freeRect.top + h, freeRect.left + w, freeRect.bottom)};
  } else {
    return {IntRect(freeRect.left, freeRect.top + h, freeRect.right, freeRect.bottom),
            IntRect(freeRect.left + w, freeRect.top, freeRect.right, freeRect.top + h)};
  }
}

} // namespace rm
