#include "strm/impl/CustomInferenceEngine.hpp"

#include "strm/Log.hpp"
#include "strm/impl/Worker.hpp"

namespace rm {

CustomInferenceEngine::CustomInferenceEngine(const InferenceEngineConfig& config) : mHandle(0) {
  LOGD("CppInferenceEngine::CppInferenceEngine()");
  for (int i = 0; i < config.NUM_WORKERS; i++) {
    workers.push_back(std::make_unique<Worker>(
        this, std::make_unique<MnnYoloV4Classifier>(config.INPUT_SIZE, config.CONF_THRESHOLD,
                                                    config.IOU_THRESHOLD, config.USE_TINY),
        std::make_unique<MnnYoloV4Classifier>(config.FULL_FRAME_INPUT_SIZE, config.CONF_THRESHOLD,
                                              config.IOU_THRESHOLD, config.USE_TINY)));
  }
}

int CustomInferenceEngine::enqueue(const cv::Mat mat, const bool isFull) {
  LOGD("CppInferenceEngine::enqueue(Mat(%d, %d, %d))", mat.cols, mat.rows, mat.channels());
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputs.push(std::make_tuple(mHandle, mat, isFull));
  inputLock.unlock();
  inputCv.notify_all();
  return mHandle++;
}

std::vector<BoundingBox> CustomInferenceEngine::getResults(const int handle) {
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

std::tuple<int, const cv::Mat, bool> CustomInferenceEngine::getInput() {
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

void
CustomInferenceEngine::enqueueResults(const int handle, const std::vector<BoundingBox>& boxes) {
  LOGD("CppInferenceEngine::enqueueResults(%d, %d)", handle, (int) boxes.size());
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.insert(std::make_pair(handle, boxes));
  resultLock.unlock();
  resultCv.notify_all();
}

long long CustomInferenceEngine::getInferenceTimeMs() {
  long long inferenceTime = 0;
  int cnt = 0;
  for (auto& worker : workers) {
    long long t_inf_worker = worker->getInferenceTimeMs();
    if (t_inf_worker > 0) {
      cnt++;
      inferenceTime += t_inf_worker;
    }
  }
  inferenceTime /= cnt;
  return inferenceTime;
}

} // namespace rm
