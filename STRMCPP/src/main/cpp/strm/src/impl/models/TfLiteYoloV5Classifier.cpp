#include "strm/impl/models/TfLiteYoloV5Classifier.hpp"

#include <chrono>
#include <map>
#include <set>

#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"

#include "strm/Config.hpp"
#include "strm/Log.hpp"

namespace rm {

TfLiteYoloV5Classifier::TfLiteYoloV5Classifier(int inputSize, float confidenceThreshold,
                                               float iouThreshold, bool isTiny)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 64) * (inputSize / 64) * 252,
                 confidenceThreshold, iouThreshold) {
  LOGD("YoloV5 TfLiteYoloV5Classifier::TfLiteYoloV5Classifier()");
  std::stringstream ss;
  ss << "/data/local/tmp/models/yolov5" << (isTiny ? "s-" : "m-") << inputSize << "-fp16.tflite";
  auto model = tflite::FlatBufferModel::BuildFromFile(ss.str().c_str());
  if (model == nullptr) {
    LOGE("YoloV5 model load failed");
  } else {
    LOGD("YoloV5 model loaded");
  }

  tflite::ops::builtin::BuiltinOpResolver resolver;
  if (tflite::InterpreterBuilder(*model, resolver)(&interpreter) != kTfLiteOk) {
    LOGE("YoloV5 interpreter creation failed");
  } else {
    LOGD("YoloV5 interpreter created");
  }

//  // For CPU (XNNPack)
//  if (interpreter->AllocateTensors() != kTfLiteOk) {
//    LOGE("YoloV5 tensor allocation failed");
//  } else {
//    LOGD("YoloV5 tensor allocated");
//  }

  auto options = TfLiteGpuDelegateOptionsV2Default();
  delegate = TfLiteGpuDelegateV2Create(&options);
  if (delegate == nullptr) {
    LOGE("GPU delegate creation failed");
  } else {
    LOGD("GPU delegate created");
  }

  if (interpreter->ModifyGraphWithDelegate(delegate) != kTfLiteOk) {
    LOGE("YoloV5 gpu delegate application failed");
  } else {
    LOGD("YoloV5 gpu delegate applied");
  }

  const std::vector<int>& inputTensorIndices = interpreter->inputs();
  const std::vector<int>& outputTensorIndices = interpreter->outputs();
  assert(inputTensorIndices.size() == 1 && outputTensorIndices.size() == 1);

  auto* inputTensorDims = interpreter->tensor(inputTensorIndices[0])->dims;
  assert(inputTensorDims->size == 4 && inputTensorDims->data[0] == 1 &&
         inputTensorDims->data[1] == inputSize && inputTensorDims->data[2] == inputSize &&
         inputTensorDims->data[3] == 3);
  auto* outputTensorDims = interpreter->tensor(outputTensorIndices[0])->dims;
  assert(outputTensorDims->size == 3 && outputTensorDims->data[0] == 1 &&
         outputTensorDims->data[1] == outputSize && outputTensorDims->data[2] == 85);

  input = interpreter->typed_tensor<float>(inputTensorIndices[0]);
  outputs = interpreter->typed_tensor<float>(outputTensorIndices[0]);
}

TfLiteYoloV5Classifier::~TfLiteYoloV5Classifier() {
  TfLiteGpuDelegateV2Delete(delegate);
}

void TfLiteYoloV5Classifier::inference(const cv::Mat& mat) {
  assert(mat.rows == inputSize && mat.cols == inputSize && mat.type() == CV_32FC3);
  std::memcpy((void*) input, (void*) mat.data, inputSize * inputSize * mat.elemSize());

  auto start = std::chrono::system_clock::now();
  interpreter->Invoke();
  auto end = std::chrono::system_clock::now();
  inferenceTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  LOGV("YoloV5 Inference %lld ms", inferenceTimeMs);
}

const float* TfLiteYoloV5Classifier::getBoxes(const int i) const {
  return &outputs[i * 85];
}

const float* TfLiteYoloV5Classifier::getConfidences(const int i) const {
  return &outputs[i * 85 + 5];
}

std::pair<float, float> TfLiteYoloV5Classifier::getReconstructRatios(
    const int originalWidth, const int originalHeight) {
  return std::make_pair((float) originalWidth, (float) originalHeight);
}

} // namespace rm
