#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <sstream>
#include <queue>

#include "mondrian/Time.hpp"

namespace md {

using VID = int;
using FID = int;
using OID = int;
using IntPair = std::pair<int, int>;
using IntPairs = std::vector<std::pair<int, int>>;

extern const OID INVALID_OID;
extern const char DELIM;
extern const int NUM_LABELS;
extern const char* COCO_LABELS[];

enum class ROIType {
  OF = 1,
  PD,
};

ROIType roiTypeOf(const std::string& roiTypeStr);

std::string str(const ROIType& roiType);

enum class Device {
  INVALID = 0,
  GPU,
  DSP,
};

Device deviceOf(const std::string& deviceStr);

std::string str(const Device& device);

enum class ExecutionType {
  MONDRIAN = 1,
  EMULATED_BATCH,
  ROI_WISE_INFERENCE,
  FRAME_WISE_INFERENCE,
};

ExecutionType executionTypeOf(const std::string& executionTypeStr);

std::string str(const ExecutionType& executionType);

enum class Origin {
  INVALID = 0,       // null value for initialization
  FULL_FRAME,        // (Box) matched Box from full frame_
  PACKED_CANVAS,     // (ROI, Box) OF ROI from bounding box, Box from those ROIs
  PD,                // (ROI, Box) PD ROI, OF ROI originated from PD ROI, Box from those ROIs
  INTERPOLATE,       // (Box) interpolated Box
  NEW_FULL_FRAME,    // (Box) unmatched Box from full frame_
  NEW_PACKED_CANVAS, // (Box) unmatched Box from packed canvas
};

Origin originOf(const std::string& originStr);

std::string str(const Origin& origin);

enum class ROIPackerType {
  MIN_CONSECUTIVE_DROP = 1,
  OF_CONFIDENCE,
};

ROIPackerType roiPackerTypeOf(const std::string& roiPackerTypeStr);

std::string str(const ROIPackerType& roiPackerType);

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

} // namespace md

#endif // DATA_TYPE_HPP_
