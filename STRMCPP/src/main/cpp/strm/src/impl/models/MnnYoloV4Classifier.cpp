#include "strm/impl/models/MnnYoloV4Classifier.hpp"

#include <chrono>
#include <map>
#include <set>

#include "strm/Config.hpp"
#include "strm/Log.hpp"

namespace rm {

MnnYoloV4Classifier::MnnYoloV4Classifier(int inputSize, float confidenceThreshold,
                                         float iouThreshold, bool isTiny)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 32) * (inputSize / 32) * (isTiny ? 15 : 63),
                 confidenceThreshold, iouThreshold) {
  LOGD("YoloV4 MnnYoloV4Classifier::MnnYoloV4Classifier()");
  std::string filepath = "/data/local/tmp/models/yolov4-";
  filepath += (isTiny ? "tiny-" : "") + std::to_string(inputSize) + "-fp16.mnn";
  interpreter = MNN::Interpreter::createFromFile(filepath.c_str());
  if (interpreter == nullptr) {
    LOGE("YoloV4 interpreter creation failed");
    return;
  } else {
    LOGD("YoloV4 interpreter created");
  }

  MNN::ScheduleConfig conf;
//  conf.type = MNN_FORWARD_OPENCL; // OpenCL Backend
  session = interpreter->createSession(conf);
  if (session == nullptr) {
    LOGE("YoloV4 session creation failed");
    return;
  } else {
    LOGD("YoloV4 session created");
  }

  std::map<std::string, MNN::Tensor*> inputTensorMap = interpreter->getSessionInputAll(session);
  std::map<std::string, MNN::Tensor*> outputTensorMap = interpreter->getSessionOutputAll(session);
  auto getShapeString = [](const std::vector<int>& shape) {
    std::string shapeString;
    for (int i = 0; i < shape.size(); i++) {
      shapeString += std::to_string(shape[i]);
      if (i != shape.size() - 1) {
        shapeString += ", ";
      }
    }
    return shapeString;
  };
  std::string inputInfo;
  for (auto& it : inputTensorMap) {
    inputInfo += it.first + ": " + getShapeString(it.second->shape());
    inputInfo += " | ";
  }
  LOGD("YoloV4 inputs  : %s", inputInfo.c_str());

  std::string outputInfo;
  for (auto& it : outputTensorMap) {
    outputInfo += it.first + ": " + getShapeString(it.second->shape());
    outputInfo += " | ";
  }
  LOGD("YoloV4 outputs : %s", outputInfo.c_str());
}

MnnYoloV4Classifier::~MnnYoloV4Classifier() = default;

void MnnYoloV4Classifier::inference(const cv::Mat& mat) {
  MNN::Tensor* inputTensor = interpreter->getSessionInputAll(session).at(INPUT_TENSOR_NAME);
  MNN::Tensor* outputBoxes = interpreter->getSessionOutputAll(session).at(OUTPUT_TENSOR_NAME_BOXES);
  MNN::Tensor* outputConfs = interpreter->getSessionOutputAll(session).at(OUTPUT_TENSOR_NAME_CONFS);

  assert(inputTensor->size() == mat.total() * mat.elemSize());
  std::memcpy((void*) inputTensor->host<float>(), (void*) mat.data, inputTensor->size());

  auto start = std::chrono::system_clock::now();
  interpreter->runSession(session);
  auto end = std::chrono::system_clock::now();
  inferenceTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  LOGV("YoloV4 Inference %lld ms", inferenceTimeMs);

  boxes = outputBoxes->host<float>();
  confidences = outputConfs->host<float>();
}

const float* MnnYoloV4Classifier::getBoxes(const int i) const {
  return &boxes[i * 4];
}

const float* MnnYoloV4Classifier::getConfidences(const int i) const {
  return &confidences[i * numLabels];
}

} // namespace rm
