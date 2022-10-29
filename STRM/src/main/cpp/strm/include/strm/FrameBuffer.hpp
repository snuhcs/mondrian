#ifndef FRAME_BUFFER_HPP_
#define FRAME_BUFFER_HPP_

#include <condition_variable>
#include <mutex>

#include "opencv2/core/mat.hpp"

namespace rm {

class Frame;

class FrameBuffer {
 public:
  FrameBuffer(int vid, int capacity, int startIndex);

  Frame* enqueue(const cv::Mat& mat);

  void freeImage(const std::vector<int>& frameIndices);

 private:
  const int vid;
  const int capacity;

  int count;
  std::mutex mtx;
  std::condition_variable cv;
  std::vector<std::unique_ptr<Frame>> frames;
};

} // namespace rm

#endif // FRAME_BUFFER_HPP_
