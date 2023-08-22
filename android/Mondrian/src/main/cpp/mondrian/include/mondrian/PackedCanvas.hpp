#ifndef PACKED_CANVAS_HPP_
#define PACKED_CANVAS_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/Frame.hpp"

namespace md {

class PackedCanvas {
 public:
  static inline const int PACKED_CANVAS_VID = -1;
  static inline std::atomic<PID> nextPID_ = 0;

  const PID pid;

  std::set<MergedROI*> packedROIs;
  const Device device; // TODO: change name to less general one. (usage of this field is not clear due to its too general name)
  const int packedCanvasSize;

  cv::Mat packedMat;

  PackedCanvas(const std::set<MergedROI*>& mergedROIs, int packedCanvasSize, Device device);

  Stream getPackedFrames() const;

  Key getKey() const {
    return {PACKED_CANVAS_VID, pid};
  }
};

} // namespace md

#endif // PACKED_CANVAS_HPP_
