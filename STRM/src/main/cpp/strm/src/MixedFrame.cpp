#include "strm/MixedFrame.hpp"

#include "strm/RoI.hpp"

namespace rm {

int MixedFrame::numMixedFrames = 0;

MixedFrame::MixedFrame(Device device, const std::set<RoI*>& packedRoIs, int mixedFrameSize,
                       bool emulatedBatch, int roiSize)
    : device(device), packedRoIs(packedRoIs), mixedFrameSize(mixedFrameSize),
      packedMat(mixedFrameSize, mixedFrameSize, CV_8UC4, cv::Scalar(114, 114, 114, 255)),
      mixedFrameIndex(numMixedFrames++) {
  // TODO: Handle different background colors according to the model
  // (e.g. white for YOLOv4, gray for YOLOv5)
  for (RoI* roi: packedRoIs) {
    assert(roi->isPacked());
    cv::Mat resizedMat = roi->getResizedMat(emulatedBatch, roiSize);
    int rw = resizedMat.cols;
    int rh = resizedMat.rows;
    auto[packX, packY] = roi->getPackedXY();
    resizedMat.copyTo(
        packedMat(cv::Rect(packX, packY, rw, rh)));
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
