#ifndef FRAME_BUFFER_HPP_
#define FRAME_BUFFER_HPP_

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "opencv2/core/mat.hpp"

namespace md {

class Frame;

class FrameBuffer {
 public:
  FrameBuffer(int vid, int capacity);

  Frame* enqueue(const cv::Mat& yuvMat);

  void free(int tailIndex);

 private:
  Frame* enqueue(int frameIndex, const cv::Mat& yuvMat);

  const int vid;
  const int capacity;

  std::vector<std::unique_ptr<Frame>> frames;

  std::atomic<int> head;
  int tail;
  std::mutex mtx;
};

} // namespace md

#endif // FRAME_BUFFER_HPP_
