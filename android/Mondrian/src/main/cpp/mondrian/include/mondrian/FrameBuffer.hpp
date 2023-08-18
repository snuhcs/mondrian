#ifndef FRAME_BUFFER_HPP_
#define FRAME_BUFFER_HPP_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"

namespace md {

class Frame;

class FrameBuffer {
 public:
  FrameBuffer(VID vid);

  Frame* enqueue(const cv::Mat& yuvMat);

  void free(int tailIndex);

 private:
  const VID vid;

  int frameCount;
  std::queue<std::unique_ptr<Frame>> frames;
  std::mutex mtx;
};

} // namespace md

#endif // FRAME_BUFFER_HPP_
