#include "CppInferenceEngine.hpp"

namespace rm {

CppInferenceEngine::CppInferenceEngine() : mHandle(0) {
  LOGD("CppInferenceEngine::CppInferenceEngine()");
  workers.push_back(std::make_unique<Worker>(this));
}

int CppInferenceEngine::enqueue(const cv::Mat mat, const bool isFull) {
  LOGD("CppInferenceEngine::enqueue(Mat(%d, %d, %d))", mat.cols, mat.rows, mat.channels());
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputs.push(std::make_pair(mHandle, mat));
  inputLock.unlock();
  inputCv.notify_all();
  return mHandle++;
}

std::vector<BoundingBox> CppInferenceEngine::getResults(const int handle) {
  LOGD("CppInferenceEngine::getResults(%d)", handle);
  std::unique_lock<std::mutex> resultLock(resultMtx);
  resultCv.wait(resultLock, [this, handle]() {
    return results.find(handle) != results.end();
  });
  std::vector<BoundingBox> boxes = results.at(handle);
  results.erase(results.find(handle));
  LOGD("CppInferenceEngine::getResults(%d) end", handle);
  return boxes;
}

std::pair<int, const cv::Mat> CppInferenceEngine::getInput() {
  LOGD("CppInferenceEngine::getInput()");
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputCv.wait(inputLock, [this]() {
    return !inputs.empty();
  });
  std::pair<int, const cv::Mat> input = inputs.front();
  inputs.pop();
  LOGD("CppInferenceEngine::getInput(%d, Mat(%d, %d, %d))",
       input.first, input.second.cols, input.second.rows, input.second.channels());
  return input;
}

void CppInferenceEngine::enqueueResults(const int handle, const std::vector<BoundingBox>& boxes) {
  LOGD("CppInferenceEngine::enqueueResults(%d, %d)", handle, (int) boxes.size());
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.insert(std::make_pair(handle, boxes));
  resultLock.unlock();
  resultCv.notify_all();
}

Worker::Worker(CppInferenceEngine* engine)
: engine(engine), isClosed(false), classifier(new YoloV4Classifier()) {
  targetSize = cv::Size(classifier->getInputSize(), classifier->getInputSize());
  LOGD("Worker::Worker()");
  thread = std::thread([this]() {
    while (!isClosed.load()) {
      Work();
    }
  });
}

void Worker::Work() {
  LOGD("Worker::Work()");
  std::pair<int, const cv::Mat> input = engine->getInput();
  const cv::Mat mat = input.second;
  int originalWidth = mat.cols;
  int originalHeight = mat.rows;
  LOGV("1Mat(%d, %d, %d, %d)", mat.cols, mat.rows, mat.channels(), mat.type());
  cv::Mat preprocessedMat = preprocess(mat, targetSize);
  LOGV("2Mat(%d, %d, %d, %d)", mat.cols, mat.rows, mat.channels(), mat.type());
  std::vector<BoundingBox> boxes = classifier->recognizeImage(
      preprocessedMat, originalWidth, originalHeight);
  LOGV("3Mat(%d, %d, %d, %d)", preprocessedMat.cols, preprocessedMat.rows,
       preprocessedMat.channels(), preprocessedMat.type());
  LOGV("4Mat(%d, %d, %d, %d)", mat.cols, mat.rows, mat.channels(), mat.type());
  engine->enqueueResults(input.first, boxes);
}

cv::Mat Worker::preprocess(const cv::Mat mat, const cv::Size& size) {
  cv::Mat preprocessedMat = mat.clone();
  cv::cvtColor(preprocessedMat, preprocessedMat, CV_BGRA2RGB);
//  auto* udata = (uint8_t*) preprocessedMat.data;
//  LOGD("Worker::cvtColor : Mat(%d, %d, %d, %d), %d, %d, %d",
//       preprocessedMat.cols, preprocessedMat.rows, preprocessedMat.channels(), preprocessedMat.type(),
//       udata[0], udata[1], udata[2]);

  cv::resize(preprocessedMat, preprocessedMat, size);
//  udata = (uint8_t*) preprocessedMat.data;
//  LOGD("Worker::resize : Mat(%d, %d, %d, %d), %d, %d, %d",
//       preprocessedMat.cols, preprocessedMat.rows, preprocessedMat.channels(), preprocessedMat.type(),
//       udata[0], udata[1], udata[2]);

  preprocessedMat.convertTo(preprocessedMat, CV_32FC3, 1.f / 255);
//  auto* data = (float*) preprocessedMat.data;
//  LOGD("Worker::convertTo : Mat(%d, %d, %d, %d), %f, %f, %f",
//       preprocessedMat.cols, preprocessedMat.rows, preprocessedMat.channels(), preprocessedMat.type(),
//       data[0], data[1], data[2]);
  return preprocessedMat;
}

} // namespace rm
