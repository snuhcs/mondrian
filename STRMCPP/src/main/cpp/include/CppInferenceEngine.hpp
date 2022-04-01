#ifndef CPP_INFERENCE_ENGINE_H
#define CPP_INFERENCE_ENGINE_H

#include <map>

#include "strm/InferenceEngine.hpp"
#include "YoloV4Classifier.hpp"

namespace rm {

class Worker;

class CppInferenceEngine : public InferenceEngine {
 public:
  CppInferenceEngine() : mHandle(0) {};

  int enqueue(const cv::Mat& mat, bool isFull) {
    std::unique_lock<std::mutex> inputLock(inputMtx);
    inputs.push(std::make_pair(mHandle, mat));
    inputCv.notify_all();
    return mHandle++;
  }

  std::pair<int, const cv::Mat&> getInput() {
    std::unique_lock<std::mutex> inputLock(inputMtx);
    inputCv.wait(inputLock, [this](){
      return !inputs.empty();
    });
    std::pair<int, const cv::Mat&> input = inputs.front();
    inputs.pop();
    return input;
  }

  void enqueueResults(const int handle, std::vector<BoundingBox> boxes) {
    std::unique_lock<std::mutex> resultLock(resultMtx);
    results.insert(std::make_pair(handle, boxes));
    resultCv.notify_all();
  }

  std::vector<BoundingBox> getResults(const int handle) {
    std::unique_lock<std::mutex> resultLock(resultMtx);
    resultCv.wait(resultLock, [this, handle](){
      return results.find(handle) != results.end();
    });
    return results.at(handle);
  }

 private:
  std::vector<std::unique_ptr<Worker>> workers;

  int mHandle;
  std::queue<std::pair<int, const cv::Mat&>> inputs;
  std::mutex inputMtx;
  std::condition_variable inputCv;
  std::map<int, std::vector<BoundingBox>> results;
  std::mutex resultMtx;
  std::condition_variable resultCv;
};

class Worker {
 public:
  Worker(CppInferenceEngine* engine) : engine(engine), isClosed(false) {
    thread = std::thread([this](){
      while (isClosed.load()) {
        Work();
      }
    });
  };

  ~Worker() {
    isClosed.store(true);
    thread.join();
  }

 private:
  void Work() {
    std::pair<int, const cv::Mat&> input = engine->getInput();
    std::vector<BoundingBox> boxes = classifier.recognizeImage(
        preprocess(input.second), input.second.cols, input.second.rows);
    engine->enqueueResults(input.first, boxes);
  };

  cv::Mat preprocess(const cv::Mat& input) {
    cv::Mat processedImage;
    input.convertTo(processedImage, CV_32FC3);
    cv::cvtColor(processedImage, processedImage, CV_BGRA2RGB);
    cv::resize(processedImage, processedImage, cv::Size(960, 960));
    cv::divide(1.0/255.0, processedImage, processedImage);
    return processedImage;
  }

  CppInferenceEngine* engine;

  YoloV4Classifier classifier;
  std::atomic_bool isClosed;
  std::thread thread;
};

}

#endif // CPP_INFERENCE_ENGINE_H
