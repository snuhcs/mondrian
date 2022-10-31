#ifndef BIN_PACKER_HPP_
#define BIN_PACKER_HPP_

#include <sstream>
#include <vector>

namespace rm {

class SpatioTemporalRoIMixer;

using IntPairs = std::vector<std::pair<int, int>>;

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

struct BoxIndices {
  IntPairs boxes;
  IntPairs indices;
};

class BinPacker {
 public:
  static std::tuple<IntPairs, IntPairs> pack(const std::vector<std::vector<IntRect>>& freeRectsVec,
                                             const IntPairs& boxes, bool backward);

  static void apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                    const BoxIndices& boxIndices);

 private:
  static void printFreeRects(const std::vector<std::vector<IntRect>>& freeRectsVec);

  static void packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                      int w, int h, int pack_i, int pack_j);

//  static void restore(std::vector<std::vector<IntRect>>& freeRectsVec,
//                      const BoxIndices& boxIndices);
//
//  static void restoreBox(std::vector<std::vector<IntRect>>& freeRectsVec,
//                         int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);
};

} // namespace rm

#endif // BIN_PACKER_HPP_
