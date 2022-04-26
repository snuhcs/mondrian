#include "strm/impl/Worker.hpp"

#include "strm/Log.hpp"

namespace rm {

Worker::Worker(CustomInferenceEngine* engine,
               std::unique_ptr<YoloV4Classifier> cls,
               std::unique_ptr<YoloV4Classifier> fullCls)
    : engine(engine), classifier(std::move(cls)),
      fullClassifier(std::move(fullCls)), isClosed(false) {
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

long long Worker::getInferenceTimeMS(){
  return classifier->getInferenceTimeMS();
}

} // namespace rm
