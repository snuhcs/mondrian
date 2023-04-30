#ifndef MIXED_FRAME_HPP_
#define MIXED_FRAME_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/Frame.hpp"

namespace md {

class MixedFrame {
 public:
  static int numMixedFrames;
  const Device device;
  const int mixedFrameIndex;
  const int mixedFrameSize;
  cv::Mat packedMat;
  std::set<ROI*> packedROIs;

  MixedFrame(Device device, const std::set<ROI*>& packedROIs, int mixedFrameSize);

  Stream getPackedFrames() const;

  int getKey() const {
    return mixedFrameIndex;
  }
};

} // namespace md

#endif // MIXED_FRAME_HPP_
