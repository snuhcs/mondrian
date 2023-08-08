#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/InferenceEngine.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"

namespace md {

class BoundingBox;
class PackedCanvas;
class ROIResizer;

class Frame {
 private:
  int width_;
  int height_;

 public:
  const int vid;
  const int frameIndex;
  const Frame* prevFrame;
  int scheduleID;
  cv::Mat yuvMat;
  cv::Mat rgbMat;
  cv::Mat resizedGrayMat;
  int PDExtractorID;
  int OFExtractorID;

  bool useInferenceResultForOF;

  bool isBoxesReady;
  bool isROIsReady;
  int numFeaturePoints;
  std::vector<std::unique_ptr<BoundingBox>> boxes;
  std::vector<std::unique_ptr<BoundingBox>> probingBoxes;
  std::vector<std::unique_ptr<MergedROI>> probingROIs;

  bool reprocessOF;
  std::vector<std::unique_ptr<ROI>> rois;
  std::vector<std::unique_ptr<MergedROI>> mergedROIs;

  bool isLastFrame;

  int inferenceFrameSize;
  Device inferenceDevice;

  const time_us enqueueTime;
  time_us fullInferenceStartTime = 0;
  time_us fullInferenceEndTime = 0;
  time_us opticalFlowROIProcessStartTime = 0;
  time_us opticalFlowROIProcessEndTime = 0;
  time_us pixelDiffROIProcessStartTime = 0;
  time_us pixelDiffROIProcessEndTime = 0;
  time_us resizeStartTime = 0;
  time_us resizeEndTime = 0;
  time_us mergeROIStartTime = 0;
  time_us mergeROIEndTime = 0;
  time_us packingStartTime = 0;
  time_us packingEndTime = 0;
  time_us scheduledTime = 0;
  time_us packedInferenceStartTime = 0;
  time_us packedInferenceEndTime = 0;
  time_us reconstructStartTime = 0;
  time_us reconstructEndTime = 0;
  time_us endTime = 0;

  Frame(const int vid, const int frameIndex, const cv::Mat& yuvMat,
        const Frame* prevFrame, const time_us& enqueueTime);

  void prepareRgbMatAndResizedGrayMat(const cv::Size& targetSize);

  void eatPDROIs(float overlap_thres);

  void filterPDROIs(float overlap_thres);

  void assignPDROIIDs();

  void resizeROIs(ROIResizer* roiResizer);

  void resetMergedROIs();

  void mergeMergedROIs(int maxSize);

  void sortMergedROIs();

  void resetProbeROIs();

  IntPairs boxesIfLast(ROIResizer* roiResizer,
                       ExecutionType executionType,
                       bool noDownsampling);

  void prepareFrameLast(const IntPairs& indices,
                        const IntPairs& locations,
                        ExecutionType executionType,
                        int roiSize,
                        bool noDownsampling);

  int maxRelativePackedCanvasIndex() const;

  bool isReadyToMarry(int packedFrameIndex) const;

  bool readyForOFExtraction() const;

  void resetOFROIExtraction();

  int width() const {
    assert(width_ != 0);
    return width_;
  }

  int height() const {
    assert(height_ != 0);
    return height_;
  }

  Key getKey() const {
    return {vid, frameIndex};
  }
};

struct FrameComp {
  bool operator()(const Frame* lhs, const Frame* rhs) const {
    if (lhs->frameIndex == rhs->frameIndex) {
      return lhs->vid < rhs->vid;
    }
    return lhs->frameIndex < rhs->frameIndex;
  }
};

using Stream = std::set<Frame*, FrameComp>;
using MultiStream = std::map<int, Stream>;

std::string str(const Stream& stream);

std::string str(const MultiStream& streams);

struct PackingResult {
  MultiStream streams;
  Frame* fullFrameTarget;
  std::vector<PackedCanvas> packedCanvases;
};

} // namespace md

#endif // FRAME_HPP_
