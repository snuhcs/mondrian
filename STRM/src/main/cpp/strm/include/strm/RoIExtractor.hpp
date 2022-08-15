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
               int frameSize, int numFramesPerInterval);

  ~RoIExtractor();

  void enqueue(Frame* frame);

  void notify();

  std::map<std::string, SortedFrames> getExtractedFrames(int numFrames);

  void reEnqueueFrames(const SortedFrames& droppedFrames);

  const cv::Size& getTargetSize() const {
    return mTargetSize;
  }

 private:
  void work();

  void resetPack();

  static void resetOFRoIExtraction(Frame* frame);

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

  std::vector<std::thread> mThreads;
  bool mbStop;

  const RoIExtractorConfig mConfig;
  const bool mAllowInterpolation;
  const bool mRoIWiseInference;

  RoIResizer* mRoIResizer;
  const PatchMixer* mPatchMixer;
  const int mFrameSize;

  std::mutex packMtx;
  std::map<int, std::vector<Rect>> mFreeRectsMap;
  int mRoICount;
  int mNumFramesPerInterval;
  bool isFullyPacked;

  static const cv::TermCriteria CRITERIA;
  const cv::Size mTargetSize;

  std::mutex mtx;
  std::condition_variable cv;
  SortedFrames mPDWaiting;
  SortedFrames mOFWaiting;
  SortedFrames mOFProcessing;
  SortedFrames mExtractionFinished;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
