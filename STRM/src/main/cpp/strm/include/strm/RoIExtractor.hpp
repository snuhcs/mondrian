#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/Utils.hpp"

namespace rm {

class RoIExtractor {
 public:
  RoIExtractor(const RoIExtractorConfig& config, bool run, int maxQueueSize);

  ~RoIExtractor();

  void enqueue(Frame* frame);

  void notify();

  std::map<std::string, SortedFrames> getExtractedFrames();

  void reEnqueueFrames(const std::vector<Frame*>& frames);

  const cv::Size& getTargetSize() const {
    return mTargetSize;
  }

 private:
  void work();

  void processPD(Frame* currFrame);

  void processOF(Frame* currFrame);

  void getOpticalFlowRoIs(const Frame* prevFrame, Frame* currFrame,
                          const std::vector<BoundingBox>& boundingBoxes,
                          const cv::Size& targetSize,
                          std::vector<std::unique_ptr<RoI>>& outChildRoIs) const;

  static std::vector<RoI::OFFeatures> opticalFlowTracking(
      const Frame* prevFrame, const Frame* currFrame, const std::vector<Rect>& boundingBoxes,
      const cv::Size& targetSize);

  void getPixelDiffRoIs(
      const Frame* prevFrame, Frame* currFrame, const cv::Size& targetSize,
      const float mixRoIArea, std::vector<std::unique_ptr<RoI>>& outChildRoIs) const;

  static cv::Mat calculateDiffAndThreshold(
      const cv::Mat& prevMat, const cv::Mat& currMat);

  static void cannyEdgeDetection(cv::Mat mat);

  const RoIExtractorConfig mConfig;
  std::vector<std::thread> mThreads;
  bool mbStop;

  static const cv::TermCriteria CRITERIA;
  const cv::Size mTargetSize;

  std::mutex mtx;
  std::condition_variable cv;
  const int mMaxQueueSize;
  std::list<Frame*> mFramesForPD;
  std::map<std::string, std::list<Frame*>> mFramesForOF;
  std::map<std::string, SortedFrames> mOFProcessingStartedFrames;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
