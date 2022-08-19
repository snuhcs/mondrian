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

enum Device {
  GPU,
  DSP,
};

struct Rect {
  float left;
  float top;
  float right;
  float bottom;

  Rect() {}

  Rect(const float left, const float top, const float right, const float bottom)
      : left(left), top(top), right(right), bottom(bottom) {};

  Rect(const Rect& r)
      : left(r.left), top(r.top), right(r.right), bottom(r.bottom) {};

  Rect(const std::pair<float, float> center, const float width, const float height) {
    left = center.first - width / 2;
    right = center.first + width / 2;
    top = center.second - height / 2;
    bottom = center.second + height / 2;
  }

  float width() const {
    return right - left;
  }

  float height() const {
    return bottom - top;
  }

  float area() const {
    return width() * height();
  }

  std::pair<float, float> center() const {
    return std::make_pair((right + left) / 2, (bottom + top) / 2);
  }

  float intersection(const Rect& other) const {
    float width = std::min(right, other.right) - std::max(left, other.left);
    float height = std::min(bottom, other.bottom) - std::max(top, other.top);
    if (width <= 0 || height <= 0) {
      return 0;
    } else {
      return width * height;
    }
  }

  float iou(const Rect& other) const {
    float inter = intersection(other);
    return inter / (area() + other.area() - inter);
  }
};

enum Origin {
  origin_Null = 0,  // null value for initialization
  origin_FF = 1,    // (Box) matched Box from full frame
  origin_BB = 2,    // (RoI, Box) OF RoI from bounding box, Box from those RoIs
  origin_PD = 3,    // (RoI, Box) PD RoI, OF RoI originated from PD RoI, Box from those RoIs
  origin_IP = 4,    // (Box) interpolated Box
  origin_NewFF = 5, // (Box) unmatched Box from full frame
  origin_NewMF = 6, // (Box) unmatched Box from mixed frame
};

struct BoundingBox {
  Rect location;
  float confidence;
  int label;
  idType choiceOfBox;
  idType id;
  RoI* srcRoI;
  Origin origin;

  BoundingBox(idType id, const Rect location, const float confidence, int label, Origin origin)
      : id(id), location(location), confidence(confidence), label(label), origin(origin),
        srcRoI(nullptr), choiceOfBox(UNASSIGNED_ID) {}
};

class RoIResizer;

struct Frame {
  const std::string key;
  const std::string shortKey;
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

  bool isFullFrameTarget;
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
  time_us mixingEndTime = 0;;
  time_us mixedInferenceStartTime = 0;
  time_us mixedInferenceEndTime = 0;
  time_us reconstructStartTime = 0;
  time_us reconstructEndTime = 0;

  Frame(const std::string& key, const int frameIndex, const cv::Mat mat,
        Frame* prevFrame, const time_us& enqueueTime)
      : key(key), shortKey(toShortKey(key)), frameIndex(frameIndex), mat(mat),
        width(mat.cols), height(mat.rows), prevFrame(prevFrame), useInferenceResultForOF(false),
        extractOFAgain(false), enqueueTime(enqueueTime), isFullFrameTarget(false),
        isBoxesReady(false), isRoIsReady(false), PDExtractorID(-1), OFExtractorID(-1),
        inferenceFrameSize(0) {}

  void resizeRoIs(RoIResizer* roiResizer);

  void resetParentRoIs();

  void mergeRoIs(float mergeThreshold, float maxSize);

  void addProbeRoIs(int numProbeSteps, float probeStepSize);

  void resetProbeRoIs();

  void filterPDRoIs(float threshold);

  bool isReadyToMarry(int mixedFrameIndex) const;

  bool readyForOFExtraction() const;

  static std::string toShortKey(const std::string& key);
};

struct FrameComp {
  bool operator()(const Frame* lhs, const Frame* rhs) const {
    if (lhs->frameIndex == rhs->frameIndex) {
      return lhs->key < rhs->key;
    }
    return lhs->frameIndex < rhs->frameIndex;
  }
};

struct InferenceInfo {
  Device device;
  int size;
  time_us latency;
  time_us accumulatedLatency = -1;
};

struct FreeRects {
  Device device;
  int frameSize;
  std::vector<Rect> rects;
};

using Stream = std::set<Frame*, FrameComp>;
using MultiStream = std::map<std::string, Stream>;

std::set<Frame*> filterLastFrames(const MultiStream& frames);

std::string toString(const MultiStream& frames);

std::string toString(const std::vector<InferenceInfo>& inferencePlan);

class Logger;

class FrameBuffer {
 public:
  FrameBuffer(const std::string& key, int capacity);

  Frame* enqueue(const cv::Mat& mat);

  void freeImage(const std::vector<int>& frameIndices, Logger* logger, Logger* roiLogger);

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

    std::pair<float, float> avgShift;
    std::pair<float, float> stdShift;
    float avgErr;
    float ncc;

    OFFeatures(const std::vector<std::pair<float, float>>& shifts, const std::vector<float>& errs)
        : shifts(shifts), errs(errs), avgShift(getShiftAvg(shifts)), stdShift(getShiftStd(shifts)),
          avgErr(getAvgErr(errs)), ncc(getNCC(shifts)) {
      if (!shifts.empty()) {
        std::vector<std::pair<float, float>> filtered = filterShifts(shifts);
        avgShift = getShiftAvg(filtered);
        stdShift = getShiftStd(filtered);
        ncc = getNCC(filtered);
      }
    }

