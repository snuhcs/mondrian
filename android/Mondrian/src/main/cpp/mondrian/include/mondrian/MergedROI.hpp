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

  MergedROI(const std::vector<ROI*>& rois,
            const std::map<Device, float>& targetScaleTable,
            bool isProbing);

  static std::unique_ptr<MergedROI> merge(const MergedROI* m0, const MergedROI* m1);

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

  Rect loc() const {
    return loc_;
  }

  BoundingBox* probingBox() const {
    return probingBoxTable_.at(PROBING_DEVICE);
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
    return borderedAreaOf(loc_.w, loc_.h, targetScaleTable_.at(device));
  }

  static int borderedAreaOf(float width, float height, float scale) {
    int bw = borderedLengthOf(width, scale);
    int bh = borderedLengthOf(height, scale);
    return bw * bh;
  }

  static int resizedLengthOf(float edgeLength, float scale) {
    return std::max(1, toInt(edgeLength * scale));
  }

  IntPair resizedMatWH() const {
    return {resizedLengthOf(loc_.w, targetScale()),
            resizedLengthOf(loc_.h, targetScale())};
  }

  static int borderedLengthOf(float edgeLength, float scale) {
    return resizedLengthOf(edgeLength, scale) + 2 * BORDER;
  }

  IntPair borderedMatWH(float scale) {
    return {borderedLengthOf(loc_.w, scale),
            borderedLengthOf(loc_.h, scale)};
  }

  IntPair borderedMatWH(Device device) const {
    assert(device != Device::INVALID);
    return {borderedLengthOf(loc_.w, targetScale(device)),
            borderedLengthOf(loc_.h, targetScale(device))};
  }

  cv::Mat mat() const;

  cv::Mat resizedMat(Device device) const;

  cv::Mat borderedMat(Device device) const;

  void setPackInfo(Device device,
                   IntPair xy,
                   int packedCanvasIndex,
                   ExecutionType executionType,
                   int roiSize);

  static std::string header();

  std::string str() const;

 private:
  static Frame* frameOf(const std::vector<ROI*>& rois);

  static Rect locOf(const std::vector<ROI*>& rois);

  Frame* const frame_;
  const std::vector<ROI*> rois_;
  const Rect loc_;
  std::map<Device, float> targetScaleTable_;
  std::vector<Device> devicePriority_;

  PID pid_;
  IntPair packedXY_;
  int packedCanvasIndex_;
  int packedCanvasSize_;
  bool isInferenced_;
  Device dispatchTargetDevice_;

  bool isProbing_;
  std::map<Device, BoundingBox*> probingBoxTable_;
  std::map<Device, BID> probingBoxIDTable_;
};

} // namespace md

#endif // MERGED_ROI_HPP_
