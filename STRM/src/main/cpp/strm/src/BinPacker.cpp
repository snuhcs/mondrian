#include "strm/BinPacker.hpp"

namespace rm {

BinPacker::BinPacker(const std::vector<std::pair<int, int>>& WHs) {
  for (auto[w, h]: WHs) {
    freeRectsVec.push_back({IntRect(0, 0, w, h)});
  }
}

bool BinPacker::pack(const std::vector<std::pair<int, int>>& boxWHs, bool reverse) {
  std::lock_guard<std::mutex> lock(mtx);
  auto copiedFreeRectsVec = freeRectsVec;
  for (const auto& wh: boxWHs) {
    const int w = wh.first;
    const int h = wh.second;
    int minRemainingArea = INT_MAX / 2;
    int pack_i = -1;
    int pack_j = -1;
    for (int _i = 0; _i < freeRectsVec.size(); _i++) {
      int i = reverse
              ? int(freeRectsVec.size()) - 1 - _i
              : _i;
      const auto& freeRects = freeRectsVec[i];
      for (int j = 0; j < freeRects.size(); j++) {
        const IntRect& freeRect = freeRects[j];
        if (canFit(w, h, freeRect)) {
          int remainingArea = w * h - freeRect.area();
          if (remainingArea < minRemainingArea) {
            minRemainingArea = remainingArea;
            pack_j = j;
          }
        }
      }
      if (pack_j != -1) {
        pack_i = i;
        break;
      }
    }
    if (pack_i == -1) {
      assert(pack_j == -1);
      return false;
    }
    IntRect freeRectToPack = copiedFreeRectsVec[pack_i][pack_j];
    copiedFreeRectsVec[pack_i].erase(copiedFreeRectsVec[pack_i].begin() + pack_j);
    auto[rect1, rect2] = splitFreeRect(w, h, freeRectToPack);
    copiedFreeRectsVec[pack_i].push_back(rect1);
    copiedFreeRectsVec[pack_i].push_back(rect2);
  }
  freeRectsVec = copiedFreeRectsVec;
  return true;
}

bool BinPacker::canFit(int w, int h, const IntRect& freeRect) {
  return w <= freeRect.width() && h <= freeRect.height();
}

std::pair<BinPacker::IntRect, BinPacker::IntRect>
BinPacker::splitFreeRect(int w, int h, const IntRect& freeRect) {
  if (freeRect.width() > freeRect.height()) {
    return {IntRect(freeRect.left + w, freeRect.top, freeRect.right, freeRect.bottom),
            IntRect(freeRect.left, freeRect.top + h, freeRect.left + w, freeRect.bottom)};
  } else {
    return {IntRect(freeRect.left, freeRect.top + h, freeRect.right, freeRect.bottom),
            IntRect(freeRect.left + w, freeRect.top, freeRect.right, freeRect.top + h)};
  }
}

} // namespace rm
