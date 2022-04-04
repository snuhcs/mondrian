#include "CppInferenceEngine.hpp"

namespace rm {

CppInferenceEngine::CppInferenceEngine(int frameSize, int fullFrameSize) : mHandle(0) {
  LOGD("CppInferenceEngine::CppInferenceEngine()");
  workers.push_back(std::make_unique<Worker>(this, frameSize, fullFrameSize));
}

int CppInferenceEngine::enqueue(const cv::Mat mat, const bool isFull) {
  LOGD("CppInferenceEngine::enqueue(Mat(%d, %d, %d))", mat.cols, mat.rows, mat.channels());
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputs.push(std::make_tuple(mHandle, mat, isFull));
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

std::tuple<int, const cv::Mat, bool> CppInferenceEngine::getInput() {
  LOGD("CppInferenceEngine::getInput()");
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputCv.wait(inputLock, [this]() {
    return !inputs.empty();
  });
  std::tuple<int, const cv::Mat, bool> input = inputs.front();
  inputs.pop();
  LOGD("CppInferenceEngine::getInput(%d, Mat(%d, %d, %d))",
       std::get<0>(input), std::get<1>(input).cols, std::get<1>(input).rows,
       std::get<1>(input).channels());
  return input;
}

void CppInferenceEngine::enqueueResults(const int handle, const std::vector<BoundingBox>& boxes) {
  LOGD("CppInferenceEngine::enqueueResults(%d, %d)", handle, (int) boxes.size());
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.insert(std::make_pair(handle, boxes));
  resultLock.unlock();
  resultCv.notify_all();
}

Worker::Worker(CppInferenceEngine* engine, int frameSize, int fullFrameSize)
    : engine(engine), isClosed(false),
      classifier(new YoloV4Classifier(frameSize)),
      fullClassifier(new YoloV4Classifier(fullFrameSize)) {
  targetSize = cv::Size(classifier->getInputSize(), classifier->getInputSize());
  fullTargetSize = cv::Size(fullClassifier->getInputSize(), fullClassifier->getInputSize());
  LOGD("Worker::Worker()");
  thread = std::thread([this]() {
    while (!isClosed.load()) {
      Work();
    }
  });
}

void Worker::Work() {
  LOGD("Worker::Work()");
  std::tuple<int, const cv::Mat, bool> input = engine->getInput();
  int handle = std::get<0>(input);
  const cv::Mat mat = std::get<1>(input);
  bool isFull = std::get<2>(input);
  int originalWidth = mat.cols;
  int originalHeight = mat.rows;
  cv::Mat preprocessedMat = preprocess(mat, isFull ? fullTargetSize : targetSize);
  std::vector<BoundingBox> boxes = (isFull ? fullClassifier : classifier)->recognizeImage(
      preprocessedMat, originalWidth, originalHeight);
  engine->enqueueResults(handle, boxes);
}

cv::Mat Worker::preprocess(const cv::Mat mat, const cv::Size& size) {
  cv::Mat preprocessedMat = mat.clone();
  cv::cvtColor(preprocessedMat, preprocessedMat, CV_BGRA2RGB);
  cv::resize(preprocessedMat, preprocessedMat, size);
  preprocessedMat.convertTo(preprocessedMat, CV_32FC3, 1.f / 255);
  return preprocessedMat;
}

} // namespace rm
