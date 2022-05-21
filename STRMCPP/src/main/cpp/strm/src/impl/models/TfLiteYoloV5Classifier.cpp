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
}

TfLiteYoloV5Classifier::~TfLiteYoloV5Classifier() {
  TfLiteGpuDelegateV2Delete(delegate);
}

void TfLiteYoloV5Classifier::inference(const cv::Mat& mat) {
  const std::vector<int>& input_tensor_indices = interpreter->inputs();
  const std::vector<int>& output_tensor_indices = interpreter->outputs();
  assert(input_tensor_indices.size() == 1 && output_tensor_indices.size() == 1);

  const size_t input_size = interpreter->tensor(input_tensor_indices[0])->bytes;
  auto* input = interpreter->typed_tensor<float>(input_tensor_indices[0]);
  assert(input_size == mat.total() * mat.elemSize());
  std::memcpy((void*) input, (void*) mat.data, input_size);

  auto start = std::chrono::system_clock::now();
  interpreter->Invoke();
  auto end = std::chrono::system_clock::now();
  inferenceTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  LOGV("YoloV5 Inference %lld ms", inferenceTimeMs);

  outputs = interpreter->typed_tensor<float>(output_tensor_indices[0]);
}

const float* TfLiteYoloV5Classifier::getBoxes(const int i) const {
  return &outputs[i * 85];
}

const float* TfLiteYoloV5Classifier::getConfidences(const int i) const {
  return &outputs[i * 85 + 5];
}

} // namespace rm
