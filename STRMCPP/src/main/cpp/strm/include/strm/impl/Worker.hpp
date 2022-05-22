#ifndef IMPL_WORKER_HPP_
#define IMPL_WORKER_HPP_

#include <thread>

#include "strm/impl/CustomInferenceEngine.hpp"
#include "strm/impl/models/Classifier.hpp"

namespace rm {

class Worker {
 public:
  Worker(CustomInferenceEngine* engine, Classifier* classifier, Classifier* fullClassifier);

  ~Worker() {
    isClosed.store(true);
    thread.join();
  }

  long long getInferenceTimeMs();

 private:
  void Work();

  CustomInferenceEngine* engine;
  Classifier* classifier;
  Classifier* fullClassifier;

  std::atomic_bool isClosed;
  std::thread thread;
};

} // namespace rm

#endif // IMPL_WORKER_HPP_
