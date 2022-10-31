#include "strm/MixedFrame.hpp"

#include "strm/RoI.hpp"
#include "strm/Log.hpp"

namespace rm {

int MixedFrame::numMixedFrames = 0;

MixedFrame::MixedFrame(Device device, const std::set<RoI*>& packedRoIs, int mixedFrameSize)
    : device(device), packedRoIs(packedRoIs), mixedFrameIndex(numMixedFrames++),
      mixedFrameSize(mixedFrameSize) {
  packedMat = cv::Mat::zeros(mixedFrameSize, mixedFrameSize, CV_8UC4);
  for (RoI* roi: packedRoIs) {
    assert(roi->isPacked());
    cv::Mat resizedMat = roi->getResizedMat();
    auto[rw, rh] = roi->getResizedMatWidthHeight();
    assert(resizedMat.cols == rw && resizedMat.rows == rh);
    auto[x, y] = roi->packedLocation;
    if (x < 0 || y < 0 || x + float(resizedMat.cols) > float(mixedFrameSize)
        || y + float(resizedMat.rows) > float(mixedFrameSize)) {
      LOGD("%f %f %d %d %d", x, y, resizedMat.cols, resizedMat.rows, mixedFrameSize);
    }
    resizedMat.copyTo(
        packedMat(cv::Rect(roi->packedLocation.first, roi->packedLocation.second,
                           resizedMat.cols, resizedMat.rows)));
    roi->packedAbsMixedFrameIndex = mixedFrameIndex;
  }
}

Stream MixedFrame::getPackedFrames() {
  Stream packedFrames;
  for (RoI* roi: packedRoIs) {
    packedFrames.insert(roi->frame);
  }
  return packedFrames;
}

} // namespace rm
