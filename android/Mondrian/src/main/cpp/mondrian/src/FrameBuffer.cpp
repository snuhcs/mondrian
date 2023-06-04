#include "mondrian/FrameBuffer.hpp"

#include "mondrian/Frame.hpp"
#include "mondrian/Log.hpp"

namespace md {

FrameBuffer::FrameBuffer(int vid, int capacity)
    : vid(vid), capacity(capacity), count(0) {}

Frame* FrameBuffer::enqueue(const cv::Mat& yuvMat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this]() { return frames.size() < capacity; });
  Frame* prevFrame = frames.find(frameIndex - 1) != frames.end()
                     ? frames.at(frameIndex - 1).get()
                     : nullptr;
  frames[frameIndex] = std::make_unique<Frame>(
      vid, frameIndex, yuvMat, prevFrame, NowMicros());
  lock.unlock();
  LOGD("%-25s                 for video %-5d frame %-4d",
       "FrameBuffer::enqueue", vid, frameIndex);
  return frames[frameIndex].get();
}

void FrameBuffer::freeImage(const std::vector<int>& frameIndices) {
  std::unique_lock<std::mutex> lock(mtx);
  // Reset smart pointers
  for (int frameIndex: frameIndices) {
    assert(frames[frameIndex] != nullptr);
    frames.erase(frameIndex);
  }
  lock.unlock();
  cv.notify_all();
  LOGD("%-25s                 for video %-5d frame %-4d ~ %-4d",
       "FrameBuffer::freeImage", vid, frameIndices.front(), frameIndices.back());
}

} // namespace md
