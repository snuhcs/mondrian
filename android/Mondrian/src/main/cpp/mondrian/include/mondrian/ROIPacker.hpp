#ifndef PATCH_MIXER_HPP_
#define PATCH_MIXER_HPP_

#include <sstream>
#include <vector>

#include "mondrian/Config.hpp"
#include "mondrian/Frame.hpp"

namespace md {

class PackedCanvas;
class ROIResizer;

struct IntRect {
  int l;
  int t;
  int r;
  int b;
  int w;
  int h;
  int area;

  IntRect(int l, int t, int r, int b)
      : l(l), t(t), r(r), b(b), w(r - l), h(b - t), area((r - l) * (b - t)) {};

  std::string str() const {
    std::stringstream ss;
    ss << "(" << l << ", " << t << ", " << r << ", " << b << ")";
    return ss.str();
  }
};

class ROIPacker {
 public:
  ROIPacker(const ROIPackerConfig& config,
            ExecutionType executionType,
            int roiSize,
            ROIResizer* roiResizer);

  std::vector<PackedCanvas> packROIs(
      MultiStream& frames,
      const std::vector<InferenceInfo>& inferencePlan,
      const Frame* fullFrame);

 private:
  void prepareFrameLast(Frame* frame,
                        const IntPairs& indices,
                        const IntPairs& locations);

  IntPairs getBoxesIfLast(const Frame* frame);

  void prepareFrameScaled(Frame* frame,
                          const IntPairs& indices,
                          const IntPairs& locations);

  static IntPairs getBoxesIfScaled(const Frame* frame);

  std::pair<IntPairs, IntPairs> pack(
      const std::vector<std::vector<IntRect>>& freeRectsVec,
      const IntPairs& boxWHs,
      bool backward) const;

  void apply(
      std::vector<std::vector<IntRect>>& freeRectsVec,
      const IntPairs& boxWH,
      const IntPairs& indices) const;

  static int getBestFitFreeRectIndex(const std::vector<IntRect>& freeRects, int w, int h);

  static void packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                      int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);

  ROIPackerConfig config_;
  ExecutionType executionType_;
  int roiSize_;
  ROIResizer* roiResizer_;
};

} // namespace md

#endif // PATCH_MIXER_HPP_
