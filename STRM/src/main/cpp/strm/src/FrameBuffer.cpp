#include "strm/FrameBuffer.hpp"

#include "strm/Frame.hpp"
#include "strm/Log.hpp"

namespace rm {

FrameBuffer::FrameBuffer(int vid, int capacity, int startIndex)
    : vid(vid), count(startIndex) {
  frames.resize(capacity);
}

Frame* FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this, frameIndex]() { return getFrame(frameIndex) == nullptr; });
  Frame* prevFrame = frameIndex == 0 ? nullptr : getFrame(frameIndex - 1);
  frames[frameIndex % frames.size()] = std::make_shared<Frame>(
      vid, frameIndex, mat, prevFrame, NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = getFrame(frameIndex);
  }
  Frame* currFrame = getFrame(frameIndex);
  lock.unlock();
  LOGD("%-25s                 for video %-5d frame %-4d",
       "FrameBuffer::enqueue", vid, frameIndex);
  return currFrame;
}

void FrameBuffer::freeImage(const std::vector<int>& frameIndices) {
  std::unique_lock<std::mutex> lock(mtx);
  // Hide them from any other frame's eyesight
  for (int frameIndex: frameIndices) {
    int index = frameIndex % int(frames.size());
    auto& uframe = frames[index];
    Frame* frame = uframe.get();

    assert(frame != nullptr);
    if (frame->nextFrame != nullptr) {
      assert(frame->nextFrame->prevFrame == frame);
    }
    if (frame->prevFrame != nullptr) {
      assert(frame->prevFrame->nextFrame == frame);
    }
    if (frames[frameIndex % frames.size()]->nextFrame != nullptr) {
      frames[frameIndex % frames.size()]->nextFrame->prevFrame = nullptr;
    }
  }
  // Reset smart pointers
  for (int frameIndex: frameIndices) {
    assert(getFrame(frameIndex) != nullptr);
    frames[frameIndex % frames.size()].reset();
  }
  lock.unlock();
  cv.notify_all();
  LOGD("%-25s                 for video %-5d frame %-4d ~ %-4d",
       "FrameBuffer::freeImage", vid, frameIndices.front(), frameIndices.back());
}

Frame* FrameBuffer::getFrame(int frameIndex) const {
  return frames[frameIndex % frames.size()].get();
}

} // namespace rm
