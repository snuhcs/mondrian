#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"
#include "mondrian/MergedROI.hpp"
#include "mondrian/ROIPacker.hpp"
#include "mondrian/ROI.hpp"

namespace md {

class BoundingBox;
class ROIResizer;

class Frame {
 private:
  static const int FULL_KEY_OFFSET;

 public:
  const int vid;
  const int frameIndex;
  int scheduleID;
  cv::Mat rgbMat;
  cv::Mat resizedGrayMat;
  Frame* prevFrame;
  Frame* nextFrame;
  int PDExtractorID;
  int OFExtractorID;

  const int width;
  const int height;

  bool useInferenceResultForOF;

  bool isBoxesReady;
  bool isROIsReady;
  std::vector<std::unique_ptr<BoundingBox>> boxes;
  std::vector<std::unique_ptr<BoundingBox>> probingBoxes;
  std::vector<std::unique_ptr<MergedROI>> probingROIs;

  bool extractOFAgain;
  std::vector<std::unique_ptr<ROI>> rois;
  std::vector<std::unique_ptr<MergedROI>> mergedROIs;

  bool isLastFrame;
  IntPairs boxesIfLast;
  IntPairs boxesIfScaled;

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

  Frame(const int vid, const int frameIndex, const cv::Mat& mat,
        Frame* prevFrame, const time_us& enqueueTime);

  void resizeROIs(ROIResizer* roiResizer, bool emulatedBatch, int roiSize);

  void resetProbeROIs();

  void filterPDROIs(float threshold, bool eatPD);

  bool isReadyToMarry(int packedFrameIndex) const;

  bool readyForOFExtraction() const;

  void resetOFROIExtraction();

  int getKey() const {
    return frameIndex + FULL_KEY_OFFSET;
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

std::string str(const Stream& frames);

std::string str(const MultiStream& frames);

} // namespace md

#endif // FRAME_HPP_
