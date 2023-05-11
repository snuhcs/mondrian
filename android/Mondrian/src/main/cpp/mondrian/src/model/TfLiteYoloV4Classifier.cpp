#include "mondrian/model/TfLiteYoloV4Classifier.hpp"

#include <map>
#include <set>

#include "opencv2/opencv.hpp"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"

#include "mondrian/Config.hpp"
#include "mondrian/Log.hpp"

namespace md {

TfLiteYoloV4Classifier::TfLiteYoloV4Classifier(std::string dataset, int inputSize,
                                               float confidenceThreshold, float iouThreshold,
                                               bool isTiny, bool forFullFrame)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 32) * (inputSize / 32) * 63,
                 confidenceThreshold, iouThreshold) {

  // TODO : use forFullFrame
  std::stringstream ss;
  ss << "/data/local/tmp/models/yolov4-";
  if (isTiny) {
    ss << "tiny-";
  }
  ss << inputSize << "-fp16.tflite";
  auto model = tflite::FlatBufferModel::BuildFromFile(ss.str().c_str());
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

  const std::vector<int>& inputTensorIndices = interpreter->inputs();
  const std::vector<int>& outputTensorIndices = interpreter->outputs();
  assert(inputTensorIndices.size() == 1 && outputTensorIndices.size() == 2);

  auto* inputTensorDims = interpreter->tensor(inputTensorIndices[0])->dims;
  assert(inputTensorDims->size == 4 && inputTensorDims->data[0] == 1 &&
         inputTensorDims->data[1] == inputSize && inputTensorDims->data[2] == inputSize &&
         inputTensorDims->data[3] == 3);

  bool isBoxesFirst = interpreter->tensor(outputTensorIndices[0])->bytes <
                      interpreter->tensor(outputTensorIndices[1])->bytes;
  int boxesIndex = isBoxesFirst ? outputTensorIndices[0] : outputTensorIndices[1];
  int confidencesIndex = isBoxesFirst ? outputTensorIndices[1] : outputTensorIndices[0];
  auto* boxesTensorDims = interpreter->tensor(boxesIndex)->dims;
  assert(boxesTensorDims->size == 3 && boxesTensorDims->data[0] == 1 &&
         boxesTensorDims->data[1] == outputSize && boxesTensorDims->data[2] == 4);
  auto* confidencesTensorDims = interpreter->tensor(confidencesIndex)->dims;
  assert(confidencesTensorDims->size == 3 && confidencesTensorDims->data[0] == 1 &&
         confidencesTensorDims->data[1] == outputSize &&
         confidencesTensorDims->data[2] == numLabels);

  input = interpreter->typed_tensor<float>(inputTensorIndices[0]);
  boxes = interpreter->typed_tensor<float>(boxesIndex);
  confidences = interpreter->typed_tensor<float>(confidencesIndex);
}

TfLiteYoloV4Classifier::~TfLiteYoloV4Classifier() {
  TfLiteGpuDelegateV2Delete(delegate);
}

cv::Mat TfLiteYoloV4Classifier::preprocess(const cv::Mat& rgbMat) {
  cv::Mat mat;
  if (rgbMat.cols != inputSize.width || rgbMat.rows != inputSize.height) {
    cv::resize(rgbMat, mat, inputSize);
  }
  mat.convertTo(mat, CV_32FC3, 1.f / 255);
  return mat;
}

void TfLiteYoloV4Classifier::inference(const cv::Mat& mat) {
  assert(mat.cols == inputSize.width && mat.rows == inputSize.height && mat.type() == CV_32FC3);
  std::memcpy((void*) input, (void*) mat.data, inputSize.area() * mat.elemSize());
  interpreter->Invoke();
}

const float* TfLiteYoloV4Classifier::getBox(const int i) const {
  return &boxes[i * 4];
}

float TfLiteYoloV4Classifier::getObjectConfidence(const int i) const {
  return 1.0;
}

const float* TfLiteYoloV4Classifier::getClassConfidences(const int i) const {
  return &confidences[i * numLabels];
}

Rect TfLiteYoloV4Classifier::reconstructBox(float x, float y, float w, float h,
                                            float imageWidth, float imageHeight) const {
  float widthRatio = imageWidth / (float) inputSize.width;
  float heightRatio = imageHeight / (float) inputSize.height;
  return Rect(
      std::max(0.0f, ((x - w / 2) * widthRatio)),
      std::max(0.0f, ((y - h / 2) * heightRatio)),
      std::min(imageWidth, ((x + w / 2) * widthRatio)),
      std::min(imageHeight,((y + h / 2) * heightRatio)));
}

} // namespace md
