#ifndef MIXED_FRAME_HPP_
#define MIXED_FRAME_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "strm/Frame.hpp"

namespace rm {

class MixedFrame {
 public:
  static int numMixedFrames;
  const Device device;
  const int mixedFrameIndex;
  const int mixedFrameSize;
  cv::Mat packedMat;
  std::set<RoI*> packedRoIs;

  MixedFrame(Device device, const std::set<RoI*>& packedRoIs, int mixedFrameSize);

  Stream getPackedFrames() const;
};

} // namespace rm

#endif // MIXED_FRAME_HPP_
