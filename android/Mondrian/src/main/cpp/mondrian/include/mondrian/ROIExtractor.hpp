#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <set>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIPacker.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

struct PackingResult {
  std::vector<PackedCanvas> packedCanvases;
  Frame* fullFrameTarget;
  MultiStream selectedFrames;
  Stream droppedFrames;
};

class ROIExtractor {
 public:
  ROIExtractor(const ROIExtractorConfig& config, int maxMergeSize,
               ROIResizer* roiResizer, ExecutionType executionType, int roiSize,
               std::vector<InferenceInfo> inferencePlan, int numVideos);

  ~ROIExtractor();

  void enqueue(Frame* frame);

  void notify();

  PackingResult prepareInference(std::vector<InferenceInfo>& nextInferencePlan,
                                 bool runFull, int scheduleID);

 private:
  void work(int extractorId);

  void processPD(Frame* currFrame);

  void processOF(Frame* currFrame);

  void postprocessOF(Frame* currFrame);

  void packGatheredMultiStream();

  void tryPack(Frame* frame);

  bool tryPackFullVid(Frame* frame);

  bool tryPackNonFullVid(Frame* frame);

  static void getOpticalFlowROIs(const Frame* prevFrame, Frame* currFrame,
                                 const std::vector<BoundingBox>& boundingBoxes,
                                 const cv::Size& targetSize,
                                 std::vector<std::unique_ptr<ROI>>& outChildROIs);

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
                        const IntPairs& indices, const IntPairs& locations);

  IntPairs getBoxesIfLast(const Frame* frame);

  void prepareFrameScaled(Frame* frame, const IntPairs& indices, const IntPairs& locations);

  static IntPairs getBoxesIfScaled(const Frame* frame);

  void applyLasts();

  std::vector<std::thread> threads_;
  bool pull_;
  bool stop_;

  static const cv::TermCriteria CRITERIA;

  const ExecutionType executionType_;
  const int ROISize_;
  const cv::Size targetSize_;
  const int maxMergeSize_;
  const ROIExtractorConfig config_;
  const int numVideos_;
  int fullFrameVid_;
  Frame* fullFrameTarget_;

  ROIResizer* ROIResizer_;

  int fullFrameInferenceCount_;
  std::vector<InferenceInfo> inferencePlan_;

  /*
   * If you want to acquire queueMtx_ and packMtx_ at the same time,
   * you must acquire packMtx_ first.
   */
  std::mutex queueMtx_;
  std::mutex packMtx_;
  std::condition_variable queueCV_;

  Stream PDWaiting_;
  Stream PDProcessing_;
  Stream OFWaiting_;
  Stream OFProcessing_;
  MultiStream OFProcessed_;
  std::vector<std::vector<IntRect>> freeRectsVec_;
  bool fullyPacked_; // Only for back to back mode

  struct LastPackInfo {
    Frame* frame;
    IntPairs indices;
    IntPairs locations;
  };

  // Can be packed as last. Otherwise packed as scaled.
  std::map<int, LastPackInfo> candidateLastFrames_;
};

} // namespace md

#endif // ROI_EXTRACTOR_HPP_
