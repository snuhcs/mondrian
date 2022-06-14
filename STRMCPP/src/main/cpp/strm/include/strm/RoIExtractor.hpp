#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <deque>
#include <queue>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/ResizeProfile.hpp"
#include "strm/Utils.hpp"

namespace rm {

class RoIExtractor {
 public:
  RoIExtractor(const RoIExtractorConfig& config, const ResizeProfile* resizeProfile);

  void enqueue(Frame* frame);

  std::vector<Frame*> getExtractedFrames();

 private:
  void work();

  void process(Frame* currFrame) const;

  static std::vector<RoI> mergeRoIs(const std::vector<RoI>& rois, const float mergeThreshold,
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
  std::vector<std::thread> mThreads;
  bool mbStop;

  const cv::Size mTargetSize;
  const ResizeProfile* mResizeProfile;

  std::mutex mFramesMtx;
  std::condition_variable mFramesCv;
  std::deque<Frame*> mFrames;
  std::deque<Frame*> mFramesToTake;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
