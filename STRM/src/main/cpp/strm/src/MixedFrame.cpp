#include "strm/MixedFrame.hpp"

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
    resizedMat.copyTo(
        packedMat(cv::Rect(roi->packedXY.first, roi->packedXY.second,
                           resizedMat.cols, resizedMat.rows)));
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
