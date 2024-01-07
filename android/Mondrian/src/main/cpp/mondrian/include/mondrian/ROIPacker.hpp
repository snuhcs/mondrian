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
                        std::map<Device, std::vector<std::vector<cv::Rect2i>>>& freeRectsVecTable,
                        const std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable);

  void processMergedROI(MergedROI* mergedROI,
                        std::map<Device, std::vector<std::vector<cv::Rect2i>>>& freeRectsVecTable,
                        const std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable);

  std::pair<Indices, std::vector<cv::Point2i>> pack(
      const std::vector<std::vector<cv::Rect2i>>& freeRectsVec,
      const std::vector<cv::Size2i>& boxWHs,
      bool backward = false) const;

  void apply(std::vector<std::vector<cv::Rect2i>>& freeRectsVec,
             const std::vector<cv::Size2i>& boxWH,
             const Indices& indices) const;

  static int getBestFitFreeRectIndex(const std::vector<cv::Rect2i>& freeRects, int w, int h);

  static void packBox(std::vector<std::vector<cv::Rect2i>>& freeRectsVec,
                      int w, int h, int pack_i, int pack_j);

  static bool canFit(int w, int h, const cv::Rect2i& freeRect);

  static std::pair<cv::Rect2i, cv::Rect2i> splitFreeRect(int w, int h, const cv::Rect2i& freeRect);

  const ROIPackerConfig config_;
  const ExecutionType executionType_;
  const int roiSize_;
  ROIResizer* roiResizer_;
};

} // namespace md

#endif // PATCH_MIXER_HPP_
