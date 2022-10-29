#ifndef BIN_PACKER_HPP_
#define BIN_PACKER_HPP_

#include <mutex>
#include <vector>

namespace rm {

class BinPacker {
 public:
  BinPacker(const std::vector<std::pair<int, int>>& WHs);

  bool pack(const std::vector<std::pair<int, int>>& boxes, bool reverse);

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

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);

  std::vector<std::vector<IntRect>> freeRectsVec;
  std::mutex mtx;
};

} // namespace rm

#endif // BIN_PACKER_HPP_
