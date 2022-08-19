#ifndef IMPL_WORKER_HPP_
#define IMPL_WORKER_HPP_

#include <queue>
#include <thread>

#include "strm/impl/models/Classifier.hpp"

namespace rm {

class InferenceEngine;

class Worker {
 public:
  Worker(InferenceEngine* engine, Device device, std::map<int, Classifier*> classifierMap);

  ~Worker();

  void enqueue(const cv::Mat& mat, int inputSize, int key);

  std::map<int, time_us> getInferenceTimes();

 private:
  void work();

  const Device device;

  std::mutex mtx;
  std::condition_variable cv;
  std::queue<std::tuple<const cv::Mat, int, int>> inputs;

  InferenceEngine* engine;
  std::map<int, Classifier*> classifierMap;

  std::atomic_bool isClosed;
  std::thread thread;
};

} // namespace rm

#endif // IMPL_WORKER_HPP_
