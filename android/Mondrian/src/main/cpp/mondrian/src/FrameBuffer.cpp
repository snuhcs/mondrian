#include "mondrian/FrameBuffer.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"

namespace md {

FrameBuffer::FrameBuffer(int vid, int capacity, bool blocking)
    : vid(vid), capacity(capacity), blocking(blocking),
      head(0), tail(0) {
  frames.reserve(capacity);
  for (int i = 0; i < capacity; ++i) {
    frames.emplace_back(nullptr);
  }
}

Frame* FrameBuffer::enqueue(const cv::Mat& yuvMat) {
  if (blocking) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return head - tail < capacity; });
    return enqueue(head++, yuvMat);
  } else {
    assert(head - tail < capacity);
    return enqueue(head++, yuvMat);
  }
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
  std::unique_lock<std::mutex> lock(mtx);
  tail = std::max(tail, tailIndex);
  lock.unlock();
  cv.notify_all();
}

} // namespace md
