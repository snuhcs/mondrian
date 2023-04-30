#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <set>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/DataType.hpp"
#include "mondrian/MixedFrame.hpp"
#include "mondrian/PatchMixer.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Utils.hpp"

namespace md {

class ROIExtractor {
 public:
  ROIExtractor(const ROIExtractorConfig& config, int maxMergeSize, bool run,
               ROIResizer* roiResizer, bool emulatedBatch, int roiSize,
               std::vector<InferenceInfo> inferencePlan, std::set<int> vids);

  ~ROIExtractor();

  void enqueue(Frame* frame);

  void notify();

  std::tuple<std::vector<MixedFrame>, Frame*, MultiStream, Stream> prepareInference(
      std::vector<InferenceInfo>& nextInferencePlan, bool runFull, int scheduleID);

 private:
  void work(int extractorId);

  void processPD(Frame* currFrame);

  void processOF(Frame* currFrame);

  void postprocessOF(Frame* currFrame);

  void tryPack(Frame* frame);

  bool tryPackFullVid(Frame* frame);

  bool tryPackNonFullVid(Frame* frame);

  void getOpticalFlowROIs(const Frame* prevFrame, Frame* currFrame,
                          const std::vector<BoundingBox>& boundingBoxes,
                          const cv::Size& targetSize,
                          std::vector<std::unique_ptr<ROI>>& outChildROIs) const;

  static std::vector<OFFeatures> opticalFlowTracking(
      const Frame* prevFrame, const Frame* currFrame, const std::vector<Rect>& boundingBoxes,
      const cv::Size& targetSize);

  void getPixelDiffROIs(Frame* currFrame, const cv::Size& targetSize,
                        const float maxPDROISize, const float minPDROISize,
                        std::vector<std::unique_ptr<ROI>>& outChildROIs) const;

  static cv::Mat calculateDiffAndThreshold(
      const cv::Mat& prevMat, const cv::Mat& currMat);

  static void cannyEdgeDetection(cv::Mat mat);

  void resetPatchMixerWithPlan(const std::vector<InferenceInfo>& inferencePlan);

  void prepareFrameLast(Frame* frame,
                        const Indices& indices, const Locations& locations);

  IntPairs getBoxesIfLast(const Frame* frame);

  void prepareScaledFrame(Frame* frame, const Indices& indices, const Locations& locations);

  static IntPairs getBoxesIfScaled(const Frame* frame);

  void applyLasts();

  std::vector<std::thread> mThreads;
  bool mbStop;

  static const cv::TermCriteria CRITERIA;

  const bool mEmulatedBatch;
  const int mROISize;
  const cv::Size mTargetSize;
  const int mMaxMergeSize;
  const ROIExtractorConfig mConfig;
  const std::set<int> mVids;
  int mFullFrameVid;
  Frame* mFullFrameTarget;

  ROIResizer* mROIResizer;

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

  // Finalized frames are packed
  std::vector<std::vector<IntRect>> mFreeRectsVec;
  bool notFullyPacked;

  struct LastPackInfo {
    Frame* frame;
    Indices indices;
    Locations locations;
  };

  // Can be packed as last. Otherwise packed as scaled.
  std::map<int, LastPackInfo> mCandidateLastFrames;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
