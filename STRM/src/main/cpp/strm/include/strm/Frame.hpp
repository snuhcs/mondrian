#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "strm/DataType.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/RoI.hpp"

namespace rm {

class BoundingBox;
class RoI;
class RoIResizer;

class Frame {
 private:
  static const int FULL_KEY_OFFSET;

 public:
  const int vid;
  const int frameIndex;
  cv::Mat mat;
  Frame* prevFrame;
  Frame* nextFrame;
  cv::Mat preProcessedMat;
  int PDExtractorID;
  int OFExtractorID;

  const float width;
  const float height;

  bool useInferenceResultForOF;

  bool isBoxesReady;
  bool isRoIsReady;
  std::vector<std::unique_ptr<BoundingBox>> boxes;
  std::vector<std::unique_ptr<BoundingBox>> probingBoxes;
  std::vector<std::unique_ptr<RoI>> probingRoIs;

  bool extractOFAgain;
  std::vector<std::unique_ptr<RoI>> childRoIs; // => box
  std::vector<std::unique_ptr<RoI>> parentRoIs;

  bool isLastFrame;
  IntPairs boxesIfLast;
  IntPairs boxesIfScaled;

  int inferenceFrameSize;
  Device inferenceDevice;

  const time_us enqueueTime;
  time_us fullInferenceStartTime = 0;
  time_us fullInferenceEndTime = 0;
  time_us opticalFlowRoIProcessStartTime = 0;
  time_us opticalFlowRoIProcessEndTime = 0;
  time_us pixelDiffRoIProcessStartTime = 0;
  time_us pixelDiffRoIProcessEndTime = 0;
  time_us resizeStartTime = 0;
  time_us resizeEndTime = 0;
  time_us mergeRoIStartTime = 0;
  time_us mergeRoIEndTime = 0;
  time_us mixingStartTime = 0;
  time_us mixingEndTime = 0;
  time_us scheduledTime = 0;
  time_us mixedInferenceStartTime = 0;
  time_us mixedInferenceEndTime = 0;
  time_us reconstructStartTime = 0;
  time_us reconstructEndTime = 0;
  time_us endTime = 0;

  Frame(const int vid, const int frameIndex, const cv::Mat mat,
        Frame* prevFrame, const time_us& enqueueTime);

  void resizeRoIs(RoIResizer* roiResizer, bool emulatedBatch, int roiSize);

  void resetParentRoIs();

  void mergeRoIs(float maxSize);

  void resetProbeRoIs();

  void filterPDRoIs(float threshold, bool eatPD);

  bool isReadyToMarry(int mixedFrameIndex) const;

  bool readyForOFExtraction() const;

  void resetOFRoIExtraction();

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

std::string toString(const Stream& frames);

std::string toString(const MultiStream& frames);

} // namespace rm

#endif // FRAME_HPP_
