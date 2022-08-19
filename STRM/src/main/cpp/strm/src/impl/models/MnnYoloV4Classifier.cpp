#include "strm/impl/models/MnnYoloV4Classifier.hpp"

#include <map>
#include <set>

#include "strm/Config.hpp"
#include "strm/Log.hpp"

namespace rm {

MnnYoloV4Classifier::MnnYoloV4Classifier(int inputSize, float confidenceThreshold,
                                         float iouThreshold, bool isTiny)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 32) * (inputSize / 32) * (isTiny ? 15 : 63),
                 confidenceThreshold, iouThreshold, GPU) {
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

  MNN::Tensor* inputTensor = interpreter->getSessionInputAll(session).at(INPUT_TENSOR_NAME);
  MNN::Tensor* boxesTensor = interpreter->getSessionOutputAll(session).at(OUTPUT_TENSOR_NAME_BOXES);
  MNN::Tensor* confidencesTensor = interpreter->getSessionOutputAll(session).at(
      OUTPUT_TENSOR_NAME_CONFS);
  const std::vector<int>& inputTensorShape = inputTensor->shape();
  assert(inputTensorShape.size() == 4 && inputTensorShape[0] == 1 &&
         inputTensorShape[1] == inputSize && inputTensorShape[2] == inputSize &&
         inputTensorShape[3] == 3);
  const std::vector<int>& boxesTensorShape = boxesTensor->shape();
  assert(boxesTensorShape.size() == 3 && boxesTensorShape[0] == 1 &&
         boxesTensorShape[1] == outputSize && boxesTensorShape[2] == 4);
  const std::vector<int>& confidencesTensorShape = confidencesTensor->shape();
  assert(confidencesTensorShape.size() == 3 && confidencesTensorShape[0] == 1 &&
         confidencesTensorShape[1] == outputSize && confidencesTensorShape[2] == numLabels);

  input = inputTensor->host<float>();
  boxes = boxesTensor->host<float>();
  confidences = confidencesTensor->host<float>();
}

MnnYoloV4Classifier::~MnnYoloV4Classifier() = default;

cv::Mat MnnYoloV4Classifier::preprocess(const cv::Mat& mat) {
  cv::Mat preprocessedMat;
  if (mat.cols != inputSize.width || mat.rows != inputSize.height) {
    cv::resize(mat, preprocessedMat, inputSize);
    cv::cvtColor(preprocessedMat, preprocessedMat, CV_BGRA2RGB);
  } else {
    cv::cvtColor(mat, preprocessedMat, CV_BGRA2RGB);
  }
  preprocessedMat.convertTo(preprocessedMat, CV_32FC3, 1.f / 255);
  return preprocessedMat;
}

void MnnYoloV4Classifier::inference(const cv::Mat& mat) {
  assert(mat.cols == inputSize.width && mat.rows == inputSize.height && mat.type() == CV_32FC3);
  std::memcpy((void*) input, (void*) mat.data, inputSize.area() * mat.elemSize());
  interpreter->runSession(session);
}

const float* MnnYoloV4Classifier::getBox(const int i) const {
  return &boxes[i * 4];
}

float MnnYoloV4Classifier::getObjectConfidence(const int i) const {
  return 1.0;
}

const float* MnnYoloV4Classifier::getClassConfidences(const int i) const {
  return &confidences[i * numLabels];
}

Rect MnnYoloV4Classifier::reconstructBox(float x, float y, float w, float h,
                                         float imageWidth, float imageHeight) {
  float widthRatio = (float) imageWidth / (float) inputSize.width;
  float heightRatio = (float) imageHeight / (float) inputSize.height;
  return Rect(
      std::max(0.0f, ((x - w / 2) * widthRatio)),
      std::max(0.0f, ((y - h / 2) * heightRatio)),
      std::min(imageWidth, ((x + w / 2) * widthRatio)),
      std::min(imageHeight, ((y + h / 2) * heightRatio)));
}

time_us MnnYoloV4Classifier::profileInferenceTime() {
  // Warmup
  interpreter->runSession(session);
  interpreter->runSession(session);

  auto start = std::chrono::system_clock::now();
  interpreter->runSession(session);
  auto end = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

} // namespace rm
