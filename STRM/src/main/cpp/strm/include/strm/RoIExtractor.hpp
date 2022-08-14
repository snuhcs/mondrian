#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/Utils.hpp"

namespace rm {

class RoIExtractor {
 public:
  RoIExtractor(const RoIExtractorConfig& config, bool run, bool allowInterpolation,
               bool roiWiseInference, const PatchMixer* patchMixer, RoIResizer* roiResizer,
               int frameSize, int numFrames);

  ~RoIExtractor();

  void enqueue(Frame* frame);

  void notify();

  std::map<std::string, SortedFrames> getExtractedFrames(int numFrames);

  void reEnqueueFrames(const std::vector<Frame*>& frames);

  const cv::Size& getTargetSize() const {
    return mTargetSize;
  }

 private:
  void work();

  void resetPack();

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
  RoIResizer* mRoIResizer;
  bool mbStop;

  const bool mAllowInterpolation;
  const bool mRoIWiseInference;
  const PatchMixer* mPatchMixer;
  const int mFrameSize;
  std::mutex packMtx;
  int mNumFrames;
  bool isFullyPacked;
  std::map<int, std::vector<Rect>> mFreeRectsMap;
  int mRoICount;

  static const cv::TermCriteria CRITERIA;
  const cv::Size mTargetSize;

  std::mutex mtx;
  std::condition_variable cv;
  std::list<Frame*> mFramesForPD;
  std::map<std::string, std::list<Frame*>> mFramesForOF;
  std::map<std::string, SortedFrames> mOFProcessingStartedFrames;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
