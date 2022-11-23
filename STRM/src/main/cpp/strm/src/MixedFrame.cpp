#include "strm/MixedFrame.hpp"

#include "strm/Log.hpp"
#include "strm/RoI.hpp"

namespace rm {

int MixedFrame::numMixedFrames = 0;

MixedFrame::MixedFrame(Device device, const std::set<RoI*>& packedRoIs, int mixedFrameSize)
    : device(device), packedRoIs(packedRoIs), mixedFrameSize(mixedFrameSize),
      packedMat(mixedFrameSize, mixedFrameSize, CV_8UC4, cv::Scalar(114, 114, 114, 255)),
      mixedFrameIndex(numMixedFrames++) {
  // TODO: Handle different background colors according to the model
  // (e.g. white for YOLOv4, gray for YOLOv5)
  for (RoI* roi: packedRoIs) {
    assert(roi->isPacked());
    cv::Mat resizedMat = roi->getResizedMat();
    int rw = resizedMat.cols;
    int rh = resizedMat.rows;
    auto[packX, packY] = roi->getPackedXY();

    cv::Rect rect(packX, packY, rw, rh);
    auto& m = packedMat;
    if (!(0 <= rect.x && 0 <= rect.width && rect.x + rect.width <= m.cols && 0 <= rect.y &&
          0 <= rect.height && rect.y + rect.height <= m.rows)) {
      LOGE("MixedFrame packedMat(%4d, %4d), RoI(x=%4d, y=%4d, w=%4d, h=%4d)",
           m.cols, m.rows, rect.x, rect.y, rect.width, rect.height);
      assert(false);
    }
    resizedMat.copyTo(packedMat(rect));
    roi->packedAbsMixedFrameIndex = mixedFrameIndex;
  }
}

Stream MixedFrame::getPackedFrames() const {
  Stream packedFrames;
  for (RoI* roi: packedRoIs) {
    packedFrames.insert(roi->frame);
  }
  return packedFrames;
}

} // namespace rm
