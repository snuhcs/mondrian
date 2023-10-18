#ifndef FRAME_BUFFER_HPP_
#define FRAME_BUFFER_HPP_

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>

#include "opencv2/core/mat.hpp"

#include "mondrian/DataType.hpp"

namespace md {

class Frame;

class FrameBuffer {
 public:
  FrameBuffer(VID vid);

  Frame* enqueue(const cv::Mat& yuvMat);

  void freeMats(int tailIndex);

  void free(int tailIndex);

 private:
  const VID vid;

  int frameCount;
  std::list<std::unique_ptr<Frame>> frames;
  std::mutex mtx;
};

} // namespace md

#endif // FRAME_BUFFER_HPP_
