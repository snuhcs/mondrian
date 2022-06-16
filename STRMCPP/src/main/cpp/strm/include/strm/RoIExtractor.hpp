#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <utility>

#include "opencv2/core/mat.hpp"

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/RoIPrioritizer.hpp"
#include "strm/ResizeProfile.hpp"

namespace rm {

class RoIExtractor {
 public:
  RoIExtractor(RoIExtractorConfig config, const ResizeProfile* resizeProfile,
               const RoIPrioritizer* roIPrioritizer);

  bool useOpticalFlowRoIs() const;

  std::vector<RoI> process(Frame* prevFrame, Frame* currFrame,
                           const std::vector<BoundingBox>& prevResults) const;

 private:
  static void mergeSingleFrameRoIs(std::vector<RoI>& rois, const float mergeThreshold,
                                   const int maxMergedRoISize);

  static std::vector<RoI> getOpticalFlowRoIs(
      const Frame* prevFrame, const Frame* currFrame,
      const std::vector<BoundingBox>& boundingBoxes, const cv::Size& targetSize);

  static std::vector<std::pair<std::pair<int, int>, float>> getShiftAndErrors(
      const Frame* prevFrame, const Frame* currFrame,
      const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize);

  static std::vector<RoI> getPixelDiffRoIs(
      const Frame* prevFrame, const Frame* currFrame, const cv::Size& targetSize,
      const int mixRoIArea);

  static cv::Mat calculateDiffAndThreshold(
      const cv::Mat& prevMat, const cv::Mat& currMat);

  static void cannyEdgeDetection(cv::Mat mat);

  const RoIExtractorConfig mConfig;
  const cv::Size mTargetSize;

  const RoIPrioritizer* mRoIPrioritizer;
  const ResizeProfile* mResizeProfile;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
