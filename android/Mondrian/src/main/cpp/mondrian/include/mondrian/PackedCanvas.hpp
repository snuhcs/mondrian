#ifndef PACKED_CANVAS_HPP_
#define PACKED_CANVAS_HPP_

#include <set>

#include "opencv2/core/mat.hpp"

#include "mondrian/Frame.hpp"

namespace md {

class PackedCanvas {
 public:
  static const int PACKED_CANVAS_VID;
  std::set<MergedROI*> packedROIs;
  const Device device;
  const int packedCanvasSize;

  cv::Mat packedMat;
  const int absolutePackedCanvasIndex;

  PackedCanvas(const std::set<MergedROI*>& mergedROIs, int packedCanvasSize, Device device);

  Stream getPackedFrames() const;

  Key getKey() const {
    return {PACKED_CANVAS_VID, absolutePackedCanvasIndex};
  }

 private:
  static int numPackedCanvases;
};

} // namespace md

#endif // PACKED_CANVAS_HPP_
