#ifndef ROI_HPP_
#define ROI_HPP_

#include <cmath>

#include "opencv2/core/hal/interface.h"
#include "opencv2/imgproc.hpp"

#include "strm/DataType.hpp"

namespace rm {

class Frame;

enum Type {
  OF = 1,
  PD = 2,
};

class OFFeatures {
 public:
//    const std::vector<std::pair<float, float>> shifts;
//    const std::vector<float> errs;

  std::pair<float, float> shiftAvg;
  std::pair<float, float> shiftStd;
  float shiftNcc;
  float avgErr;

  OFFeatures(const std::vector<std::pair<float, float>>& shifts,
             const std::vector<float>& errs,
             const std::vector<uchar>& statusVec) {
    assert(shifts.size() == errs.size() && errs.size() == statusVec.size());
    if (std::all_of(statusVec.begin(), statusVec.end(),
                    [](const uchar& status) { return status == 0; })) {
      shiftAvg = {0, 0};
      shiftStd = {0, 0};
      shiftNcc = 100;
      avgErr = 100;
      return;
    }
    auto[validShifts, validErrs] = filterShiftsWithStatus(
        shifts, errs, statusVec);
    auto[filteredShifts, filteredErrs] = filterOutlierShifts(validShifts, validErrs);
    assert(!filteredShifts.empty());
    shiftAvg = getShiftAvg(filteredShifts);
    shiftStd = getShiftStd(filteredShifts);
    shiftNcc = getNCC(filteredShifts);
    avgErr = getAvgErr(filteredErrs);
  }

  static std::pair<std::vector<std::pair<float, float>>, std::vector<float>>
  filterShiftsWithStatus(const std::vector<std::pair<float, float>>& shifts,
                         const std::vector<float>& errs,
                         const std::vector<uchar>& statusVec) {
    std::vector<std::pair<float, float>> filteredShifts;
    std::vector<float> filteredErrs;
    for (int i = 0; i < shifts.size(); i++) {
      assert(statusVec[i] == 0 || statusVec[i] == 1);
      if (statusVec[i] == 1) {
        filteredShifts.push_back(shifts[i]);
        filteredErrs.push_back(errs[i]);
      }
    }
    return {filteredShifts, filteredErrs};
  }

  static std::pair<std::vector<std::pair<float, float>>, std::vector<float>>
  filterOutlierShifts(const std::vector<std::pair<float, float>>& shifts,
                      const std::vector<float>& errs) {
    assert(!shifts.empty());
    std::vector<float> distances;
    for (const auto&[x, y]: shifts) {
      distances.push_back(x * x + y * y);
    }
    std::sort(distances.begin(), distances.end());
    int q1_distance = distances[distances.size() / 4];
    std::vector<std::pair<float, float>> filteredShifts;
    std::vector<float> filteredErrs;
    for (int i = 0; i < shifts.size(); i++) {
      auto&[x, y] = shifts[i];
      if (x * x + y * y >= q1_distance) {
        filteredShifts.push_back({x, y});
        filteredErrs.push_back(errs[i]);
      }
    }
    return {filteredShifts, filteredErrs};
  }

  static std::pair<float, float> getShiftAvg(const std::vector<std::pair<float, float>>& shifts) {
    if (shifts.empty()) {
      return {0, 0};
    }
    std::pair<float, float> shift = {0, 0};
    for (const auto&[x, y]: shifts) {
      shift.first += x;
      shift.second += y;
    }
    shift.first /= shifts.size();
    shift.second /= shifts.size();
    return shift;
  }

  static std::pair<float, float> getShiftStd(const std::vector<std::pair<float, float>>& shifts) {
    if (shifts.empty()) {
      return {0, 0};
    }
    std::pair<float, float> var = {0, 0};
    auto[avgX, avgY] = getShiftAvg(shifts);
    for (const auto&[x, y]: shifts) {
      var.first += (x - avgX) * (x - avgX);
      var.second += (y - avgY) * (y - avgY);
    }
    var.first /= shifts.size();
    var.second /= shifts.size();
    return {std::sqrt(var.first), std::sqrt(var.second)};
  }

  static float getAvgErr(const std::vector<float>& errs) {
    if (errs.empty()) {
      return 0;
    }
    float err = 0;
    for (const float& e: errs) {
      err += e;
    }
    return err /= errs.size();
  }

