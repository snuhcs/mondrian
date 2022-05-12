#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <utility>

#include "opencv2/core/mat.hpp"

#include "Config.hpp"
#include "DataType.hpp"

namespace rm {

class RoIExtractor {
 public:
  RoIExtractor(RoIExtractorConfig config);

  bool useOpticalFlowRoIs() const;

  void process(const std::pair<std::pair<std::shared_ptr<Frame>, std::shared_ptr<Frame>>, std::vector<BoundingBox>>& item) const;

 private:
  static void mergeSingleFrameRoIs(std::vector<RoI>& rois, const Frame* frame,
                                   const float mergeThreshold, const int maxMergedRoISize);

  static std::vector<RoI> getOpticalFlowRoIs(const Frame *prevFrame, Frame *currFrame,
                                             const std::vector<BoundingBox> &boundingBoxes,
                                             const cv::Size &targetSize,
                                             cv::Mat &prevMat, cv::Mat &currMat);

  static std::vector<std::pair<int, int>>
  getBoundingBoxShifts(const cv::Mat &prevImage, const cv::Mat &currImage,
                       const std::vector<Rect> &boundingBoxes,
                       const cv::Size &targetSize, Frame *currFrame);

  static std::vector<RoI> getPixelDiffRoIs(
          const Frame* prevFrame, Frame *currFrame, const cv::Size& targetSize,
          const int mixRoIArea);

  static cv::Mat calculateDiffAndThreshold(
      const cv::Mat& prevMat, const cv::Mat& currMat);

  static void cannyEdgeDetection(cv::Mat mat);

  const RoIExtractorConfig mConfig;
  const cv::Size mTargetSize;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
