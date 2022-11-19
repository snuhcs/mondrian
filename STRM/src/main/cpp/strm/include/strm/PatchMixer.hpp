#ifndef PATCH_MIXER_HPP_
#define PATCH_MIXER_HPP_

#include <sstream>
#include <vector>

#include "strm/DataType.hpp"

namespace rm {

class SpatioTemporalRoIMixer;

using WHs = IntPairs;
using Indices = IntPairs;
using Locations = IntPairs;

struct IntRect {
  int left;
  int top;
  int right;
  int bottom;

  IntRect(int left, int top, int right, int bottom)
      : left(left), top(top), right(right), bottom(bottom) {};

  int width() const {
    return right - left;
  }

  int height() const {
    return bottom - top;
  }

  int area() const {
    return width() * height();
  }

  std::string toString() const {
    std::stringstream ss;
    ss << "(" << left << ", " << top << ", " << right << ", " << bottom << ")";
    return ss.str();
  }
};

class PatchMixer {
 public:
  static std::tuple<Indices, Locations> pack(const std::vector<std::vector<IntRect>>& freeRectsVec,
                                             const WHs& boxWHs, bool backward,
                                             bool emulatedBatch, int singleInputSize);

  static void apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                    const WHs& boxWH, const Indices& indices,
                    bool emulatedBatch, int singleInputSize);

 private:
  static int getBestFitFreeRectIndex(const std::vector<IntRect>& freeRects, int w, int h);

  static void packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                      int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
