#include "strm/BinPacker.hpp"

#include <sstream>

#include "strm/Log.hpp"

namespace rm {

BinPacker::BinPacker(const std::vector<std::pair<int, int>>& WHs) {
  for (auto[w, h]: WHs) {
    freeRectsVec.push_back({IntRect(0, 0, w, h)});
  }
}

std::vector<std::pair<int, int>> BinPacker::pack(const std::vector<std::pair<int, int>>& boxWHs,
                                                 bool reverse) const {
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

std::vector<std::pair<int, int>> BinPacker::apply(const std::vector<std::pair<int, int>>& boxes,
                                                  const std::vector<std::pair<int, int>>& packIndices) {
  assert(boxes.size() == packIndices.size());
  std::vector<std::pair<int, int>> packedWHs;
  for (int i = 0; i < boxes.size(); i++) {
    auto[w, h] = boxes[i];
    auto[pack_i, pack_j] = packIndices[i];
    packedWHs.push_back(packBox(freeRectsVec, w, h, pack_i, pack_j));
  }
  return packedWHs;
}

void BinPacker::restore(const std::vector<std::pair<int, int>>& boxes,
                        const std::vector<std::pair<int, int>>& packIndices) {
  assert(boxes.size() == packIndices.size());
  for (int i = int(boxes.size()) - 1; i >= 0; i--) {
    auto[w, h] = boxes[i];
    auto[pack_i, pack_j] = packIndices[i];
    restoreBox(freeRectsVec, w, h, pack_i, pack_j);
  }
}

std::pair<int, int> BinPacker::packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                                       int w, int h, int pack_i, int pack_j) {
  assert(pack_i < freeRectsVec.size() && pack_j < freeRectsVec[pack_i].size());
  IntRect freeRectToPack = freeRectsVec[pack_i][pack_j];
  freeRectsVec[pack_i].erase(freeRectsVec[pack_i].begin() + pack_j);
  auto[rect0, rect1] = splitFreeRect(w, h, freeRectToPack);
  freeRectsVec[pack_i].push_back(rect0);
  freeRectsVec[pack_i].push_back(rect1);
  return {freeRectToPack.left, freeRectToPack.top};
}

void BinPacker::restoreBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                           int w, int h, int pack_i, int pack_j) {
  IntRect rect0 = *(freeRectsVec[pack_i].rbegin() + 1); // Last - 1 rect
  IntRect rect1 = *(freeRectsVec[pack_i].rbegin());     // Last     rect
  freeRectsVec[pack_i].pop_back();                      // Remove rect1
  freeRectsVec[pack_i].pop_back();                      // Remove rect0
  // Reverse order BinPacker::splitFreeRect
  assert(rect0.bottom == rect1.bottom || rect0.right == rect1.right);
  if (rect0.bottom == rect1.bottom) {
    assert(rect0.left - w == rect1.left);
    assert(rect0.top == rect1.top - h);
    assert(rect0.left - w == rect1.right - w);
  } else { // rect0.right == rect1.right
    assert(rect0.left == rect1.left - w);
    assert(rect0.top - h == rect1.top);
    assert(rect0.top - h == rect1.bottom - h);
  }
  if (rect0.bottom == rect1.bottom) {
    IntRect mergedFreeRect(rect1.left, rect0.top, rect0.right, rect0.bottom);
    freeRectsVec[pack_i].insert(freeRectsVec[pack_i].begin() + pack_j, mergedFreeRect);
  } else { // rect0.right == rect1.right
    IntRect mergedFreeRect(rect0.left, rect1.top, rect0.right, rect0.bottom);
    freeRectsVec[pack_i].insert(freeRectsVec[pack_i].begin() + pack_j, mergedFreeRect);
  }
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

std::string BinPacker::toString() const {
  std::stringstream ss;
  for (auto& freeRects : freeRectsVec) {
    ss << std::endl;
    for (auto& freeRect : freeRects) {
      ss << freeRect.toString() << ", ";
    }
  }
  return ss.str();
}

void BinPacker::restoreTest() {
  BinPacker binPacker({{640, 640}, {640, 640}});
  std::vector<std::pair<int, int>> boxes0 = {{20, 30}, {189, 83}, {92, 30}, {39, 40}, {20, 39}};
  std::vector<std::pair<int, int>> boxes1 = {{29, 20}, {2, 82}, {280, 38}, {29, 290}, {20, 24}};
  LOGD("XXX 1 %s", binPacker.toString().c_str());
  auto packIndices0 = binPacker.pack(boxes0, false);
  auto packedLocations0 = binPacker.apply(boxes0, packIndices0);
  LOGD("XXX 2 %s", binPacker.toString().c_str());

  auto packIndices1 = binPacker.pack(boxes1, false);
  auto packedLocation1 = binPacker.apply(boxes1, packIndices1);
  LOGD("XXX 3 %s", binPacker.toString().c_str());
  binPacker.restore(boxes1, packIndices1);
  LOGD("XXX 4 %s", binPacker.toString().c_str());
}

} // namespace rm
