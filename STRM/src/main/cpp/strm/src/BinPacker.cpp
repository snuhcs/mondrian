#include "strm/BinPacker.hpp"

#include <sstream>

#include "strm/Log.hpp"

namespace rm {

std::tuple<IntPairs, IntPairs> BinPacker::pack(
    const std::vector<std::vector<IntRect>>& freeRectsVec,
    const IntPairs& boxWHs, bool backward) {
  LOGD("BinPacker::pack(# boxes=%lu, backward=%d)", boxWHs.size(), backward);
  printFreeRects(freeRectsVec);
  LOGD("===== Try Pack Boxes =====");
  for (const auto&[w, h]: boxWHs) {
    LOGD("(%3d, %3d)", w, h);
  }
  LOGD("=====   Boxes End    =====");

  auto copiedFreeRectsVec = freeRectsVec;
  IntPairs packIndices;
  IntPairs packLocations;
  for (const auto&[w, h]: boxWHs) {
    int minRemainingArea = INT_MAX / 2;
    int pack_i = -1;
    int pack_j = -1;
    for (int _i = 0; _i < copiedFreeRectsVec.size(); _i++) {
      int i = backward
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
      LOGD("===== Try Pack Failed =====");
      assert(packIndices == packLocations);
      return {packIndices, packLocations};
    } else {
      packIndices.emplace_back(pack_i, pack_j);
      const IntRect& rect = copiedFreeRectsVec[pack_i][pack_j];
      packLocations.emplace_back(rect.left, rect.top);
    }
    packBox(copiedFreeRectsVec, w, h, pack_i, pack_j);
  }
  LOGD("===== Try Pack Success =====");
  assert(packIndices == packLocations);
  return {packIndices, packLocations};
}

void BinPacker::apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                      const BoxIndices& boxIndices) {
  LOGD("BinPacker::apply(%lu)", boxIndices.boxes.size());
  printFreeRects(freeRectsVec);
  const auto& boxes = boxIndices.boxes;
  const auto& packIndices = boxIndices.indices;
  assert(boxes.size() == packIndices.size());
  LOGD("=====   Apply boxes   =====");
  for (int i = 0; i < boxes.size(); i++) {
    auto[w, h] = boxes[i];
    auto[pack_i, pack_j] = packIndices[i];
    LOGD("(%3d, %3d), (%3d, %3d)", w, h, pack_i, pack_j);
    packBox(freeRectsVec, w, h, pack_i, pack_j);
  }
  LOGD("=====    Apply End    =====");
  printFreeRects(freeRectsVec);
}

void BinPacker::packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                        int w, int h, int pack_i, int pack_j) {
  assert(pack_i < freeRectsVec.size() && pack_j < freeRectsVec[pack_i].size());
  IntRect freeRectToPack = freeRectsVec[pack_i][pack_j];
  freeRectsVec[pack_i].erase(freeRectsVec[pack_i].begin() + pack_j);
  auto[rect0, rect1] = splitFreeRect(w, h, freeRectToPack);
  freeRectsVec[pack_i].push_back(rect0);
  freeRectsVec[pack_i].push_back(rect1);
  return {freeRectToPack.left, freeRectToPack.top};
}

bool BinPacker::canFit(int w, int h, const IntRect& freeRect) {
  return w <= freeRect.width() && h <= freeRect.height();
}

std::pair<IntRect, IntRect> BinPacker::splitFreeRect(int w, int h, const IntRect& freeRect) {
  if (freeRect.width() > freeRect.height()) {
    return {IntRect(freeRect.left + w, freeRect.top, freeRect.right, freeRect.bottom),
            IntRect(freeRect.left, freeRect.top + h, freeRect.left + w, freeRect.bottom)};
  } else {
    return {IntRect(freeRect.left, freeRect.top + h, freeRect.right, freeRect.bottom),
            IntRect(freeRect.left + w, freeRect.top, freeRect.right, freeRect.top + h)};
  }
}

void BinPacker::printFreeRects(const std::vector<std::vector<IntRect>>& freeRectsVec) {
  for (int i = 0; i < freeRectsVec.size(); i++) {
    LOGD("===== %3d FreeRects =====", i);
    for (const auto& freeRect: freeRectsVec[i]) {
      LOGD("%s", freeRect.toString().c_str());
    }
  }
  LOGD("===== FreeRects End =====");
}

//void BinPacker::restore(std::vector<std::vector<IntRect>>& freeRectsVec,
//                        const BoxIndices& boxIndices) {
//  LOGD("BinPacker::restore(# boxes=%lu)", boxIndices.boxes.size());
//  LOGD("=====    Before    =====");
//  printFreeRects(freeRectsVec);
//  const auto& boxes = boxIndices.boxes;
//  const auto& packIndices = boxIndices.indices;
//  assert(boxes.size() == packIndices.size());
//  for (int i = int(boxes.size()) - 1; i >= 0; i--) {
//    auto[w, h] = boxes[i];
//    auto[pack_i, pack_j] = packIndices[i];
//    restoreBox(freeRectsVec, w, h, pack_i, pack_j);
//  }
//  LOGD("=====    After     =====");
//  printFreeRects(freeRectsVec);
//}

//void BinPacker::restoreBox(std::vector<std::vector<IntRect>>& freeRectsVec,
//                           int w, int h, int pack_i, int pack_j) {
//  IntRect rect0 = *(freeRectsVec[pack_i].rbegin() + 1); // Last - 1 rect
//  IntRect rect1 = *(freeRectsVec[pack_i].rbegin());     // Last     rect
//  freeRectsVec[pack_i].pop_back();                      // Remove rect1
//  freeRectsVec[pack_i].pop_back();                      // Remove rect0
//  // Reverse order BinPacker::splitFreeRect
//  assert(rect0.bottom == rect1.bottom || rect0.right == rect1.right);
//  if (rect0.bottom == rect1.bottom) {
//    assert(rect0.left - w == rect1.left);
//    assert(rect0.top == rect1.top - h);
//    assert(rect0.left - w == rect1.right - w);
//  } else { // rect0.right == rect1.right
//    assert(rect0.left == rect1.left - w);
//    assert(rect0.top - h == rect1.top);
//    assert(rect0.top - h == rect1.bottom - h);
//  }
//  if (rect0.bottom == rect1.bottom) {
//    IntRect mergedFreeRect(rect1.left, rect0.top, rect0.right, rect0.bottom);
//    freeRectsVec[pack_i].insert(freeRectsVec[pack_i].begin() + pack_j, mergedFreeRect);
//  } else { // rect0.right == rect1.right
//    IntRect mergedFreeRect(rect0.left, rect1.top, rect0.right, rect0.bottom);
//    freeRectsVec[pack_i].insert(freeRectsVec[pack_i].begin() + pack_j, mergedFreeRect);
//  }
//}

} // namespace rm
