#ifndef ROI_EXTRACTOR_HPP_
#define ROI_EXTRACTOR_HPP_

#include <list>
#include <queue>
#include <set>
#include <thread>
#include <utility>

#include "opencv2/core/mat.hpp"

#include "strm/BinPacker.hpp"
#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/MixedFrame.hpp"
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

  void tryPack(Frame* frame);

  bool tryPackFullVid(Frame* frame);

  bool tryPackNonFullVid(Frame* frame);

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

  void prepareFrameLast(Frame* frame,
                        const Indices& indices, const Locations& locations);

  IntPairs getBoxesIfLast(const Frame* frame);

  static void prepareScaledFrame(Frame* frame,
                                 const Indices& indices, const Locations& locations);

  static IntPairs getBoxesIfScaled(const Frame* frame);

  void applyLasts();

  std::vector<std::thread> mThreads;
  bool mbStop;

  static const cv::TermCriteria CRITERIA;
  const cv::Size mTargetSize;
  const int mMaxMergeSize;
  const RoIExtractorConfig mConfig;
  const std::set<int> mVids;
  int mFullFrameVid;
  Frame* mFullFrameTarget;

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

  void test(const LastPackInfo& info) const {
    assert(info.frame->boxesIfLast.size() == info.indices.size());
  }

  IntPairs origWHs;

  void testPackedFrames(bool all = false) const {
    for (auto&[vid, frames]: mPackedFrames) {
      for (auto& frame: frames) {
        bool isCandidate = false;
        for (auto&[vid, info]: mCandidateLastFrames) {
          if (info.frame == frame) {
            isCandidate = true;
            break;
          }
        }
        if (!all && isCandidate) {
          continue;
        }
        for (auto& pRoI: frame->parentRoIs) {
          assert(pRoI->packedMixedFrameIndex < origWHs.size());
          auto[mw, mh] = origWHs[pRoI->packedMixedFrameIndex];
          auto[x, y] = pRoI->packedLocation;
          auto[w, h] = pRoI->getResizedMatWidthHeight();
          assert(x >= 0 && y >= 0 && w >= 0 && h >= 0 && x + w <= mw && y + h <= mh);
        }
        assert(isCandidate || frame->probingRoIs.empty());
        for (auto& probingRoI: frame->probingRoIs) {
          auto[mw, mh] = origWHs[probingRoI->packedMixedFrameIndex];
          auto[x, y] = probingRoI->packedLocation;
          auto[w, h] = probingRoI->getResizedMatWidthHeight();
          assert(x >= 0 && y >= 0 && w >= 0 && h >= 0 && x + w <= mw && y + h <= mh);
        }
      }
    }
  }

  std::string candidateLastFramesStr() const {
    std::stringstream ss;
    for (const auto&[vid, info] : mCandidateLastFrames) {
      ss << vid << ", ";
    }
    return ss.str();
  }
};

} // namespace rm

#endif // ROI_EXTRACTOR_HPP_
