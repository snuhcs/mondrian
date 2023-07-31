#include "mondrian/FrameBuffer.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"

namespace md {

FrameBuffer::FrameBuffer(int vid, int capacity)
    : vid(vid), capacity(capacity), head(0), tail(0) {
  frames.reserve(capacity);
  for (int i = 0; i < capacity; ++i) {
    frames.emplace_back(nullptr);
  }
}

Frame* FrameBuffer::enqueue(const cv::Mat& yuvMat) {
  assert(head - tail < capacity);
  return enqueue(head++, yuvMat);
}

Frame* FrameBuffer::enqueue(int frameIndex, const cv::Mat& yuvMat) {
  Frame* prevFrame = frameIndex != 0
                     ? frames[(frameIndex - 1) % capacity].get()
                     : nullptr;
  auto currFrame = std::make_unique<Frame>(vid, frameIndex, yuvMat, prevFrame, NowMicros());
  frames[frameIndex % capacity] = std::move(currFrame);
  return frames[frameIndex % capacity].get();
}

void FrameBuffer::free(int tailIndex) {
  std::lock_guard<std::mutex> lock(mtx);
  tail = std::max(tail, tailIndex);
}

} // namespace md
