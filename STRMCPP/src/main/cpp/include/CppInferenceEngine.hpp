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
  CppInferenceEngine(int frameSize, int fullFrameSize);

  int enqueue(const cv::Mat mat, const bool isFull) override;

  std::vector<BoundingBox> getResults(const int handle) override;

  std::tuple<int, const cv::Mat, bool> getInput();

  void enqueueResults(const int handle, const std::vector<BoundingBox>& boxes);

 private:
  std::vector<std::unique_ptr<Worker>> workers;

  int mHandle;
  std::queue<std::tuple<int, const cv::Mat, bool>> inputs;
  std::mutex inputMtx;
  std::condition_variable inputCv;
  std::map<int, std::vector<BoundingBox>> results;
  std::mutex resultMtx;
  std::condition_variable resultCv;
};

class Worker {
 public:
  Worker(CppInferenceEngine* engine, int frameSize, int fullFrameSize);

  ~Worker() {
    isClosed.store(true);
    thread.join();
  }

 private:
  void Work();

  static cv::Mat preprocess(const cv::Mat mat, const cv::Size& size);

  CppInferenceEngine* engine;

  std::unique_ptr<YoloV4Classifier> classifier;
  std::unique_ptr<YoloV4Classifier> fullClassifier;
  std::atomic_bool isClosed;
  std::thread thread;
  cv::Size targetSize;
  cv::Size fullTargetSize;
};

} // namespace rm

#endif // CPP_INFERENCE_ENGINE_H
