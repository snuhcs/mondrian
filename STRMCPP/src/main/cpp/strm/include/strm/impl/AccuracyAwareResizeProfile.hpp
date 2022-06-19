#ifndef IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_
#define IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_

#include "strm/ResizeProfile.hpp"
#include "strm/impl/ImplConfig.hpp"
#include "strm/impl/CustomTreeOF.hpp"
#include "strm/impl/CustomTreePD.hpp"

namespace rm {

class AccuracyAwareResizeProfile : public ResizeProfile {
 public:
  AccuracyAwareResizeProfile(int resizeMargin)
      : RESIZE_MARGIN(resizeMargin) {}

  int getTargetSize(const idType id, const RoI::Features& features) const {
    auto record = sizeTable.find(id);
    int sizeWithFeatures = getSizeWithFeature(features);

    /*
     * if record exists and record's previous size with features matches, return size in record
     * else return size with features
     */
    return (record != sizeTable.end() && record->second.first == sizeWithFeatures) ?
           record->second.second :
           sizeWithFeatures;
  }

  int getSizeWithFeature(const RoI::Features& features) const {
    if (features.type == RoI::OF) {
      return (int) OFTree(features.xyRatio, features.getShiftSize(), features.err) + RESIZE_MARGIN;
    } else if (features.type == RoI::PD) {
      return (int) PDTree(features.xyRatio, features.diffAreaRatio) + RESIZE_MARGIN;
    }
    return INT_MAX;
  }

  void updateTable(RoI& roi) {
    if (roi.roisForProbing.empty()) {
      return;
    }

    // find box from largest RoI
    BoundingBox* boxFromLargestRoI = nullptr;
    for (auto it = roi.roisForProbing.rbegin(); it != roi.roisForProbing.rend(); it++) {
      boxFromLargestRoI = it->probingBox;
      if (boxFromLargestRoI != nullptr) {
        break;
      }
    }

    // if box is found nowhere, record to use even bigger size than biggest size
    if (boxFromLargestRoI == nullptr) {
      if (sizeTable.find(roi.id) != sizeTable.end()) {
        sizeTable.erase(roi.id);
      }
      if (roi.roisForProbing.size() > 1) {
        int maxSize = roi.roisForProbing.rbegin()->targetSize;
        int minSize = roi.roisForProbing.begin()->targetSize;
        int evenBiggerSize = maxSize + (maxSize - minSize) / (roi.roisForProbing.size() - 1);
        sizeTable.insert(
            {roi.id,
             std::make_pair(getSizeWithFeature(roi.features), evenBiggerSize)});
      }
      return;
    }

    // from largest RoI to smallest RoI, find smallest target size with marginal confidence & IoU loss
    int smallestSizePossible = boxFromLargestRoI->targetSize;
    for (int i = roi.roisForProbing.size() - 1; i >= 0; --i) {
      BoundingBox* probeBox = roi.roisForProbing[i].probingBox;
      // if box not found, stop checking
      if (probeBox == nullptr) {
        break;
      }
      // check if the size is usable
      if (isUsable(*probeBox, *boxFromLargestRoI)) {
        smallestSizePossible = probeBox->targetSize;
      } else {
        break;
      }
    }

    // update table
    if (sizeTable.find(roi.id) != sizeTable.end()) {
      sizeTable.erase(roi.id);
    }
    sizeTable.insert({roi.id, std::make_pair(getSizeWithFeature(roi.features), smallestSizePossible)});
  }

  float getOverlap(Rect& targetRect, Rect& baseRect) {
    int intersection = targetRect.intersection(baseRect);
    float overlapRatio = (float) intersection / (float) (baseRect.area());
    return overlapRatio;
  }

  bool isUsable(BoundingBox& targetBox, BoundingBox& baseBox) {
    float overlapThreshold = 0.8;
    float confidenceThreshold = 0.3;
    float confidenceDiffThreshold = 0.1;
    return (getOverlap(targetBox.location, baseBox.location) > overlapThreshold)
           && (targetBox.confidence > confidenceThreshold)
           && ((baseBox.confidence - targetBox.confidence) < confidenceDiffThreshold);

  }

 private:
  const int RESIZE_MARGIN;
  std::map<idType, std::pair<int, int>> sizeTable; // id, result from tree, result from probing
};

} // namespace rm

#endif // IMPL_ACCURACY_AWARE_RESIZE_PROFILE_HPP_
