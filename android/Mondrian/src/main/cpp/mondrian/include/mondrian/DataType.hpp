#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <sstream>

#include "mondrian/Time.hpp"
#include "mondrian/Utils.hpp"

namespace md {

using ID = int;
using IntPair = std::pair<int, int>;
using IntPairs = std::vector<std::pair<int, int>>;

extern const ID INVALID_ID;

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

struct Rect {
  float l;
  float t;
  float r;
  float b;
  float w;
  float h;
  float maxWH;
  float area;

  Rect() {}

  Rect(float l, float t, float r, float b) : l(l), t(t), r(r), b(b) {
    w = r - l;
    h = b - t;
    maxWH = std::max(w, h);
    area = w * h;
  };

  Rect(const Rect& r) : Rect(r.l, r.t, r.r, r.b) {};

  Rect(const std::pair<float, float> center, const float width, const float height) :
      Rect(center.first - width / 2,
           center.first + width / 2,
           center.second - height / 2,
           center.second + height / 2) {}

  std::pair<float, float> center() const {
    return std::make_pair((r + l) / 2, (b + t) / 2);
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

  BoundingBox(ID id, const Rect location, const float confidence, int label, Origin origin)
      : id(id), loc(location), confidence(confidence), label(label), origin(origin),
        srcROI(nullptr), choiceOfBox(INVALID_ID) {}

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
