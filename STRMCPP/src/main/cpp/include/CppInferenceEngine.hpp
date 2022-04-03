#ifndef CPP_INFERENCE_ENGINE_H
#define CPP_INFERENCE_ENGINE_H

#include <map>
#include <queue>
#include <thread>

#include "strm/Log.hpp"
#include "strm/InferenceEngine.hpp"
#include "YoloV4Classifier.hpp"

namespace rm {

class Worker;

class CppInferenceEngine : public InferenceEngine {
 public:
  CppInferenceEngine();

  void hi() {
    LOGD("hi");
  }

  int enqueue(const cv::Mat mat, bool isFull) override;

  std::vector<BoundingBox> getResults(const int handle) override;

  std::pair<int, cv::Mat> getInput();

  void enqueueResults(const int handle, const std::vector<BoundingBox>& boxes);

 private:
  std::vector<std::unique_ptr<Worker>> workers;

  int mHandle;
  std::queue<std::pair<int, cv::Mat>> inputs;
  std::mutex inputMtx;
  std::condition_variable inputCv;
  std::map<int, std::vector<BoundingBox>> results;
  std::mutex resultMtx;
  std::condition_variable resultCv;
};

class Worker {
 public:
  Worker(CppInferenceEngine* engine);

  ~Worker() {
    isClosed.store(true);
    thread.join();
  }

 private:
  void Work();

  static void preprocess(cv::Mat& mat);

  CppInferenceEngine* engine;

  std::unique_ptr<YoloV4Classifier> classifier;
  std::atomic_bool isClosed;
  std::thread thread;
};

}

#endif // CPP_INFERENCE_ENGINE_H
