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
  RoIExtractor(const RoIExtractorConfig& config, int maxMergeSize, bool run,
               RoIResizer* roiResizer, std::vector<InferenceInfo> inferencePlan,
               std::set<int> vids);

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

  void tryPack(Frame* currFrame, const IntPairs& boxesIfLast, const IntPairs& boxesIfIntermediate);

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

  void prepareFrameLast(Frame* frame, const IntPairs& packedLocations,
                        const IntPairs& packedIndices);

  IntPairs getBoxesIfLastFrame(const Frame* frame);

  static void prepareFrameIntermediate(Frame* frame,
                                       const IntPairs& packedLocations,
                                       const IntPairs& packedIndices);

  static IntPairs getBoxesIfIntermediateFrame(const Frame* frame);

  void packAndApplyPrevs(bool isLast, int skipVid = -1);

  void packAndApplyPrev(bool isLast, int targetVid);

  void restorePrevs(bool isLast, int skipVid = -1);

  void restorePrev(bool isLast, int targetVid);

  std::vector<std::thread> mThreads;
  bool mbStop;

  static const cv::TermCriteria CRITERIA;
  const cv::Size mTargetSize;
  const int mMaxMergeSize;
  const RoIExtractorConfig mConfig;
  const std::set<int> mVids;
  int mFullFrameVid;

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

  Stream mPDWaiting;
  Stream mOFWaiting;
  Stream mOFProcessing;

  MultiStream mPackedFrames;
  std::unique_ptr<BinPacker> mBinPacker;
  bool isFullyPacked;

  struct PackInfo {
    Frame* frame;
    BoxIndices last;
    BoxIndices inter;
    IntPairs lastPackedLocations;
    IntPairs interPackedLocations;
  };

  std::vector<int> mPrevPackVids;
  std::map<int, PackInfo> mPrevPackInfos;
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
