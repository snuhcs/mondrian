#include "mondrian/PackedCanvas.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/ROI.hpp"

namespace md {

PackedCanvas::PackedCanvas(const std::set<MergedROI*>& mergedROIs,
                           int packedCanvasSize,
                           Device device)
    : device(device), packedROIs(mergedROIs), packedCanvasSize(packedCanvasSize),
      packedMat(packedCanvasSize, packedCanvasSize, CV_8UC3, cv::Scalar(114, 114, 114)),
      pid(nextPID_++) {
  // TODO: Handle different background colors according to the model
  //       e.g. white for YOLOv4, gray for YOLOv5
  for (MergedROI* mergedROI : mergedROIs) {
    assert(mergedROI->isPacked());
    cv::Mat borderMat = mergedROI->borderedMat(device);
    int bw = borderMat.cols;
    int bh = borderMat.rows;
    auto [packX, packY] = mergedROI->packedXY();

    cv::Rect2i rect(packX, packY, bw, bh);
    auto& m = packedMat;
    if (!(0 <= rect.x && 0 <= rect.width && rect.x + rect.width <= m.cols && 0 <= rect.y &&
        0 <= rect.height && rect.y + rect.height <= m.rows)) {
      LOGE("PackedCanvas packedMat(%4d, %4d), ROI(x=%4d, y=%4d, w=%4d, h=%4d)",
           m.cols, m.rows, rect.x, rect.y, rect.width, rect.height);
      assert(false);
    }
    borderMat.copyTo(packedMat(rect));
    mergedROI->setPID(pid);
    mergedROI->setPackedCanvasSize(packedCanvasSize);
  }
}

Stream PackedCanvas::getPackedFrames() const {
  FrameSet packedFrameSet;
  for (MergedROI* mergedROI : packedROIs) {
    packedFrameSet.insert(mergedROI->frame());
  }
  Stream packedFrames(packedFrameSet.begin(), packedFrameSet.end());
  return packedFrames;
}

} // namespace md
