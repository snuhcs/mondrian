#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "strm/Time.hpp"

namespace rm {

typedef int idType;

extern const idType UNASSIGNED_ID;
extern const idType MERGED_ROI_ID;

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

  Rect(const std::pair<int, int> center, const int width, const int height) {
    left = center.first - width / 2;
    right = center.first + width / 2;
    top = center.second - height / 2;
    bottom = center.second + height / 2;
  }

  int width() const {
    return right - left;
  }

  int height() const {
    return bottom - top;
  }

  int area() const {
    return width() * height();
  }

  std::pair<int, int> center() const {
    return std::make_pair((right + left) / 2, (bottom + top) / 2);
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

enum Origin {
  originNull = 0,
  fromBB = 1,
  fromPD = 2,
  fromIP = 3,
};

struct BoundingBox {
  Rect location;
  float confidence;
  int label;
  idType id;
  RoI* srcRoI;
  Origin origin;

  BoundingBox(idType id, const Rect location, const float confidence, int label, Origin origin)
      : id(id), location(location), confidence(confidence), label(label), origin(origin),
        srcRoI(nullptr) {}
};

enum RoIExtractionStatus {
  OF_WAITING = 1,
  OF_EXTRACTING = 2,
  OF_EXTRACTED = 3,
};

struct Frame {
  const std::string key;
  const std::string shortKey;
  const int frameIndex;
  cv::Mat mat;
  Frame* prevFrame;
  Frame* nextFrame;
  cv::Mat preProcessedMat;

  const int width;
  const int height;

  bool useInferenceResultForOF;

  bool isBoxesReady;
  bool isRoIsReady;
  std::vector<std::unique_ptr<BoundingBox>> boxes;
  std::vector<std::unique_ptr<BoundingBox>> probingBoxes;
  std::vector<std::unique_ptr<RoI>> probingRoIs;

  RoIExtractionStatus roiExtractionStatus;
  std::vector<std::unique_ptr<RoI>> childRoIs; // => box
  std::vector<std::unique_ptr<RoI>> parentRoIs;

  bool isFullFrameTarget;

  const time_us enqueueTime;
  time_us fullFrameEnqueueTime = 0;
  time_us fullFrameGetResultsTime = 0;
  time_us opticalFlowRoIProcessStartTime = 0;
  time_us opticalFlowRoIProcessEndTime = 0;
  time_us pixelDiffRoIProcessStartTime = 0;
  time_us pixelDiffRoIProcessEndTime = 0;
  time_us resizeStartTime = 0;
  time_us resizeEndTime = 0;
  time_us mergeRoIStartTime = 0;
  time_us mergeRoIEndTime = 0;
  time_us mixingStartTime = 0;
  time_us mixingEndTime = 0;;
  time_us reconstructStartTime = 0;
  time_us reconstructEndTime = 0;
  time_us endTime = 0;

  Frame(const std::string& key, const int frameIndex, const cv::Mat mat,
        Frame* prevFrame, const time_us& enqueueTime)
      : key(key), shortKey(toShortKey(key)), frameIndex(frameIndex), mat(mat),
        width(mat.cols), height(mat.rows), prevFrame(prevFrame), useInferenceResultForOF(false),
        roiExtractionStatus(OF_WAITING), enqueueTime(enqueueTime), isFullFrameTarget(false),
        isBoxesReady(false), isRoIsReady(false) {}

  ~Frame() {
    endTime = NowMicros();
  }

  void initParentRoIs();

  void mergeRoIs(float mergeThreshold, int maxSize);

  void addProbeRoIs(int numProbeSteps, int probeStepSize);

  void filterPDRoIs(float threshold);

  bool isReadyToMarry(int mixedFrameIndex) const;

  bool readyForPDExtraction() const;

  bool readyForOFExtraction() const;

  static std::string toShortKey(const std::string& key);
};

struct FrameIndexComp {
  bool operator()(const Frame* lhs, const Frame* rhs) const {
    if (lhs->frameIndex == rhs->frameIndex) {
      return lhs->key < rhs->key;
    }
    return lhs->frameIndex < rhs->frameIndex;
  }
};

using SortedFrames = std::set<Frame*, FrameIndexComp>;

std::set<Frame*> filterLastFrames(const std::map<std::string, SortedFrames>& frames);

std::string toString(const std::map<std::string, SortedFrames>& frames);

class Logger;

class FrameBuffer {
 public:
  FrameBuffer(const std::string& key, int capacity);

  Frame* enqueue(const cv::Mat& mat);

  void freeImage(const std::vector<int> &frameIndices, Logger *logger, Logger *roiLogger);

 private:
  const std::string key;
  const std::string shortKey;
  const int capacity;

  int count;
  std::mutex mtx;
  std::condition_variable cv;
  std::vector<std::unique_ptr<Frame>> frames;
};

struct RoI {
  enum Type {
    OF = 1,
    PD = 2,
  };

  struct OFFeatures {
    const std::vector<std::pair<float, float>> shifts;
    const std::vector<float> errs;

    const std::pair<float, float> avgShift;
    const std::pair<float, float> stdShift;
    const float avgErr;
    const float ncc;

    OFFeatures(const std::vector<std::pair<float, float>>& shifts, const std::vector<float>& errs)
        : shifts(shifts), errs(errs), avgShift(getShiftAvg(shifts)), stdShift(getShiftStd(shifts)),
          avgErr(getAvgErr(errs)), ncc(getNCC(shifts)) {}

    static std::pair<float, float> getShiftAvg(const std::vector<std::pair<float, float>>& shifts) {
      if (shifts.empty()) {
        return {0, 0};
      }
      std::pair<float, float> shift = {0, 0};
      for (const auto&[x, y] : shifts) {
        shift.first += x;
        shift.second += y;
      }
      shift.first /= shifts.size();
      shift.second /= shifts.size();
      return shift;
    }

    static std::pair<float, float> getShiftStd(const std::vector<std::pair<float, float>>& shifts) {
      if (shifts.empty()) {
        return {0, 0};
      }
      std::pair<float, float> var = {0, 0};
      auto[avgX, avgY] = getShiftAvg(shifts);
      for (const auto&[x, y] : shifts) {
        var.first += (x - avgX) * (x - avgX);
        var.second += (y - avgY) * (y - avgY);
      }
      var.first /= shifts.size();
      var.second /= shifts.size();
      return {std::sqrt(var.first), std::sqrt(var.second)};
    }

    static float getAvgErr(const std::vector<float>& errs) {
      if (errs.empty()) {
        return 0;
      }
      float err = 0;
      for (const float& e : errs) {
        err += e;
      }
      return err /= errs.size();
    }

    static float getNCC(const std::vector<std::pair<float, float>>& shifts) {
      if (shifts.size() <= 1) {
        return 0;
      }
      float ncc = 0;
      for (int i = 0; i < shifts.size(); i++) {
        for (int j = i + 1; j < shifts.size(); j++) {
          auto&[Xi, Yi] = shifts[i];
          auto&[Xj, Yj] = shifts[j];
          float sizeI = Xi * Xi + Yi * Yi;
          float sizeJ = Xj * Xj + Yj * Yj;
          if (sizeI == 0 || sizeJ == 0) {
            continue;
          }
          ncc += (Xi * Xj + Yi * Yj) / std::sqrt(sizeI * sizeJ);
        }
      }
      return ncc / (shifts.size() * (shifts.size() - 1) / 2);
    }
  };

  struct Features {
    int width;
    int height;
    int label;
    Type type;
    float xyRatio;
    OFFeatures ofFeatures;
  };

  Frame* frame;
  const Rect origLoc;
  const Rect paddedLoc;
  Type type;
  Origin origin;
  int label;
  Features features;
  std::vector<RoI*> roisForProbing;
  float priority;

  inline static std::atomic<idType> lastId = 0;
  idType id;
  RoI* prevRoI; // only valid with childRoIs
  RoI* nextRoI; // only valid with childRoIs
  std::vector<RoI*> childRoIs;
  RoI* parentRoI;

  int maxEdgeLength;
  int targetSize;
  std::pair<int, int> packedLocation;
  static const std::pair<int, int> NOT_PACKED;

  int packedMixedFrameIndex;
  int packedAbsMixedFrameIndex;
  bool isProbingRoI;
  bool isMatchTried; // only valid within parentRoIs
  BoundingBox* box;
  BoundingBox* probingBox;

  RoI(RoI* prevRoI,
      const idType id,
      Frame* frame,
      const Rect origLoc,
      const Type type,
      const Origin origin,
      const int label,
      const OFFeatures ofFeatures,
      int roiPadding,
      bool isProbingRoI)
      : prevRoI(prevRoI), id(id), frame(frame), origLoc(origLoc), paddedLoc(
      std::max(0, origLoc.left - roiPadding),
      std::max(0, origLoc.top - roiPadding),
      std::min(frame->mat.cols, origLoc.right + roiPadding),
      std::min(frame->mat.rows, origLoc.bottom + roiPadding)),
        type(type), origin(origin), label(label), features{
          paddedLoc.width(),
          paddedLoc.height(),
          label,
          type,
          (float) origLoc.width() / (float) origLoc.height(),
          ofFeatures
      }, maxEdgeLength(std::max(paddedLoc.width(), paddedLoc.height())),
        targetSize(maxEdgeLength), packedLocation(NOT_PACKED), isMatchTried(false),
        nextRoI(nullptr), parentRoI(nullptr), box(nullptr), probingBox(nullptr),
        packedMixedFrameIndex(INT_MAX), packedAbsMixedFrameIndex(-1),
        isProbingRoI(isProbingRoI) {
    if (prevRoI != nullptr) {
      prevRoI->nextRoI = this;
    }
  };

  RoI(const RoI& roi) = default;

  static std::unique_ptr<RoI> mergeRoIs(const RoI* pRoI0, const RoI* pRoI1) {
    assert(pRoI0->frame == pRoI1->frame);
    int newLeft = std::min(pRoI0->paddedLoc.left, pRoI1->paddedLoc.left);
    int newTop = std::min(pRoI0->paddedLoc.top, pRoI1->paddedLoc.top);
    int newRight = std::max(pRoI0->paddedLoc.right, pRoI1->paddedLoc.right);
    int newBottom = std::max(pRoI0->paddedLoc.bottom, pRoI1->paddedLoc.bottom);
    RoI::Type roiType = pRoI0->type != RoI::Type::PD || pRoI1->type != RoI::Type::PD
                        ? RoI::Type::OF
                        : RoI::Type::PD;
    int roiLabel;
    if (pRoI0->label == pRoI1->label) {
      roiLabel = pRoI0->label;
    } else if (pRoI0->label != -1 && pRoI1->label == -1) {
      roiLabel = pRoI0->label;
    } else if (pRoI0->label == -1 && pRoI1->label != -1) {
      roiLabel = pRoI1->label;
    } else {
      roiLabel = -1;
    }
    std::unique_ptr<RoI> mergedRoI(
        new RoI(nullptr, MERGED_ROI_ID, pRoI0->frame, Rect(newLeft, newTop, newRight, newBottom),
                roiType, originNull, roiLabel, OFFeatures({}, {}), 0, false));
    mergedRoI->targetSize = (pRoI0->targetSize * pRoI1->maxEdgeLength >
                             pRoI1->targetSize * pRoI0->maxEdgeLength) ?
                            mergedRoI->maxEdgeLength * pRoI0->targetSize / pRoI0->maxEdgeLength :
                            mergedRoI->maxEdgeLength * pRoI1->targetSize / pRoI1->maxEdgeLength;
    return std::move(mergedRoI);
  }

  static std::pair<idType, idType> getNewIds(unsigned long num) {
    idType minId = lastId.fetch_add(num);
    idType maxId = minId + num;
    // [minId, maxId)
    return std::pair<idType, idType>(minId, maxId);
  }

  bool isProbingReady() const {
    if (roisForProbing.empty()) {
      return false;
    }
    bool ready = true;
    for (auto& pRoI : roisForProbing) {
      ready &= pRoI->isMatchTried;
    }
    return ready;
  }

  bool isPacked() const {
    return packedLocation != NOT_PACKED;
  }

  bool isParent() const {
    return childRoIs.size() > 1;
  }

  int getPaddedArea() const {
    return paddedLoc.area();
  }

  int getResizedArea() const {
    const std::pair<int, int> resizedWH = getResizedWidthHeight();
    return resizedWH.first * resizedWH.second;
  }

  std::pair<int, int> getResizedWidthHeight() const {
    if (maxEdgeLength <= targetSize) {
      return std::make_pair(paddedLoc.width(), paddedLoc.height());
    }
    if (paddedLoc.width() > paddedLoc.height()) {
      return std::make_pair(targetSize, paddedLoc.height() * targetSize / paddedLoc.width());
    } else {
      return std::make_pair(paddedLoc.width() * targetSize / paddedLoc.height(), targetSize);
    }
  }

  cv::Mat getOrigMat() const {
    return frame->mat.operator()(
        cv::Rect(origLoc.left, origLoc.top, origLoc.width(), origLoc.height()));
  }

  cv::Mat getPaddedMat() const {
    return frame->mat.operator()(
        cv::Rect(paddedLoc.left, paddedLoc.top, paddedLoc.width(), paddedLoc.height()));
  }

  cv::Mat getResizedMat() const {
    std::pair<int, int> wh = getResizedWidthHeight();
    cv::Mat resizedMat;
    cv::resize(getPaddedMat(), resizedMat, cv::Size(wh.first, wh.second));
    return resizedMat;
  }

  bool operator<(const RoI& roi) const {
    return targetSize < roi.targetSize;
  }
};

struct MixedFrame {
  static int numMixedFrames;
  const int mixedFrameIndex;
  cv::Mat packedMat;
  std::set<RoI*> packedRoIs;

  MixedFrame(std::set<RoI*> packedRoIs, int mixedFrameSize)
      : packedRoIs(packedRoIs), mixedFrameIndex(numMixedFrames++) {
    packedMat = cv::Mat::zeros(mixedFrameSize, mixedFrameSize, CV_8UC4);
    for (RoI* roi : packedRoIs) {
      assert(roi->isPacked());
      std::pair<int, int> wh = roi->getResizedWidthHeight();
      roi->getResizedMat().copyTo(
          packedMat(cv::Rect(roi->packedLocation.first, roi->packedLocation.second,
                             wh.first, wh.second)));
      roi->packedAbsMixedFrameIndex = mixedFrameIndex;
    }
  }

  SortedFrames getPackedFrames() {
    SortedFrames packedFrames;
    for (RoI* roi : packedRoIs) {
      packedFrames.insert(roi->frame);
    }
    return packedFrames;
  }
};

} // namespace rm

#endif // DATA_TYPE_HPP_
