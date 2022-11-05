#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include "strm/Time.hpp"

namespace rm {

typedef int idType;

extern const idType UNASSIGNED_ID;
extern const idType MERGED_ROI_ID;

enum Device {
  GPU,
  DSP,
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

class RoI;

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

std::string toString(const std::vector<InferenceInfo>& inferencePlan);

} // namespace rm

#endif // DATA_TYPE_HPP_
