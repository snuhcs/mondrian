#include "strm/impl/Worker.hpp"

#include "strm/Log.hpp"
#include "strm/InferenceEngine.hpp"

namespace rm {

Worker::Worker(InferenceEngine* engine, Device device, std::map<int, Classifier*> classifierMap)
    : engine(engine), device(device), classifierMap(std::move(classifierMap)), isClosed(false) {
  thread = std::thread([this]() { work(); });
}

Worker::~Worker() {
  isClosed.store(true);
  thread.join();
}

void Worker::work() {
  while (!isClosed.load()) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this](){ return !inputs.empty(); });
    auto[mat, size, key] = std::move(inputs.front());
    inputs.pop();
    lock.unlock();

    assert(classifierMap.find(size) != classifierMap.end());
    std::vector<BoundingBox> boxes = classifierMap[size]->recognizeImage(mat);
    engine->drawInferenceResult(mat, boxes);
    engine->enqueueResults(key, boxes);
  }
}

void Worker::enqueue(const cv::Mat& mat, int inputSize, int key) {
  std::unique_lock<std::mutex> lock(mtx);
  inputs.push({mat, inputSize, key});
  lock.unlock();
  cv.notify_one();
}

std::map<int, time_us> Worker::getInferenceTimes() {
  std::map<int, time_us> timeTable;
  for (auto& [inputSize, classifier] : classifierMap) {
    assert (classifier->getInferenceTime() > 0);
    timeTable[inputSize] = classifier->getInferenceTime();
  }
  return timeTable;
}

} // namespace rm
