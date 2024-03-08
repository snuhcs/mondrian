#ifndef MERGED_ROI_HPP_
#define MERGED_ROI_HPP_

#include <memory>
#include <vector>

#include "mondrian/ROI.hpp"

namespace md {

class BoundingBox;

class MergedROI {
 public:
  static inline int BORDER = 2;
  static inline const cv::Scalar BORDER_COLOR{255, 255, 255};
  static inline const cv::Point2i INVALID_XY{-1, -1};

  MergedROI(ROI* roi, bool isProbing);

  MergedROI(const std::vector<MergedROI*>& mergedROIs);

  static void mergeROIs(std::vector<std::unique_ptr<MergedROI>>& mergedROIs, int maxSize);

  Device targetDevice() const {
    return dispatchTargetDevice_;
  }

  void setDevicePriority(const std::vector<Device>& devicePriority) {
    devicePriority_ = devicePriority;
  }

  const std::vector<Device>& devicePriority() const {
    return devicePriority_;
  }

  float targetScale() const {
    return targetScaleTable_.at(dispatchTargetDevice_);
  }

  float targetScale(Device device) const {
    return targetScaleTable_.at(device);
  }

  const std::map<Device, float>& targetScaleTable() const {
    return targetScaleTable_;
  }

  void setTargetScale(float targetScale) {
    for (auto& [device, targetScale_] : targetScaleTable_) {
      targetScale_ = targetScale;
    }
  }

  bool isProbing() const {
    return isProbing_;
  }

  Frame* frame() const {
    return frame_;
  }

  const std::vector<ROI*>& rois() const {
    return rois_;
  }

  cv::Rect2f loc() const {
    return loc_;
  }

  BoundingBox* probingBox() const {
    return probingBoxTable_.at(PROBING_DEVICE);
  }

  void setProbingBox(BoundingBox* box);

  bool isPacked() const {
    return packedXY_ != INVALID_XY;
  }

  cv::Point2i packedXY() const {
    return packedXY_;
  }

  int packedCanvasIndex() const {
    return packedCanvasIndex_;
  }

  int packedCanvasSize() const {
    return packedCanvasSize_;
  }

  PID pid() const {
    return pid_;
  }

  void setPID(PID pid) {
    pid_ = pid;
  }

  void setPackedCanvasSize(int packedCanvasSize) {
    packedCanvasSize_ = packedCanvasSize;
  }

  bool isInferenced() const {
    return isInferenced_;
  }

  void setInferenced(bool isInferenced) {
    isInferenced_ = isInferenced;
  }

  static int toInt(float v) {
    return std::round(v);
  }

  int borderedArea(Device device) const {
    return borderedAreaOf(loc_.width, loc_.height, targetScaleTable_.at(device));
  }

  static int borderedAreaOf(float width, float height, float scale) {
    int bw = borderedLengthOf(width, scale);
    int bh = borderedLengthOf(height, scale);
    return bw * bh;
  }

  static int resizedLengthOf(float edgeLength, float scale) {
    return std::max(1, toInt(edgeLength * scale));
  }

  cv::Size2i resizedMatWH() const {
    return {resizedLengthOf(loc_.width, targetScale()),
            resizedLengthOf(loc_.height, targetScale())};
  }

  static int borderedLengthOf(float edgeLength, float scale) {
    return resizedLengthOf(edgeLength, scale) + 2 * BORDER;
  }

  cv::Size2i borderedMatWH(float scale) {
    return {borderedLengthOf(loc_.width, scale),
            borderedLengthOf(loc_.height, scale)};
  }

  cv::Size2i borderedMatWH(Device device) const {
    assert(device != Device::INVALID);
    return {borderedLengthOf(loc_.width, targetScale(device)),
            borderedLengthOf(loc_.height, targetScale(device))};
  }

  cv::Mat mat() const;

  cv::Mat resizedMat(Device device) const;

  cv::Mat borderedMat(Device device) const;

  void setPackInfo(Device device,
                   cv::Point2i xy,
                   int packedCanvasIndex,
                   ExecutionType executionType,
                   int roiSize);

  cv::Rect2i packedLoc() const;

  cv::Rect2f reconstructBoxPos(const BoundingBox& packedBox) const;

  static std::string header();

  std::string str(time_us baseTime) const;

 private:
  Frame* const frame_;
  const std::vector<ROI*> rois_;
  const cv::Rect2f loc_;
  std::map<Device, float> targetScaleTable_;
  std::vector<Device> devicePriority_;

  PID pid_;
  cv::Point2i packedXY_;
  int packedCanvasIndex_;
  int packedCanvasSize_;

  bool isInferenced_;
  Device dispatchTargetDevice_;

  bool isProbing_;
  std::map<Device, BoundingBox*> probingBoxTable_;
  std::map<Device, BID> probingBoxIDTable_;

 public:
  time_us packedInferenceStartTime = 0;
  time_us packedInferenceEndTime = 0;
  time_us reconstructStartTime = 0;
  time_us reconstructEndTime = 0;
};

} // namespace md

#endif // MERGED_ROI_HPP_
