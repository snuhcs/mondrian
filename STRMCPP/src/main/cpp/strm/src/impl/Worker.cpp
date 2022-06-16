#include "strm/impl/Worker.hpp"

#include "strm/Log.hpp"

namespace rm {

Worker::Worker(CustomInferenceEngine* engine, Classifier* cls)
    : engine(engine), classifier(cls), isClosed(false) {
  LOGD("Worker::Worker()");
  thread = std::thread([this]() {
    while (!isClosed.load()) {
      Work();
    }
  });
}

void Worker::Work() {
  LOGD("Worker::work()");
  std::tuple<int, const cv::Mat> input = engine->getInput();
  int handle = std::get<0>(input);
  const cv::Mat mat = std::get<1>(input);
  std::vector<BoundingBox> boxes = classifier->recognizeImage(mat);
  engine->drawInferenceResult(mat, boxes);
  engine->enqueueResults(handle, boxes);
}

long long Worker::getInferenceTimeMs() {
  return classifier->getInferenceTimeMs();
}

} // namespace rm
