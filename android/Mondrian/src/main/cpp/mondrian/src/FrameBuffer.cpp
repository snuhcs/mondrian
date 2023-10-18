#include "mondrian/FrameBuffer.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"

namespace md {

FrameBuffer::FrameBuffer(VID vid)
    : vid(vid), frameCount(0) {}

Frame* FrameBuffer::enqueue(const cv::Mat& yuvMat) {
  std::lock_guard<std::mutex> lock(mtx);
  FID fid = frameCount++;
  Frame* prevFrame = frames.empty() ? nullptr : frames.back().get();
  auto currFrame = std::make_unique<Frame>(vid, fid, yuvMat, prevFrame, NowMicros());
  frames.push_back(std::move(currFrame));
  return frames.back().get();
}

void FrameBuffer::freeMats(int tailIndex) {
  std::lock_guard<std::mutex> lock(mtx);
  for (auto& frame : frames) {
    if (!frame->released && frame->fid <= tailIndex) {
      frame->released = true;
      frame->yuvMat.release();
      frame->resizedGrayMat.release();
    }
  }
}

void FrameBuffer::free(int tailIndex) {
  std::lock_guard<std::mutex> lock(mtx);
  while (!frames.empty() && frames.front()->fid <= tailIndex) {
    frames.pop_front();
  }
}

} // namespace md
