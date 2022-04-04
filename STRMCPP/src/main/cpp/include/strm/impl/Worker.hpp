#ifndef IMPL_WORKER_HPP_
#define IMPL_WORKER_HPP_

#include <thread>

#include "strm/impl/CustomInferenceEngine.hpp"
#include "strm/impl/YoloV4Classifier.hpp"

namespace rm {

class Worker {
 public:
  Worker(CustomInferenceEngine* engine, int frameSize, int fullFrameSize);

  ~Worker() {
    isClosed.store(true);
    thread.join();
  }

 private:
  void Work();

  static cv::Mat preprocess(const cv::Mat mat, const cv::Size& size);

  CustomInferenceEngine* engine;

  std::unique_ptr<YoloV4Classifier> classifier;
  std::unique_ptr<YoloV4Classifier> fullClassifier;
  std::atomic_bool isClosed;
  std::thread thread;
  cv::Size targetSize;
  cv::Size fullTargetSize;
};

} // namespace rm

#endif // IMPL_WORKER_HPP_
