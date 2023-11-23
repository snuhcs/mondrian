#ifndef PATCH_MIXER_HPP_
#define PATCH_MIXER_HPP_

#include <sstream>
#include <vector>

#include "mondrian/Config.hpp"
#include "mondrian/Frame.hpp"

namespace md {

class InferenceInfo;
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
            const ExecutionType executionType,
            const int roiSize,
            ROIResizer* roiResizer);

  void pack(const int currID,
            const MultiStream& streams,
            const std::vector<InferenceInfo>& inferencePlan,
            const Frame* fullFrameTarget);

  std::map<Device, std::vector<PackedCanvas>> generatePackedCanvases(
      const int currID,
      const MultiStream& streams,
      const std::vector<InferenceInfo>& inferencePlan,
      const Frame* fullFrameTarget);

 private:
  void processLastFrame(Frame* lastFrame,
                        std::map<Device, std::vector<std::vector<IntRect>>>& freeRectsVecTable,
                        const std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable);

  void processMergedROI(MergedROI* mergedROI,
                        std::map<Device, std::vector<std::vector<IntRect>>>& freeRectsVecTable,
                        const std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable);


  std::pair<IntPairs, IntPairs> pack(const std::vector<std::vector<IntRect>>& freeRectsVec,
                                     const IntPairs& boxWHs,
                                     bool backward = false) const;

  void apply(std::vector<std::vector<IntRect>>& freeRectsVec,
             const IntPairs& boxWH,
             const IntPairs& indices) const;

  static int getBestFitFreeRectIndex(const std::vector<IntRect>& freeRects, int w, int h);

  static void packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                      int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const IntRect& freeRect);

  static std::pair<IntRect, IntRect> splitFreeRect(int w, int h, const IntRect& freeRect);

  const ROIPackerConfig config_;
  const ExecutionType executionType_;
  const int roiSize_;
  ROIResizer* roiResizer_;
};

} // namespace md

#endif // PATCH_MIXER_HPP_
