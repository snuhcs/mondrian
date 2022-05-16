#include "strm/impl/models/TfLiteYoloV4Classifier.hpp"

#include <chrono>
#include <map>
#include <set>

#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"

#include "strm/Config.hpp"
#include "strm/Log.hpp"

namespace rm {

TfLiteYoloV4Classifier::TfLiteYoloV4Classifier(int inputSize, float confidenceThreshold,
                                               float iouThreshold, bool isTiny)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 32) * (inputSize / 32) * 63,
                 confidenceThreshold, iouThreshold) {
  LOGD("YoloV4 TfLiteYoloV4Classifier::TfLiteYoloV4Classifier()");
  std::string filepath = "/data/local/tmp/models/yolov4-";
  filepath += (isTiny ? "tiny-" : "") + std::to_string(inputSize) + "-fp16.tflite";
  auto model = tflite::FlatBufferModel::BuildFromFile(filepath.c_str());
  if (model == nullptr) {
    LOGE("YoloV4 model load failed");
  } else {
    LOGD("YoloV4 model loaded");
  }

  tflite::ops::builtin::BuiltinOpResolver resolver;
  if (tflite::InterpreterBuilder(*model, resolver)(&interpreter) != kTfLiteOk) {
    LOGE("YoloV4 interpreter creation failed");
  } else {
    LOGD("YoloV4 interpreter created");
  }

//  // For CPU (XNNPack)
//  if (interpreter->AllocateTensors() != kTfLiteOk) {
//    LOGE("YoloV4 tensor allocation failed");
//  } else {
//    LOGD("YoloV4 tensor allocated");
//  }

  auto options = TfLiteGpuDelegateOptionsV2Default();
  delegate = TfLiteGpuDelegateV2Create(&options);
  if (delegate == nullptr) {
    LOGE("GPU delegate creation failed");
  } else {
    LOGD("GPU delegate created");
  }

  if (interpreter->ModifyGraphWithDelegate(delegate) != kTfLiteOk) {
    LOGE("YoloV4 gpu delegate application failed");
  } else {
    LOGD("YoloV4 gpu delegate applied");
  }
}

TfLiteYoloV4Classifier::~TfLiteYoloV4Classifier() {
  TfLiteGpuDelegateV2Delete(delegate);
}

std::pair<float*, float*> TfLiteYoloV4Classifier::inference(const cv::Mat& mat) {
  const std::vector<int>& inputs = interpreter->inputs();
  const std::vector<int>& outputs = interpreter->outputs();
  assert(inputs.size() == 1 && outputs.size() == 2);

  const size_t input_size = interpreter->tensor(inputs[0])->bytes;
  auto* input = interpreter->typed_tensor<float>(inputs[0]);
  assert(input_size == mat.total() * mat.elemSize());
  std::memcpy((void*) input, (void*) mat.data, input_size);

  auto start = std::chrono::system_clock::now();
  interpreter->Invoke();
  auto end = std::chrono::system_clock::now();
  inferenceTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  LOGV("YoloV4 Inference %lld ms", inferenceTimeMs);

  bool is_bbox_first =
      interpreter->tensor(outputs[0])->bytes < interpreter->tensor(outputs[1])->bytes;
  auto* bboxes = interpreter->typed_tensor<float>(outputs[is_bbox_first ? 0 : 1]);
  auto* confidences = interpreter->typed_tensor<float>(outputs[is_bbox_first ? 1 : 0]);
  return std::make_pair(bboxes, confidences);
}

} // namespace rm
