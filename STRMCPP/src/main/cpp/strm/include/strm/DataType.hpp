#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <atomic>
#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <set>

#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "strm/Time.hpp"

namespace rm {

typedef unsigned long idType;

extern const idType UNASSIGNED_ID;

struct RoI;

struct Rect {
  int left;
  int top;
  int right;
  int bottom;

  Rect() {}

  Rect(const int left, const int top, const int right, const int bottom)
      : left(left), top(top), right(right), bottom(bottom) {};

  Rect(const Rect& r)
      : left(r.left), top(r.top), right(r.right), bottom(r.bottom) {};

  int width() const {
    return right - left;
  }

  int height() const {
    return bottom - top;
  }

  int area() const {
    return width() * height();
  }

  int intersection(const Rect& other) const {
    int width = std::min(right, other.right) - std::max(left, other.left);
    int height = std::min(bottom, other.bottom) - std::max(top, other.top);
    if (width <= 0 || height <= 0) {
      return 0;
    } else {
      return width * height;
    }
  }

  float iou(const Rect& other) const {
    int inter = intersection(other);
    return (float) inter / (area() + other.area() - inter);
  }
};

struct BoundingBox {
  Rect location;
  float confidence;
  std::string labelName;
  int targetSize;
  idType id;

  BoundingBox(idType id, const Rect location, const float confidence, const std::string labelName, int targetSize=-1)
      : id(id), location(location), confidence(confidence), labelName(labelName), targetSize(targetSize) {}
};

enum RoIExtractionStatus {
  OF_WAITING = 1,
  OF_EXTRACTING = 2,
  OF_EXTRACTED = 3,
};

struct Frame {
  static int ROI_PADDING;

  const std::string key;
  const int frameIndex;
  cv::Mat mat;
  Frame* prevFrame;
  Frame* nextFrame;
  cv::Mat preProcessedMat;

  const int width;
  const int height;

  std::vector<BoundingBox> boxes;

  // For next frame OF roi extraction
  bool isOFReady;
  std::vector<BoundingBox> boxesToTrack;

  RoIExtractionStatus roiExtractionStatus;
  std::vector<RoI> origRoIs;
  std::vector<RoI> rois;

  const time_us enqueueTime;
  time_us dispatcherProcessStartTime = 0;
  time_us dispatcherProcessEndTime = 0;
  time_us fullFrameEnqueueTime = 0;
  time_us fullFrameGetResultsTime = 0;
  time_us opticalFlowRoIProcessStartTime = 0;
  time_us opticalFlowRoIProcessEndTime = 0;
  time_us pixelDiffRoIProcessStartTime = 0;
  time_us pixelDiffRoIProcessEndTime = 0;
  time_us resizeRoIStartTime = 0;
  time_us resizeRoIEndTime = 0;
  time_us mergeRoIStartTime = 0;
  time_us mergeRoIEndTime = 0;
  time_us mixingStartTime = 0;
  time_us mixingEndTime = 0;
  time_us mixedFrameCreateStartTime = 0;
  time_us mixedFrameCreateEndTime = 0;
  time_us mixedFrameEnqueueTime = 0;
  time_us reconstructStartTime = 0;
  time_us reconstructEndTime = 0;
  time_us endTime = 0;

  Frame(const std::string& key, const int frameIndex, const cv::Mat mat,
        Frame* prevFrame, const time_us& enqueueTime)
      : key(key), frameIndex(frameIndex), mat(mat), width(mat.cols), height(mat.rows),
        prevFrame(prevFrame), isOFReady(false), roiExtractionStatus(OF_WAITING),
        enqueueTime(enqueueTime) {}

  ~Frame() {
    endTime = NowMicros();
  }

  std::string shortKey() const {
    return key.substr(key.size() - 8);
  }

  void updateBoxesToTrackWithInferenceResult();

  void updateBoxesToTrackWithRoIs();

  bool readyForOFExtraction() const;
};

struct RoI {
  enum Type {
    OF = 1,
    PD = 2,
  };

  struct Features {
    std::string labelName;
    Type type;
    float xyRatio;
    std::pair<int, int> shift;
    float err;
    float diffAreaRatio;

    int getShiftSize() const {
      return shift.first * shift.first + shift.second * shift.second;
    }
  };

  Frame* frame;
  Rect location;
  Type type;
  std::string labelName;
  Features features;
  std::vector<RoI> roisForProbing;

  inline static std::atomic<idType> lastId;
  idType id;
  idType parentId;
  std::vector<idType> childrenId;

  int maxEdgeLength;
  int targetSize;
  std::pair<int, int> packedLocation;

  bool isDone;
  std::vector<BoundingBox> boxes;