  static float getNCC(const std::vector<std::pair<float, float>>& shifts) {
    if (shifts.size() <= 1) {
      return 0;
    }
    float ncc = 0;
    for (int i = 0; i < shifts.size(); i++) {
      for (int j = i + 1; j < shifts.size(); j++) {
        auto&[Xi, Yi] = shifts[i];
        auto&[Xj, Yj] = shifts[j];
        float sizeI = Xi * Xi + Yi * Yi;
        float sizeJ = Xj * Xj + Yj * Yj;
        if (sizeI == 0 || sizeJ == 0) {
          continue;
        }
        ncc += (Xi * Xj + Yi * Yj) / std::sqrt(sizeI * sizeJ);
      }
    }
    return ncc / (shifts.size() * (shifts.size() - 1) / 2);
  }
};

struct Features {
  float width;
  float height;
  int label;
  Type type;
  Origin origin;
  float xyRatio;
  OFFeatures ofFeatures;
  float confidence;
};

class RoI {
 public:
  static const int INVALID_CONF;

  Frame* frame;
  const Rect origLoc;
  Rect paddedLoc;
  const int roiBorder;

  Type type;
  Origin origin;
  int label;
  Features features;
  std::vector<RoI*> roisForProbing;
  float priority;

  inline static std::atomic<idType> lastId = 0;
  idType id;
  RoI* prevRoI; // only valid with childRoIs
  RoI* nextRoI; // only valid with childRoIs
  std::vector<RoI*> childRoIs;
  RoI* parentRoI;

  float maxEdgeLength;

 private:
  float targetScale;
  int scaleLevel;

  IntPair packedXY;
  int packedMixedFrameIndex;

 public:
  static const IntPair INVALID_XY;
  static const cv::Scalar BORDER_COLOR;

  int packedAbsMixedFrameIndex;
  int packedMixedFrameSize;
  bool isProbingRoI;
  BoundingBox* box;
  BoundingBox* probingBox;

  RoI(RoI* prevRoI,
      const idType id,
      Frame* frame,
      const Rect origLoc,
      const Type type,
      const Origin origin,
      const int label,
      const OFFeatures ofFeatures,
      const float confidence,
      const float roiPadding,
      const int roiBorder,
      const bool isProbingRoI);

  void setPaddedLoc(const Rect& newOrigLoc);

  void eatPD(const Rect& PDRect);

  static std::unique_ptr<RoI> mergeRoIs(const RoI* pRoI0, const RoI* pRoI1);

  static std::pair<idType, idType> getNewIds(unsigned long num) {
    idType minId = lastId.fetch_add(num);
    idType maxId = minId + num;
    // [minId, maxId)
    return std::pair<idType, idType>(minId, maxId);
  }

  bool isPacked() const {
    return packedXY != INVALID_XY;
  }

  bool isParent() const {
    return childRoIs.size() > 1;
  }

  float getPaddedArea() const {
    return paddedLoc.area();
  }

  static int getResizedArea(float width, float height, float scale) {
    int rw = getResizedMatEdgeLength(width, scale);
    int rh = getResizedMatEdgeLength(height, scale);
    return rw * rh;
  }

  int getResizedArea() const {
    return getResizedArea(paddedLoc.width(), paddedLoc.height(), targetScale);
  }

  float getTargetScale() const {
    return targetScale;
  }

  int getScaleLevel() const {
    return scaleLevel;
  }

  void setTargetScale(float newTargetScale, int newScaleLevel);

  IntPair getPackedXY() const {
    return packedXY;
  }

  int getPackedMixedFrameIndex() const {
    return packedMixedFrameIndex;
  }

  void setPackInfo(IntPair xy, int mixedFrameIndex, bool emulatedBatch, int roiSize) {
    if (emulatedBatch) {
      auto[bw, bh] = getBorderMatWidthHeight();
      xy.first += (roiSize - bw) / 2;
      xy.second += (roiSize - bh) / 2;
    }
    packedXY = xy;
    packedMixedFrameIndex = mixedFrameIndex;
  }

  static int toInt(float v) {
    return std::round(v);
  }

  static int getResizedMatEdgeLength(float edgeLength, float scale) {
    return std::max(1, toInt(edgeLength * scale));
  }

  IntPair getResizedMatWidthHeight(float scale = -1) const {
    if (scale == -1) {
      scale = targetScale;
    }
    return {getResizedMatEdgeLength(paddedLoc.width(), scale),
            getResizedMatEdgeLength(paddedLoc.height(), scale)};
  }

  IntPair getBorderMatWidthHeight(float scale = -1) const {
    auto[rw, rh] = getResizedMatWidthHeight(scale);
    return {rw + 2 * roiBorder, rh + 2 * roiBorder};
  }

  cv::Mat getPaddedMat() const;

  cv::Mat getResizedMat() const;

  cv::Mat getBorderMat() const;
};

} // namespace rm

#endif // ROI_HPP_
