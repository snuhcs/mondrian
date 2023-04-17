#include "mondrian/model/TfLiteYoloV5Classifier.hpp"

#include <map>
#include <set>

#include "opencv2/imgproc.hpp"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"

#include "mondrian/Config.hpp"
#include "mondrian/Log.hpp"

namespace md {

TfLiteYoloV5Classifier::TfLiteYoloV5Classifier(std::string dataset, int inputSize,
                                               float confidenceThreshold, float iouThreshold,
                                               bool isTiny, bool forFullFrame)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 64) * (inputSize / 64) * 252,
                 confidenceThreshold, iouThreshold, GPU) {
  std::stringstream ss;

  if (forFullFrame) {
    ss << "/data/local/tmp/models/" << dataset << "-full-yolov5l-" << inputSize << "-fp16.tflite";
  } else {
    ss << "/data/local/tmp/models/" << dataset << "-pack-yolov5" << (isTiny ? "s-" : "m-") << inputSize << "-fp16.tflite";
    // Note: currently not using isTiny. It's just placeholder.
  }
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

cv::Mat TfLiteYoloV5Classifier::preprocess(const cv::Mat& rgbMat) {
  cv::Mat mat;
  const int& width = rgbMat.cols;
  const int& height = rgbMat.rows;
  if (width * inputSize.height > height * inputSize.width) {
    resizeWidth = inputSize.width;
    resizeHeight = height * inputSize.width / width;
    left = 0;
    right = 0;
    top = (inputSize.height - resizeHeight) / 2;
    bottom = (inputSize.height - resizeHeight) - top;
  } else {
    resizeHeight = inputSize.height;
    resizeWidth = width * inputSize.height / height;
    top = 0;
    bottom = 0;
    left = (inputSize.width - resizeWidth) / 2;
    right = (inputSize.width - resizeWidth) - left;
  }
  cv::resize(rgbMat, mat, cv::Size(resizeWidth, resizeHeight));
  cv::copyMakeBorder(mat, mat, top, bottom, left, right,
                     cv::BORDER_CONSTANT, cv::Scalar(114, 114, 114));
  mat.convertTo(mat, CV_32FC3, 1.f / 255);
  return mat;
}

void TfLiteYoloV5Classifier::inference(const cv::Mat& mat) {
  assert(mat.cols == inputSize.width && mat.rows == inputSize.height && mat.type() == CV_32FC3);
  std::memcpy((void*) input, (void*) mat.data, inputSize.area() * mat.elemSize());
  interpreter->Invoke();
}

const float* TfLiteYoloV5Classifier::getBox(const int i) const {
  return &outputs[i * 85];
}

float TfLiteYoloV5Classifier::getObjectConfidence(const int i) const {
  return outputs[i * 85 + 4];
}

const float* TfLiteYoloV5Classifier::getClassConfidences(const int i) const {
  return &outputs[i * 85 + 5];
}

Rect TfLiteYoloV5Classifier::reconstructBox(float x, float y, float w, float h,
                                            float imageWidth, float imageHeight) {
  x *= (float) inputSize.width;
  y *= (float) inputSize.height;
  w *= (float) inputSize.width;
  h *= (float) inputSize.height;
  float gain = std::min((float) inputSize.width / (float) imageWidth,
                        (float) inputSize.height / (float) imageHeight);
  float xPad = ((float) inputSize.width - (float) imageWidth * gain) / 2;
  float yPad = ((float) inputSize.height - (float) imageHeight * gain) / 2;
  return Rect(
      std::max(0.0f, ((x - w / 2 - xPad) / gain)),
      std::max(0.0f, ((y - h / 2 - yPad) / gain)),
      std::min(imageWidth, ((x + w / 2 - xPad) / gain)),
      std::min(imageHeight, ((y + h / 2 - yPad) / gain)));
}

void TfLiteYoloV5Classifier::singleInference() const {
  interpreter->Invoke();
}

} // namespace md
