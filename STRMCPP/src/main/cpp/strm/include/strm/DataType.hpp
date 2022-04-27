#ifndef DATA_TYPE_HPP_
#define DATA_TYPE_HPP_

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <fstream>

#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "strm/Time.hpp"

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

  const time_us enqueueTime;
  time_us dispatcherProcessStartTime = 0;
  time_us fullFrameEnqueueTime = 0;
  time_us fullFrameGetResultsTime = 0;
  time_us opticalFlowRoIProcessStartTime = 0;
  time_us opticalFlowRoIProcessEndTime = 0;
  time_us pixelDiffRoIProcessStartTime = 0;
  time_us pixelDiffRoIProcessEndTime = 0;
  time_us mergeRoIStartTime = 0;
  time_us mergeRoIEndTime = 0;
  time_us resizeRoIStartTime = 0;
  time_us resizeRoIEndTime = 0;
  time_us mixingStartTime = 0;
  time_us mixingEndTime = 0;
  time_us mixedFrameCreateStartTime = 0;
  time_us mixedFrameCreateEndTime = 0;
  time_us mixedFrameEnqueueTime = 0;
  time_us reconstructStartTime = 0;
  time_us reconstructEndTime = 0;
  time_us endTime = 0;

  Frame(const std::string& key, const int frameIndex, const cv::Mat mat,
        const time_us& enqueueTime)
      : key(key), frameIndex(frameIndex), mat(mat), isResultReady(false),
        enqueueTime(enqueueTime) {}

  ~Frame() {
    endTime = NowMicros();
  }
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

  RoI(const Frame* frame, const Rect location, const Type type, const std::string labelName,
      const int minOriginLength)
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

  MixedFrame(const int mixedFrameIndex, const std::vector<Frame*> packedFrames,
             const int mixedFrameSize, const bool mixing)
      : mixedFrameIndex(mixedFrameIndex), packedFrames(packedFrames) {
    if (mixing) {
      const time_us mixedFrameCreateStartTime = NowMicros();
      packedMat = cv::Mat::zeros(mixedFrameSize, mixedFrameSize, CV_8UC4);
      for (Frame* frame : packedFrames) {
        for (RoI& roi : frame->rois) {
          if (roi.isPacked()) {
            std::pair<int, int> wh = roi.getResizedWidthHeight();
            roi.getResizedMat().copyTo(
                packedMat(cv::Rect(roi.packedLocation.first, roi.packedLocation.second,
                                   wh.first, wh.second)));
          }
        }
      }
      const time_us mixedFrameCreateEndTime = NowMicros();
      for (Frame* frame : packedFrames) {
        frame->mixedFrameCreateStartTime = mixedFrameCreateStartTime;
        frame->mixedFrameCreateEndTime = mixedFrameCreateEndTime;
      }
    }
  }
};

} // namespace rm

#endif // DATA_TYPE_HPP_
