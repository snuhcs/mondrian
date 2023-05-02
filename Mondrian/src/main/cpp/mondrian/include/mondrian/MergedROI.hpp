#ifndef MERGED_ROI_HPP_
#define MERGED_ROI_HPP_

#include <memory>
#include <vector>

#include "mondrian/ROI.hpp"

namespace md {

class MergedROI {
 public:
  static int BORDER;
  static const cv::Scalar BORDER_COLOR;
  static const IntPair INVALID_XY;

  MergedROI(const std::vector<ROI*>& rois, float targetScale, bool isProbing);

  static std::unique_ptr<MergedROI> merge(const MergedROI* m0, const MergedROI* m1);

  static std::vector<std::unique_ptr<MergedROI>> mergeROIs(const std::vector<std::unique_ptr<ROI>>& rois, int maxSize);

  float targetScale() const {
    return targetScale_;
  }

  void setTargetScale(float targetScale) {
    targetScale_ = targetScale;
  }

  bool isProbing() const {
    return isProbing_;
  }

  Frame* frame() const {
    return frame_;
  }

  Rect loc() const {
    return loc_;
  }

  BoundingBox* probingBox() const {
    return probingBox_;
  }

  void setProbingBox(BoundingBox* box) {
    probingBox_ = box;
  }

  bool isPacked() const {
    return packedXY_ != INVALID_XY;
  }

  IntPair packedXY() const {
    return packedXY_;
  }

  int relativePackedCanvasIndex() const {
    return relativePackedCanvasIndex_;
  }

  int absolutePackedCanvasIndex() const {
    return absolutePackedCanvasIndex_;
  }

  void setAbsolutePackedCanvasIndex(int absolutePackedCanvasIndex) {
    absolutePackedCanvasIndex_ = absolutePackedCanvasIndex;
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

  int resizedArea() const {
    return resizedAreaOf(loc_.w, loc_.h, targetScale_);
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
      scale = targetScale_;
    }
    return {resizedLengthOf(loc_.w, scale),
            resizedLengthOf(loc_.h, scale)};
  }

  static int borderedLengthOf(float edgeLength, float scale) {
    return resizedLengthOf(edgeLength, scale) + 2 * BORDER;
  }

  IntPair borderedMatWH(float scale = -1.0f) const {
    if (scale == -1.0f) {
      scale = targetScale_;
    }
    return {borderedLengthOf(loc_.w, scale),
            borderedLengthOf(loc_.h, scale)};
  }

  cv::Mat mat() const;

  cv::Mat resizedMat() const;

  cv::Mat borderedMat() const;

  void setPackInfo(IntPair xy, int relativePackedCanvasIndex,
                   ExecutionType executionType, int roiSize);

 private:
  static Frame* frameOf(const std::vector<ROI*>& rois);

  static Rect locOf(const std::vector<ROI*>& rois);

  const std::vector<ROI*> rois_;
  float targetScale_;

  Frame* frame_;
  const Rect loc_;

  IntPair packedXY_;
  int relativePackedCanvasIndex_;
  int absolutePackedCanvasIndex_;
  int packedCanvasSize_;

  bool isProbing_;
  BoundingBox* probingBox_;
};

} // namespace md

#endif // MERGED_ROI_HPP_
