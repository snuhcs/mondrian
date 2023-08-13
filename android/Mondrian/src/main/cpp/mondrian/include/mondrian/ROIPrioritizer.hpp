#ifndef ROI_PRIORITIZER_HPP_
#define ROI_PRIORITIZER_HPP_

#include <set>

#include "mondrian/Frame.hpp"

namespace md {

class MergedROI;

struct StartEndLength {
  int vid_;
  int roiID_;
  int start_;
  int end_;
  int length_;

  StartEndLength(int vid, int roiID, int start, int end)
      : vid_(vid), roiID_(roiID), start_(start), end_(end), length_(end - start) {}

  int mid() const {
    return start_ + length_ / 2;
  }

  bool operator<(const StartEndLength& other) const {
    if (length_ != other.length_) {
      return length_ > other.length_;
    }
    if (start_ != other.start_) {
      return start_ > other.start_;
    }
    assert(end_ == other.end_);
    if (roiID_ != other.roiID_) {
      return roiID_ > other.roiID_;
    }
    if (vid_ != other.vid_) {
      return vid_ > other.vid_;
    }
    return vid_ > other.vid_;
  }
};

struct MergedROIoFPriorityComparator {
  bool operator()(const MergedROI* lhs, const MergedROI* rhs) const {
    auto errOf = [](const MergedROI* mergedROI) {
      float err = 0;
      for (const auto& roi : mergedROI->rois()) {
        err += roi->features.ofFeatures.shiftNcc + roi->features.ofFeatures.avgErr;
      }
      return err;
    };
    float lErr = errOf(lhs);
    float rErr = errOf(rhs);
    if (lErr != rErr) {
      return lErr > rErr;
    }

    // For MergedROI identity.
    if (lhs->frame() != rhs->frame()) {
      return lhs->frame() > rhs->frame();
    }
    return lhs > rhs;
  }
};

class ROIPrioritizer {
 public:
  static std::vector<MergedROI*> order(const MultiStream& streams,
                                       const ROIPackerType type);

 private:
  static std::vector<MergedROI*> minConsecutiveDrop(const MultiStream& streams);

  static std::vector<MergedROI*> ofConfidence(const MultiStream& streams);
};

} // namespace md

#endif // ROI_PRIORITIZER_HPP_
