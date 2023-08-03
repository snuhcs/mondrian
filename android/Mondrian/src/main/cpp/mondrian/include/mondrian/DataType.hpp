#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <sstream>
#include <queue>

#include "mondrian/Time.hpp"

namespace md {

using ID = int;
using IntPair = std::pair<int, int>;
using IntPairs = std::vector<std::pair<int, int>>;

extern const ID INVALID_ID;
extern const char* COCO_LABELS[];

enum Type {
  OF = 1,
  PD = 2,
};

enum Device {
  GPU = 0,
  DSP = 1,
  NO_DEVICE = 2,
};

Device deviceOf(const std::string& deviceStr);

std::string str(const Device& device);

enum ExecutionType {
  MONDRIAN = 0,
  EMULATED_BATCH = 1,
  ROI_WISE_INFERENCE = 2,
  FRAME_WISE_INFERENCE = 3,
};

ExecutionType executionTypeOf(const std::string& executionTypeStr);

std::string str(const ExecutionType& executionType);

enum Origin {
  O_INVALID = 0,           // null value for initialization
  O_FULL_FRAME = 1,        // (Box) matched Box from full frame_
  O_PACKED_CANVAS = 2,     // (ROI, Box) OF ROI from bounding box, Box from those ROIs
  O_PD = 3,                // (ROI, Box) PD ROI, OF ROI originated from PD ROI, Box from those ROIs
  O_INTERPOLATE = 4,       // (Box) interpolated Box
  O_NEW_FULL_FRAME = 5,    // (Box) unmatched Box from full frame_
  O_NEW_PACKED_CANVAS = 6, // (Box) unmatched Box from packed canvas
};

enum ROIPackerType {
  MIN_MAX_PROPAGATION = 0,
  OF_CONFIDENCE,
};

ROIPackerType roiPackerTypeOf(const std::string& roiPackerTypeStr);

std::string str(const ROIPackerType& roiPrioritizerType);

struct Rect {
  float l;
  float t;
  float r;
  float b;
  float w;
  float h;
  float minWH;
  float maxWH;
  float area;

  Rect() {}

  Rect(float l, float t, float r, float b) : l(l), t(t), r(r), b(b) {
    w = r - l;
    h = b - t;
    minWH = std::min(w, h);
    maxWH = std::max(w, h);
    area = w * h;
  };

  Rect(const Rect& r) : Rect(r.l, r.t, r.r, r.b) {};

  std::pair<float, float> center() const {
    return {(r + l) / 2,
            (b + t) / 2};
  }

  bool isValid() const {
    return l <= r && t <= b;
  }

  bool overlap(const Rect& other) const {
    return l < other.r && r > other.l && t < other.b && b > other.t;
  }

  float intersection(const Rect& other) const {
    float width = std::min(r, other.r) - std::max(l, other.l);
    float height = std::min(b, other.b) - std::max(t, other.t);
    if (width <= 0 || height <= 0) {
      return 0;
    } else {
      return width * height;
    }
  }

  float iou(const Rect& other) const {
    float inter = intersection(other);
    return inter / (area + other.area - inter);
  }

  static Rect merge(const Rect& rect0, const Rect& rect1) {
    return {std::min(rect0.l, rect1.l),
            std::min(rect0.t, rect1.t),
            std::max(rect0.r, rect1.r),
            std::max(rect0.b, rect1.b)};
  }

  Rect clip(const Rect& other) const {
    return {std::min(std::max(l, other.l), other.r),
            std::min(std::max(t, other.t), other.b),
            std::min(std::max(r, other.l), other.r),
            std::min(std::max(b, other.t), other.b)};
  }
};

class ROI;

struct BoundingBox {
  ID id;
  Rect loc;
  float confidence;
  int label;
  ROI* srcROI;
  Origin origin;
  ID choiceOfBox;

  BoundingBox(ID id,
              const Rect location,
              const float confidence,
              int label,
              Origin origin)
      : id(id),
        loc(location),
        confidence(confidence),
        label(label),
        origin(origin),
        srcROI(nullptr),
        choiceOfBox(INVALID_ID) {}

  static std::string header(char delim) {
    std::stringstream ss;
    ss << "id" << delim
       << "left" << delim
       << "top" << delim
       << "right" << delim
       << "bottom" << delim
       << "confidence" << delim
       << "label" << delim
       << "origin" << delim
       << "choiceOfBox";
    return ss.str();
  }

  std::string str(char delim) const {
    std::stringstream ss;
    ss << id << delim
       << loc.l << delim
       << loc.t << delim
       << loc.r << delim
       << loc.b << delim
       << confidence << delim
       << COCO_LABELS[label] << delim
       << origin << delim
       << choiceOfBox;
    return ss.str();
  }
};

struct InferenceInfo {
  Device device;
  int size;
  time_us latency;
  time_us accumulatedLatency = -1;
};

std::string str(const std::vector<InferenceInfo>& inferencePlan);

} // namespace md

#endif // DATA_TYPE_HPP_
