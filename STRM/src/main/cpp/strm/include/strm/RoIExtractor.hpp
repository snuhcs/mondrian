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
               std::map<Device, std::vector<int>>& inferencePlan);

  ~RoIExtractor();

  void enqueue(Frame* frame);

  void notify();

  MultiStream getExtractedFrames(std::map<Device, std::vector<int>>& inferencePlan);

  void reEnqueueFrames(const Stream& droppedFrames);

 private:
  void work(int extractorId);

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

  std::mutex packMtx;
  std::vector<std::pair<Device, std::vector<Rect>>> mFreeRectsMap;
  int mRoICount;
  int mNumFramesPerInterval;
  std::map<Device, std::vector<int>> mInferencePlan;
  bool isFullyPacked;

  static const cv::TermCriteria CRITERIA;
  const cv::Size mTargetSize;

  std::mutex mtx;
  std::condition_variable cv;
  Stream mPDWaiting;
  Stream mOFWaiting;
  Stream mOFProcessing;
  Stream mExtractionFinished;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
