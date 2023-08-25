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
  bool testROIsIntegrity() const;

 public:
  const VID vid;
  const FID fid;
  const Frame* prevFrame;
  cv::Mat yuvMat;
  cv::Mat resizedGrayMat;
  std::vector<std::unique_ptr<ROI>> rois;
  std::vector<std::unique_ptr<MergedROI>> mergedROIs;
  std::map<Device, std::vector<std::unique_ptr<MergedROI>>> probingROIsTable;
  std::vector<std::unique_ptr<BoundingBox>> boxes;
  std::map<Device, std::vector<std::unique_ptr<BoundingBox>>> probingBoxesTable;

  bool useInferenceResultForOF;
  bool isBoxesReady;
  bool isROIsReady;
  bool reprocessOF;
  bool isLastFrame;

  int scheduleID;
  int PDExtractorID;
  int OFExtractorID;
  int numFeaturePoints;
  int inferenceFrameSize;
  Device deviceIfFullFrame; // else Device::INVALID
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

  Frame(const VID vid, const FID fid, const cv::Mat& yuvMat,
        const Frame* prevFrame, const time_us& enqueueTime);

  cv::Mat rgbMat() const;

  void prepareResizedGrayMat(const cv::Size& targetSize);

  void eatPDROIs(float overlap_thres);

  void filterPDROIs(float overlap_thres);

  void assignPDROIIDs();

  void resizeROIs(ROIResizer* roiResizer,
                  ExecutionType executionType,
                  int roiSize);

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

  bool isReadyToMarry() const;

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
    return {vid, fid};
  }

  static std::string header();

  std::string str(time_us baseTime) const;
};

struct FrameComp {
  bool operator()(const Frame* lhs, const Frame* rhs) const {
    if (lhs->fid == rhs->fid) {
      return lhs->vid < rhs->vid;
    }
    return lhs->fid < rhs->fid;
  }
};

using Stream = std::set<Frame*, FrameComp>;
using MultiStream = std::map<int, Stream>;

std::string str(const Stream& stream);

std::string str(const MultiStream& streams);

struct PackingResult {
  MultiStream streams;
  Frame* fullFrameTarget;
  std::map<Device, std::vector<PackedCanvas>> packedCanvasesTable;
};

} // namespace md

#endif // FRAME_HPP_
