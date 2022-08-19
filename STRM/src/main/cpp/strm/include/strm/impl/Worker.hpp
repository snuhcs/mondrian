#ifndef IMPL_WORKER_HPP_
#define IMPL_WORKER_HPP_

#include <thread>

#include "strm/impl/models/Classifier.hpp"

namespace rm {

class InferenceEngine;

class Worker {
 public:
  Worker(InferenceEngine* engine, std::map<int, Classifier*> classifierMap);

  ~Worker() {
    isClosed.store(true);
    thread.join();
  }

  std::map<int, time_us> getInferenceTimeUs();

 private:
  void Work();

  InferenceEngine* engine;
  std::map<int, Classifier*> classifierMap;

  std::atomic_bool isClosed;
  std::thread thread;
};

} // namespace rm

#endif // IMPL_WORKER_HPP_
