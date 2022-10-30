#ifndef BIN_PACKER_HPP_
#define BIN_PACKER_HPP_

#include <sstream>
#include <vector>

namespace rm {

class SpatioTemporalRoIMixer;

using IntPairs = std::vector<std::pair<int, int>>;

struct BoxIndices {
  IntPairs boxes;
  IntPairs indices;
};

class BinPacker {
  friend SpatioTemporalRoIMixer;
 public:
  BinPacker(const IntPairs& WHs);

  /*
   * 1. Simulate packing with BinPacker::pack(boxes, reverse=false)
   *   * auto packIndices = pack(...);
   *   * You can check that the all boxes are packed.
   *     * The number of not packed boxes: boxes.size() - packIndices.size()
   * 2. Apply simulated indices with BinPacker::apply(packIndices)
   *   * apply(packIndices);
   *   * return packedWHs
   */
  IntPairs pack(const IntPairs& boxes, bool reverse) const;

  IntPairs apply(const BoxIndices& boxIndices);

  void restore(const BoxIndices& boxIndices);

  std::string toString() const;

 private:
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

  static std::pair<int, int> packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                                     int w, int h, int pack_i, int pack_j);

  static void restoreBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                         int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);

  std::vector<std::vector<IntRect>> freeRectsVec;

  static void restoreTest();
};

} // namespace rm

#endif // BIN_PACKER_HPP_