  RoI(const idType id,
      Frame* frame,
      const Rect location,
      const Type type,
      const std::string labelName,
      const std::pair<int, int>& shift,
      const float err,
      const float diffAreaRatio)
      : id(id),
        frame(frame),
        location(location),
        type(type),
        labelName(labelName),
        features{labelName, type, (float) location.width() / (float) location.height(),
                 std::make_pair(shift.first, shift.second), err, diffAreaRatio},
        maxEdgeLength(std::max(location.width(), location.height())),
        targetSize(maxEdgeLength),
        packedLocation(std::make_pair(-1, -1)),
        isDone(false),
        parentId(-1) {};

  static RoI mergeRoIs(const RoI& roi0, const RoI& roi1) {
    assert(roi0.frame == roi1.frame);
    int newLeft = std::min(roi0.location.left, roi1.location.left);
    int newTop = std::min(roi0.location.top, roi1.location.top);
    int newRight = std::max(roi0.location.right, roi1.location.right);
    int newBottom = std::max(roi0.location.bottom, roi1.location.bottom);
    RoI::Type roiType = roi0.type == RoI::Type::OF || roi1.type == RoI::Type::OF
                        ? RoI::Type::OF
                        : RoI::Type::PD;
    std::string roiLabel = roi0.labelName.empty() || roi1.labelName.empty()
                           || roi0.labelName != roi1.labelName
                           ? "" : roi0.labelName;
    RoI mergedRoI(RoI::getNewIds(1).first, roi0.frame, Rect(newLeft, newTop, newRight, newBottom), roiType, roiLabel,
                  std::make_pair(0, 0), 0, 0);
    mergedRoI.childrenId.emplace_back(roi0.id);
    mergedRoI.childrenId.emplace_back(roi1.id);
    mergedRoI.targetSize = (roi0.targetSize * roi1.maxEdgeLength > roi1.targetSize * roi0.maxEdgeLength) ?
                           mergedRoI.maxEdgeLength * roi0.targetSize / roi0.maxEdgeLength :
                           mergedRoI.maxEdgeLength * roi1.targetSize / roi1.maxEdgeLength;
    return mergedRoI;
  }

  static std::pair<idType, idType> getNewIds(unsigned long num) {
    idType minId = lastId.fetch_add(num);
    idType maxId = minId + num;
    // [minId, maxId)
    return std::pair<idType, idType>(minId, maxId);
  }

  bool isPacked() const {
    return packedLocation.first >= 0 && packedLocation.second >= 0;
  }

  bool isChild() const {
    return (parentId != -1);
  }

  bool isParent() const {
    return (!childrenId.empty());
  }

  int getArea() const {
    return location.area();
  }

  int getResizedArea() const {
    const std::pair<int, int> resizedWH = getResizedWidthHeight();
    return resizedWH.first * resizedWH.second;
  }

  std::pair<int, int> getResizedWidthHeight() const {
    if (maxEdgeLength <= targetSize) {
      return std::make_pair(location.width(), location.height());
    }
    if (location.width() > location.height()) {
      return std::make_pair(targetSize, location.height() * targetSize / location.width());
    } else {
      return std::make_pair(location.width() * targetSize / location.height(), targetSize);
    }
  }

  cv::Mat getMat() const {
    return frame->mat.operator()(
        cv::Rect(location.left, location.top, location.width(), location.height()));
  }

  cv::Mat getResizedMat() const {
    std::pair<int, int> wh = getResizedWidthHeight();
    cv::Mat resizedMat;
    cv::resize(getMat(), resizedMat, cv::Size(wh.first, wh.second));
    return resizedMat;
  }

  bool operator < (const RoI& roi) const {
    return (targetSize < roi.targetSize);
  }
};

struct MixedFrame {
  const int mixedFrameIndex;
  cv::Mat packedMat;
  std::vector<RoI*> packedRoIs;

  MixedFrame(const int mixedFrameIndex, std::vector<RoI*> packedRoIs, int mixedFrameSize)
      : mixedFrameIndex(mixedFrameIndex), packedRoIs(packedRoIs) {
    packedMat = cv::Mat::zeros(mixedFrameSize, mixedFrameSize, CV_8UC4);
    for (RoI* roi : packedRoIs) {
      if (roi->isPacked()) {
        std::pair<int, int> wh = roi->getResizedWidthHeight();
        roi->getResizedMat().copyTo(
            packedMat(cv::Rect(roi->packedLocation.first, roi->packedLocation.second,
                               wh.first, wh.second)));
      }
    }
  }

  std::set<Frame*> getPackedFrames() {
    std::set<Frame*> packedFrames;
    for (RoI* roi : packedRoIs) {
      packedFrames.insert(roi->frame);
    }
    return packedFrames;
  }
};

} // namespace rm

#endif // DATA_TYPE_HPP_
