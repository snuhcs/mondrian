#include "strm/BinPacker.hpp"

namespace rm {

BinPacker::BinPacker(const std::vector<std::pair<int, int>>& WHs) {
  for (auto[w, h]: WHs) {
    freeRectsVec.push_back({IntRect(0, 0, w, h)});
  }
}

std::vector<std::pair<int, int>> BinPacker::pack(const std::vector<std::pair<int, int>>& boxWHs,
                                                 bool reverse) {
  auto copiedFreeRectsVec = freeRectsVec;
  std::vector<std::pair<int, int>> packIndices;
  for (const auto&[w, h]: boxWHs) {
    int minRemainingArea = INT_MAX / 2;
    int pack_i = -1;
    int pack_j = -1;
    for (int _i = 0; _i < copiedFreeRectsVec.size(); _i++) {
      int i = reverse
              ? int(copiedFreeRectsVec.size()) - 1 - _i
              : _i;
      for (int j = 0; j < copiedFreeRectsVec[i].size(); j++) {
        const IntRect& freeRect = copiedFreeRectsVec[i][j];
        if (canFit(w, h, freeRect)) {
          int remainingArea = freeRect.area() - w * h;
          if (minRemainingArea > remainingArea) {
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
      return packIndices;
    } else {
      packIndices.emplace_back(pack_i, pack_j);
    }
    packBox(copiedFreeRectsVec, w, h, pack_i, pack_j);
  }
  return packIndices;
}

void BinPacker::apply(const std::vector<std::pair<int, int>>& boxes,
                      const std::vector<std::pair<int, int>>& packIndices) {
  assert(boxes.size() == packIndices.size());
  for (int i = 0; i < boxes.size(); i++) {
    auto[w, h] = boxes[i];
    auto[pack_i, pack_j] = packIndices[i];
    packBox(freeRectsVec, w, h, pack_i, pack_j);
  }
}

void BinPacker::packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                        int w, int h, int pack_i, int pack_j) {
  assert(pack_i < freeRectsVec.size() && pack_j < freeRectsVec[pack_i].size());
  IntRect freeRectToPack = freeRectsVec[pack_i][pack_j];
  freeRectsVec[pack_i].erase(freeRectsVec[pack_i].begin() + pack_j);
  auto[rect1, rect2] = splitFreeRect(w, h, freeRectToPack);
  freeRectsVec[pack_i].push_back(rect1);
  freeRectsVec[pack_i].push_back(rect2);
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
