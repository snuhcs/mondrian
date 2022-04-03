#include "CppInferenceEngine.hpp"

namespace rm {

CppInferenceEngine::CppInferenceEngine() : mHandle(0) {
  workers.push_back(std::make_unique<Worker>(this));
}

int CppInferenceEngine::enqueue(const cv::Mat mat, bool isFull) {
  LOGD("CPP enqueue(Mat(%d, %d, %d))", mat.cols, mat.rows, mat.channels());
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputs.push(std::make_pair(mHandle, mat.clone()));
  inputLock.unlock();
  inputCv.notify_all();
  return mHandle++;
}

std::vector<BoundingBox> CppInferenceEngine::getResults(const int handle) {
  LOGD("CppInferenceEngine::getResults");
  std::unique_lock<std::mutex> resultLock(resultMtx);
  resultCv.wait(resultLock, [this, handle]() {
    return results.find(handle) != results.end();
  });
  std::vector<BoundingBox> boxes = results.at(handle);
  results.erase(results.find(handle));
  return boxes;
}

std::pair<int, cv::Mat> CppInferenceEngine::getInput() {
  LOGD("CppInferenceEngine::getInput");
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputCv.wait(inputLock, [this]() {
    return !inputs.empty();
  });
  std::pair<int, cv::Mat> input = inputs.front();
  inputs.pop();
  return input;
}

void CppInferenceEngine::enqueueResults(const int handle, const std::vector<BoundingBox>& boxes) {
  LOGD("CppInferenceEngine::enqueueResults");
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.insert(std::make_pair(handle, boxes));
  resultLock.unlock();
  resultCv.notify_all();
}

Worker::Worker(CppInferenceEngine* engine) : engine(engine), isClosed(false),
                                             classifier(new YoloV4Classifier()) {
  LOGD("Worker::Worker()");
  thread = std::thread([this]() {
    while (!isClosed.load()) {
      Work();
    }
  });
}

void Worker::Work() {
  LOGD("Worker::Work()");
  std::pair<int, cv::Mat> input = engine->getInput();
  cv::Mat& mat = input.second;
  auto* data = (uint8_t*) mat.data;
  LOGD("Worker: Mat(%d, %d, %d, %d), %d, %d, %d",
       mat.cols, mat.rows, mat.channels(), mat.type(),
       data[0], data[1], data[2]);
  preprocess(mat);
  std::vector<BoundingBox> boxes = classifier->recognizeImage(
      mat, input.second.cols, input.second.rows);
  engine->enqueueResults(input.first, boxes);
}

void Worker::preprocess(cv::Mat& mat) {
  cv::cvtColor(mat, mat, CV_BGRA2RGB);
  auto* udata = (uint8_t*) mat.data;
  LOGD("Worker::cvtColor : Mat(%d, %d, %d, %d), %d, %d, %d",
       mat.cols, mat.rows, mat.channels(), mat.type(),
       udata[0], udata[1], udata[2]);

  mat.convertTo(mat, CV_32FC3, 1.f / 255);
  auto* data = (float*) mat.data;
  LOGD("Worker::convertTo : Mat(%d, %d, %d, %d), %f, %f, %f",
       mat.cols, mat.rows, mat.channels(), mat.type(),
       data[0], data[1], data[2]);

  cv::resize(mat, mat, cv::Size(960, 960));
  data = (float*) mat.data;
  LOGD("Worker::resize : Mat(%d, %d, %d, %d), %f, %f, %f",
       mat.cols, mat.rows, mat.channels(), mat.type(),
       data[0], data[1], data[2]);
}

} // namespace rm
