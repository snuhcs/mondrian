#ifndef PATCH_MIXER_HPP_
#define PATCH_MIXER_HPP_

#include <sstream>
#include <vector>

#include "mondrian/Frame.hpp"

namespace md {

class PackedCanvas;

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

struct PackingResult {
  MultiStream streams;
  Frame* fullFrameTarget;
  std::vector<PackedCanvas> packedCanvases;
};

class ROIPacker {
 public:
  static PackingResult packCanvases(MultiStream streams,
                                    std::vector<InferenceInfo> inferencePlan,
                                    const int fullFrameVid,
                                    const ExecutionType executionType,
                                    const int roiSize,
                                    const ROIPrioritizerType roiPrioritizerType,
                                    const bool noDownsamplingForLast);

 private:
  static std::pair<IntPairs, IntPairs> pack(const std::vector<std::vector<IntRect>>& freeRectsVec,
                                            const IntPairs& boxWHs,
                                            bool backward,
                                            ExecutionType executionType,
                                            int roiSize);

  static void apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                    const IntPairs& boxWH,
                    const IntPairs& indices,
                    ExecutionType executionType,
                    int roiSize);

  static int getBestFitFreeRectIndex(const std::vector<IntRect>& freeRects, int w, int h);

  static void packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                      int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);
};

} // namespace md

#endif // PATCH_MIXER_HPP_
