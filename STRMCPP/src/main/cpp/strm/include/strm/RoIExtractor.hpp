#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <utility>

#include "opencv2/core/mat.hpp"

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/ResizeProfile.hpp"
#include "strm/Utils.hpp"

namespace rm {

class RoIExtractor {
 public:
  struct Job {
    Frame* prevFrame;
    Frame* currFrame;
  };

  RoIExtractor(const RoIExtractorConfig& config, const ResizeProfile* resizeProfile);

  void enqueuePDJob(Frame* prevFrame, Frame* currFrame);

  void enqueueOFJob(Frame* prevFrame, Frame* currFrame, std::vector<BoundingBox> boxes);

  std::vector<RoI> getRoIs();

 private:
  void work();

  std::vector<RoI> process(Frame* prevFrame, Frame* currFrame,
                           const std::vector<BoundingBox>& prevResults) const;

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

  const ResizeProfile* mResizeProfile;

  std::vector<std::thread> mThreads;

  ConcurrentQueue<Job> mPDJobs;
  ConcurrentQueue<Job> mOFJobs;
  ConcurrentQueue<RoI> mRoIs;

  std::mutex mtx;
  std::condition_variable cv;
  std::atomic_bool mbStop;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
