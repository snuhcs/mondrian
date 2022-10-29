#ifndef BIN_PACKER_HPP_
#define BIN_PACKER_HPP_

#include <vector>

namespace rm {

class BinPacker {
 public:
  BinPacker(const std::vector<std::pair<int, int>>& WHs);

  /*
   * 1. Simulate packing with BinPacker::pack(boxes, reverse=false)
   *   * auto packIndices = pack(...);
   *   * You can check that the all boxes are packed.
   *     * The number of not packed boxes: boxes.size() - packIndices.size()
   * 2. Apply simulated indices with BinPacker::apply(packIndices)
   *   * apply(packIndices);
   */
  std::vector<std::pair<int, int>> pack(const std::vector<std::pair<int, int>>& boxes,
                                        bool reverse);

  void apply(const std::vector<std::pair<int, int>>& boxes,
             const std::vector<std::pair<int, int>>& packIndices);

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
  };

  static void packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                      int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);

  std::vector<std::vector<IntRect>> freeRectsVec;
};

} // namespace rm

#endif // BIN_PACKER_HPP_