    static std::vector<std::pair<float, float>> filterShifts(
        const std::vector<std::pair<float, float>>& shifts) {
      std::vector<float> distances;
      for (const auto&[x, y] : shifts) {
        distances.push_back(x * x + y * y);
      }
      auto const q1_index = int(float(distances.size()) * 0.25);
      std::nth_element(distances.begin(), distances.begin() + q1_index, distances.end());
      float q1 = distances[q1_index];
      std::vector<std::pair<float, float>> filteredShifts;
      for (auto&[x, y] : shifts) {
        if (x * x + y * y > q1) {
          filteredShifts.emplace_back(x, y);
        }
      }
      return filteredShifts;
    }

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
    float width;
    float height;
    int label;
    Type type;
    Origin origin;
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

  float maxEdgeLength;

 private:
  float targetSize;

 public:
  std::pair<float, float> packedLocation;
  static const std::pair<float, float> NOT_PACKED;

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
      float roiPadding,
      bool isProbingRoI)
      : prevRoI(prevRoI), id(id), frame(frame), origLoc(origLoc), paddedLoc(
      std::max(0.0f, origLoc.left - roiPadding),
      std::max(0.0f, origLoc.top - roiPadding),
      std::min(float(frame->mat.cols), origLoc.right + roiPadding),
      std::min(float(frame->mat.rows), origLoc.bottom + roiPadding)),
        type(type), origin(origin), label(label), features{
          paddedLoc.width(),
          paddedLoc.height(),
          label,
          type,
          origin,
          (float) origLoc.width() / (float) origLoc.height(),
          ofFeatures
      }, maxEdgeLength(std::max(paddedLoc.width(), paddedLoc.height())),
        targetSize(maxEdgeLength), packedLocation(NOT_PACKED), isMatchTried(false),
        nextRoI(nullptr), parentRoI(nullptr), box(nullptr), probingBox(nullptr),
        packedMixedFrameIndex(INT_MAX), packedAbsMixedFrameIndex(-1),
        isProbingRoI(isProbingRoI), priority(-1) {
    if (prevRoI != nullptr) {
      prevRoI->nextRoI = this;
    }
  };

  static std::unique_ptr<RoI> mergeRoIs(const RoI* pRoI0, const RoI* pRoI1) {
    assert(pRoI0->frame == pRoI1->frame);
    float newLeft = std::min(pRoI0->paddedLoc.left, pRoI1->paddedLoc.left);
    float newTop = std::min(pRoI0->paddedLoc.top, pRoI1->paddedLoc.top);
    float newRight = std::max(pRoI0->paddedLoc.right, pRoI1->paddedLoc.right);
    float newBottom = std::max(pRoI0->paddedLoc.bottom, pRoI1->paddedLoc.bottom);
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
                roiType, origin_Null, roiLabel, OFFeatures({}, {}), 0, false));
    mergedRoI->setTargetSize(pRoI0->targetSize * pRoI1->maxEdgeLength >
                             pRoI1->targetSize * pRoI0->maxEdgeLength ?
                             mergedRoI->maxEdgeLength * pRoI0->targetSize / pRoI0->maxEdgeLength :
                             mergedRoI->maxEdgeLength * pRoI1->targetSize / pRoI1->maxEdgeLength);
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

  float getPaddedArea() const {
    return paddedLoc.area();
  }

  float getResizedArea() const {
    const std::pair<float, float> resizedWH = getResizedWidthHeight();
    return resizedWH.first * resizedWH.second;
  }

  float getTargetSize() const {
    return targetSize;
  }

  void setTargetSize(float newTargetSize) {
    float minEdgeLength = std::min(paddedLoc.width(), paddedLoc.height());
    targetSize = std::max(maxEdgeLength / minEdgeLength,
                          std::min(maxEdgeLength, newTargetSize));
  }

  std::pair<float, float> getResizedWidthHeight() const {
    assert(targetSize <= maxEdgeLength);
    std::pair<float, float> wh;
    if (targetSize == maxEdgeLength) {
      wh = {paddedLoc.width(), paddedLoc.height()};
    }
    if (paddedLoc.width() > paddedLoc.height()) {
      wh = {targetSize, paddedLoc.height() * targetSize / paddedLoc.width()};
    } else {
      wh = {paddedLoc.width() * targetSize / paddedLoc.height(), targetSize};
    }
    return {std::max(1.0f, wh.first), std::max(1.0f, wh.second)};
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
    auto[w, h] = getResizedWidthHeight();
    assert(w >= 1 && h >= 1);
    cv::Mat resizedMat;
    cv::resize(getPaddedMat(), resizedMat, cv::Size(w, h));
    return resizedMat;
  }

  bool operator<(const RoI& roi) const {
    return targetSize < roi.targetSize;
  }
};

struct MixedFrame {
  static int numMixedFrames;
  const Device device;
  const int mixedFrameIndex;
  const int mixedFrameSize;
  cv::Mat packedMat;
  std::set<RoI*> packedRoIs;

  MixedFrame(Device device, std::set<RoI*> packedRoIs, int mixedFrameSize)
      : device(device), packedRoIs(packedRoIs), mixedFrameIndex(numMixedFrames++),
        mixedFrameSize(mixedFrameSize) {
    packedMat = cv::Mat::zeros(mixedFrameSize, mixedFrameSize, CV_8UC4);
    for (RoI* roi : packedRoIs) {
      assert(roi->isPacked());
      std::pair<float, float> wh = roi->getResizedWidthHeight();
      roi->getResizedMat().copyTo(
          packedMat(cv::Rect(roi->packedLocation.first, roi->packedLocation.second,
                             wh.first, wh.second)));
      roi->packedAbsMixedFrameIndex = mixedFrameIndex;
    }
  }

  Stream getPackedFrames() {
    Stream packedFrames;
    for (RoI* roi : packedRoIs) {
      packedFrames.insert(roi->frame);
    }
    return packedFrames;
  }
};

} // namespace rm

#endif // DATA_TYPE_HPP_
