#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "strm/BinPacker.hpp"
#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/Utils.hpp"

namespace rm {

class RoIExtractor {
 public:
  RoIExtractor(const RoIExtractorConfig& config, int maxMergeSize,
               bool run, bool roiWiseInference, RoIResizer* roiResizer,
               std::vector<InferenceInfo> inferencePlan);

  ~RoIExtractor();

  void enqueue(Frame* frame);

  void notify();

  std::tuple<std::vector<MixedFrame>, Frame*, MultiStream, Stream> prepareInference(
      std::vector<InferenceInfo>& nextInferencePlan, bool runFull);

 private:
  void work(int extractorId);

  void processPD(Frame* currFrame);

  void processOF(Frame* currFrame);

  void postprocessOF(Frame* currFrame);

  void getOpticalFlowRoIs(const Frame* prevFrame, Frame* currFrame,
                          const std::vector<BoundingBox>& boundingBoxes,
                          const cv::Size& targetSize,
                          std::vector<std::unique_ptr<RoI>>& outChildRoIs) const;

  static std::vector<OFFeatures> opticalFlowTracking(
      const Frame* prevFrame, const Frame* currFrame, const std::vector<Rect>& boundingBoxes,
      const cv::Size& targetSize);

  void getPixelDiffRoIs(Frame* currFrame, const cv::Size& targetSize, const float minRoIArea,
                        std::vector<std::unique_ptr<RoI>>& outChildRoIs) const;

  static cv::Mat calculateDiffAndThreshold(
      const cv::Mat& prevMat, const cv::Mat& currMat);

  static void cannyEdgeDetection(cv::Mat mat);

  void resetBinPackerWithPlan(const std::vector<InferenceInfo>& inferencePlan);

  std::vector<std::pair<int, int>> getBoxesIfLastFrame(const Frame* frame);

  void prepareFrameLast(Frame* frame,
                        const std::vector<std::pair<int, int>>& packedLocations,
                        const std::vector<std::pair<int, int>>& packedIndices);

  static std::vector<std::pair<int, int>> getBoxesIfIntermediateFrame(const Frame* frame);

  static void prepareFrameIntermediate(Frame* frame,
                                const std::vector<std::pair<int, int>>& packedLocations,
                                const std::vector<std::pair<int, int>>& packedIndices);

  std::vector<std::thread> mThreads;
  bool mbStop;

  static const cv::TermCriteria CRITERIA;
  const cv::Size mTargetSize;
  const int mMaxMergeSize;
  const RoIExtractorConfig mConfig;
  const bool mRoIWiseInference;

  RoIResizer* mRoIResizer;

  int mFullFrameInferenceCount;
  std::vector<InferenceInfo> mInferencePlan;

  /*
   * If you want to acquire queueMtx and packMtx at the same time,
   * you must acquire packMtx first.
   */
  std::mutex queueMtx;
  std::mutex packMtx;
  std::condition_variable queueCv;
  std::condition_variable packCv;

  Stream mPDWaiting;
  Stream mOFWaiting;
  Stream mOFProcessing;

  Stream mPackedFrames;
  std::unique_ptr<BinPacker> mBinPacker;
  bool isFullyPacked;
  Frame* prevPackedFrame;
  std::vector<std::pair<int, int>> prevPackIndicesIntermediate;
  std::vector<std::pair<int, int>> prevBoxesIfIntermediate;
  std::vector<std::pair<int, int>> prevPackIndicesLast;
  std::vector<std::pair<int, int>> prevBoxesIfLast;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
