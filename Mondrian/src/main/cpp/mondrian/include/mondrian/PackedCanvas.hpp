#ifndef PACKED_CANVAS_HPP_
#define PACKED_CANVAS_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/Frame.hpp"

namespace md {

class PackedCanvas {
 public:
  std::set<MergedROI*> packedROIs;
  const Device device;
  const int packedCanvasSize;

  cv::Mat packedMat;
  const int absolutePackedCanvasIndex;

  PackedCanvas(const std::set<MergedROI*>& mergedROIs, int packedCanvasSize, Device device);

  Stream getPackedFrames() const;

  int getKey() const {
    return absolutePackedCanvasIndex;
  }

 private:
  static int numPackedCanvases;
};

} // namespace md

#endif // PACKED_CANVAS_HPP_
