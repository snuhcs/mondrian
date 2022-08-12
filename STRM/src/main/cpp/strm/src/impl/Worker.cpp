#include "strm/impl/Worker.hpp"

#include "strm/Log.hpp"

namespace rm {

Worker::Worker(CustomInferenceEngine* engine, std::map<int, Classifier*> classifierMap)
    : engine(engine), classifierMap(std::move(classifierMap)), isClosed(false) {
  thread = std::thread([this]() {
    while (!isClosed.load()) {
      Work();
    }
  });
}

void Worker::Work() {
  auto input = engine->getInput();
  int handle = std::get<0>(input);
  const cv::Mat mat = std::get<1>(input);
  const int resizeTarget = std::get<2>(input);
  assert(classifierMap.find(resizeTarget) != classifierMap.end());
  std::vector<BoundingBox> boxes = classifierMap[resizeTarget]->recognizeImage(mat);
  engine->drawInferenceResult(mat, boxes);
  engine->enqueueResults(handle, boxes);
}

long long Worker::getInferenceTimeMs(int inputSize) {
  assert(classifierMap.find(inputSize) != classifierMap.end());
  return classifierMap[inputSize]->getInferenceTimeMs();
}

} // namespace rm
