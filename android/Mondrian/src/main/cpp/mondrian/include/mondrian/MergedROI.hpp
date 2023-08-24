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
  static inline const IntPair INVALID_XY{-1, -1};

  MergedROI(const std::vector<ROI*>& rois, std::map<Device, float> targetScaleTable, bool isProbing);

  static std::unique_ptr<MergedROI> merge(const MergedROI* m0, const MergedROI* m1);

  static void mergeROIs(std::vector<std::unique_ptr<MergedROI>>& mergedROIs, int maxSize);

  void dispatchTo(Device device);

  bool isDispatched() const {
    return dispatchTargetDevice != Device::INVALID;
  }

  void setTargetDevice(Device device) {
    dispatchTargetDevice = device;
  }

  Device getTargetDevice() const {
    return dispatchTargetDevice;
  }


  float targetScale() const {
    return targetScaleTable_.at(Device::GPU);
  }

  void setTargetScale(float targetScale) {
    targetScaleTable_[Device::GPU] = targetScale;
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

  Rect loc() const {
    return loc_;
  }

  BoundingBox* probingBox() const {
    return probingBoxTable_.at(Device::GPU);
  }

  void setProbingBox(BoundingBox* box);

  bool isPacked() const {
    return packedXY_ != INVALID_XY;
  }

  IntPair packedXY() const {
    return packedXY_;
  }

  int packedCanvasIndex() const {
    return packedCanvasIndex_;
  }

  PID pid() const {
    return pid_;
  }

  void setPID(PID pid) {
    pid_ = pid;
  }

  int packedCanvasSize() const {
    return packedCanvasSize_;
  }

  void setPackedCanvasSize(int packedCanvasSize) {
    packedCanvasSize_ = packedCanvasSize;
  }

  static int toInt(float v) {
    return std::round(v);
  }

  int resizedArea(Device device) const {
    return resizedAreaOf(loc_.w, loc_.h, targetScaleTable_.at(device));
  }

  static int resizedAreaOf(float width, float height, float scale) {
    int rw = resizedLengthOf(width, scale);
    int rh = resizedLengthOf(height, scale);
    return rw * rh;
  }

  static int resizedLengthOf(float edgeLength, float scale) {
    return std::max(1, toInt(edgeLength * scale));
  }

  IntPair resizedMatWH(float scale = -1.0f) const {
    if (scale == -1.0f) {
      scale = targetScaleTable_.at(Device::GPU);
    }
    return {resizedLengthOf(loc_.w, scale),
            resizedLengthOf(loc_.h, scale)};
  }

  static int borderedLengthOf(float edgeLength, float scale) {
    return resizedLengthOf(edgeLength, scale) + 2 * BORDER;
  }

  IntPair borderedMatWH(float scale = -1.0f) const {
    if (scale == -1.0f) {
      scale = targetScaleTable_.at(Device::GPU);
    }
    return {borderedLengthOf(loc_.w, scale),
            borderedLengthOf(loc_.h, scale)};
  }

  cv::Mat mat() const;

  cv::Mat resizedMat(Device device) const;

  cv::Mat borderedMat(Device device) const;

  void setPackInfo(IntPair xy, int packedCanvasIndex,
                   ExecutionType executionType, int roiSize);

  static std::string header();

  std::string str() const;

 private:
  static Frame* frameOf(const std::vector<ROI*>& rois);

  static Rect locOf(const std::vector<ROI*>& rois);

  Frame* const frame_;
  const std::vector<ROI*> rois_;
  const Rect loc_;
  std::map<Device, float> targetScaleTable_;

  PID pid_;
  IntPair packedXY_;
  int packedCanvasIndex_;
  int packedCanvasSize_;
  Device dispatchTargetDevice;

  bool isProbing_;
  std::map<Device, BoundingBox*> probingBoxTable_;
  std::map<Device, BID> probingBoxIDTable_;
};

} // namespace md

#endif // MERGED_ROI_HPP_
