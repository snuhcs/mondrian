#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <sstream>

#include "mondrian/Time.hpp"
#include "mondrian/Utils.hpp"

namespace md {

typedef int idType;

using IntPair = std::pair<int, int>;
using IntPairs = std::vector<std::pair<int, int>>;

extern const idType UNASSIGNED_ID;
extern const idType MERGED_ROI_ID;

enum Device {
  NO_DEVICE = -1,
  GPU = 0,
  DSP = 1,
};

Device toDevice(std::string deviceStr);

const char* toConstStr(Device device);

enum Origin {
  O_NULL = 0,            // null value for initialization
  O_FULL_FRAME = 1,      // (Box) matched Box from full frame
  O_PACKED_BBOX = 2,     // (ROI, Box) OF ROI from bounding box, Box from those ROIs
  O_PD = 3,              // (ROI, Box) PD ROI, OF ROI originated from PD ROI, Box from those ROIs
  O_INTERPOLATE = 4,     // (Box) interpolated Box
  O_NEW_FULL_FRAME = 5,  // (Box) unmatched Box from full frame
  O_NEW_PACKED_BBOX = 6, // (Box) unmatched Box from packed canvas
};

struct Rect {
  float l;
  float t;
  float r;
  float b;
  float w;
  float h;
  float area;

  Rect() {}

  Rect(float l, float t, float r, float b)
      : l(l), t(t), r(r), b(b), w(r - l), h(b - t), area((r - l) * (b - t)) {};

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
  Rect location;
  float confidence;
  int label;
  idType choiceOfBox;
  idType id;
  ROI* srcROI;
  Origin origin;

  BoundingBox(idType id, const Rect location, const float confidence, int label, Origin origin)
      : id(id), location(location), confidence(confidence), label(label), origin(origin),
        srcROI(nullptr), choiceOfBox(UNASSIGNED_ID) {}

  std::string str() const {
    std::stringstream ss;
    ss << id << ','
       << location.l << ','
       << location.t << ','
       << location.r << ','
       << location.b << ','
       << confidence << ','
       << origin << ','
       << choiceOfBox << ','
       << COCO_LABELS[label];
    return ss.str();
  }
};

struct InferenceInfo {
  Device device;
  int size;
  time_us latency;
  time_us accumulatedLatency = -1;
};

std::string toString(const std::vector<InferenceInfo>& inferencePlan);

} // namespace md

#endif // DATA_TYPE_HPP_
