#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <string>
#include <utility>
#include <vector>

#include "opencv2/opencv.hpp"

#include "strm/Log.hpp"

namespace rm {

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

  BoundingBox(const Rect location, const float confidence, const std::string labelName)
      : location(location), confidence(confidence), labelName(labelName) {}
};

struct Frame {
  const std::string key;
  const int frameIndex;
  const cv::Mat mat;

  std::atomic_bool isResultReady;
  std::vector<BoundingBox> boxes;

  std::vector<RoI> rois;
  std::vector<RoI> opticalFlowRoIs;

  Frame(const Frame& frame)
      : key(frame.key), frameIndex(frame.frameIndex), mat(frame.mat.clone()), isResultReady(false) {}

  Frame(const std::string& key, const int frameIndex, const cv::Mat mat)
      : key(key), frameIndex(frameIndex), mat(mat), isResultReady(false) {}
};

struct RoI {
  enum Type {
    OF = 1,
    PD = 2,
  };

  const Frame* frame;
  Rect location;
  Type type;
  std::string labelName;

  int minOriginLength;
  float scale;
  std::pair<int, int> packedLocation;

  int handle;
  std::vector<BoundingBox> boxes;

  RoI(const Frame* frame, const Rect location, const Type type, const std::string labelName)
      : frame(frame), location(location), type(type), labelName(labelName),
        minOriginLength(-1), scale(1), packedLocation(std::make_pair(-1, -1)),
        handle(-1) {};

  RoI(const Frame* frame, const Rect location, const Type type, const std::string labelName, const int minOriginLength)
      : frame(frame), location(location), type(type), labelName(labelName),
        minOriginLength(minOriginLength), scale(1), packedLocation(std::make_pair(-1, -1)),
        handle(-1) {};

  bool isPacked() const {
    return packedLocation.first >= 0 && packedLocation.second >= 0;
  }

  int getArea() const {

    return location.width() * location.height();
  }

  std::pair<int, int> getResizedWidthHeight() const {
    return std::pair<int, int>(std::max(1, (int) (location.width() * scale)),
                               std::max(1, (int) (location.height() * scale)));
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
};

struct MixedFrame {
  const int mixedFrameIndex;
  cv::Mat packedMat;
  std::vector<Frame*> packedFrames;

  int handle;
  std::vector<BoundingBox> boxes;

  MixedFrame(const int mixedFrameIndex, const std::vector<Frame*> packedFrames, const int mixedFrameSize)
      : mixedFrameIndex(mixedFrameIndex), packedFrames(packedFrames) {
    packedMat = cv::Mat::zeros(mixedFrameSize, mixedFrameSize, CV_8UC4);
    for (Frame* frame : packedFrames) {
      for (RoI& roi : frame->rois) {
        if (roi.isPacked()) {
          std::pair<int, int> wh = roi.getResizedWidthHeight();
          roi.getResizedMat().copyTo(
              packedMat(cv::Rect(roi.packedLocation.first, roi.packedLocation.second, wh.first, wh.second)));
        }
      }
    }
  }
};

} // namespace rm

#endif // DATA_TYPE_HPP_
