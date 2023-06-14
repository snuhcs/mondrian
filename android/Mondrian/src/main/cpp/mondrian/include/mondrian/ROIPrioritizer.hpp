#ifndef ROI_PRIORITIZER_HPP_
#define ROI_PRIORITIZER_HPP_

#include <set>

#include "mondrian/Frame.hpp"

namespace md {

class MergedROI;

class StartEndLength {
 public:
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

  bool contains(int vid, int roiID, int mid) const {
    return vid_ == vid && roiID_ == roiID && start_ <= mid && mid < end_;
  }

  bool operator<(const StartEndLength& other) const {
    if (length_ == other.length_) {
      return start_ > other.start_;
    }
    return length_ > other.length_;
  }
};

class ROIPrioritizer {
 public:
  static std::vector<MergedROI*> order(const MultiStream& mergedROIs, int fullFrameVid);
};

} // namespace md

#endif // ROI_PRIORITIZER_HPP_
