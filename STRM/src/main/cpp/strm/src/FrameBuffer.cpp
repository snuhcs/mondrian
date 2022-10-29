#include "strm/FrameBuffer.hpp"

#include "strm/Frame.hpp"
#include "strm/Log.hpp"

namespace rm {

FrameBuffer::FrameBuffer(int vid, int capacity, int startIndex)
    : vid(vid), capacity(capacity), count(startIndex) {
  frames.resize(capacity);
}

Frame* FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this, frameIndex]() { return frames[frameIndex % capacity].get() == nullptr; });
  Frame* prevFrame = frameIndex == 0 ? nullptr : frames[(frameIndex - 1) % capacity].get();
  frames[frameIndex % capacity] = std::make_unique<Frame>(
      vid, frameIndex, mat, prevFrame, NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[frameIndex % capacity].get();
  }
  Frame* currFrame = frames[frameIndex % capacity].get();
  lock.unlock();
  LOGD("%-25s                 for video %-5d frame %-4d",
       "FrameBuffer::enqueue", vid, frameIndex);
  return currFrame;
}

void FrameBuffer::freeImage(const std::vector<int>& frameIndices) {
  std::unique_lock<std::mutex> lock(mtx);
  // Hide them from any other frame's eyesight
  for (int frameIndex: frameIndices) {
    auto frame = frames[frameIndex % capacity].get();

    assert(frame != nullptr);
    assert(frame->nextFrame != nullptr);
    assert(frame->nextFrame->prevFrame == frame);
    if (frame->prevFrame != nullptr) {
      assert(frame->prevFrame->nextFrame == frame);
    }
    frames[frameIndex % capacity]->nextFrame->prevFrame = nullptr;
  }
  // Reset smart pointers
  for (int frameIndex: frameIndices) {
    assert(frames[frameIndex % capacity].get() != nullptr);
    frames[frameIndex % capacity].reset();
  }
  lock.unlock();
  cv.notify_all();
  LOGD("%-25s                 for video %-5d frame %-4d ~ %-4d",
       "FrameBuffer::freeImage", vid, frameIndices.front(), frameIndices.back());
}

} // namespace rm
