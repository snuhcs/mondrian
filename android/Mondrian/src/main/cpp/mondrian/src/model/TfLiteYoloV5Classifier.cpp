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
                                               float confThres, float iouThres,
                                               bool isTiny, bool forFullFrame)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 32) * (inputSize / 32) * 63,
                 confThres, iouThres) {
  std::stringstream ss;
  ss << "/data/local/tmp/models/";

  if (dataset != "pretrained") {
    ss << dataset << "-";
    if (forFullFrame) {
      ss << "full-";
    } else {
      ss << "pack-";
    }
  }

  // Note: currently not using isTiny. It's just placeholder.
  ss << "yolov5" << (forFullFrame ? "l" : (isTiny ? "s" : "m")) << "-";
  ss << inputSize << "-fp16.tflite";

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
  assert(inputTensorDims->size == 4 && inputTensorDims->data[0] == 1
             && inputTensorDims->data[1] == inputSize && inputTensorDims->data[2] == inputSize
             && inputTensorDims->data[3] == 3);
  auto* outputTensorDims = interpreter->tensor(outputTensorIndices[0])->dims;
  assert(outputTensorDims->size == 3 && outputTensorDims->data[0] == 1
             && outputTensorDims->data[1] == outputSize && outputTensorDims->data[2] == 85);

  TfLiteTensor* inputTensor = interpreter->tensor(inputTensorIndices[0]);
  TfLiteTensor* outputTensor = interpreter->tensor(outputTensorIndices[0]);
  input = inputTensor->data.f;
  outputs = outputTensor->data.f;
}

cv::Mat TfLiteYoloV5Classifier::preprocess(const cv::Mat& rgbMat) const {
  const int& width = rgbMat.cols;
  const int& height = rgbMat.rows;
  bool isLandscape = width * inputSize.height >= height * inputSize.width;
  int resizeWidth = isLandscape ? inputSize.width
                                : (width * inputSize.height / height);
  int resizeHeight = isLandscape ? (height * inputSize.width / width)
                                 : inputSize.height;
  int left = (inputSize.width - resizeWidth) / 2;
  int right = (inputSize.width - resizeWidth) - left;
  int top = (inputSize.height - resizeHeight) / 2;
  int bottom = (inputSize.height - resizeHeight) - top;

  cv::Mat inputTensor;
  cv::resize(rgbMat, inputTensor, {resizeWidth, resizeHeight});
  cv::copyMakeBorder(inputTensor, inputTensor,
                     top, bottom, left, right,
                     cv::BORDER_CONSTANT, cv::Scalar(114, 114, 114));
  inputTensor.convertTo(inputTensor, CV_32FC3, 1.f / 255);
  return inputTensor;
}

void TfLiteYoloV5Classifier::inference(const cv::Mat& inputTensor) const {
  assert(inputTensor.cols == inputSize.width);
  assert(inputTensor.rows == inputSize.height);
  assert(inputTensor.type() == CV_32FC3);
  std::memcpy((void*) input, (void*) inputTensor.data, inputSize.area() * inputTensor.elemSize());
  interpreter->Invoke();
}

std::vector<BoundingBox> TfLiteYoloV5Classifier::postprocess(int width, int height) const {
  std::vector<BoundingBox> boxes;
  for (int i = 0; i < outputSize; i++) {
    float maxConfidence = 0;
    int maxLabel = -1;
    for (int label = 0; label < numLabels; label++) {
      float confidence = outputs[i * 85 + 5 + label];
      if (maxConfidence < confidence) {
        maxConfidence = confidence;
        maxLabel = label;
      }
    }
    maxConfidence *= outputs[i * 85 + 4];
    if (maxLabel == 0 && maxConfidence > confThres) {
      Rect rect = reconstructBox((float) outputs[i * 85 + 0],
                                 (float) outputs[i * 85 + 1],
                                 (float) outputs[i * 85 + 2],
                                 (float) outputs[i * 85 + 3],
                                 (float) width, (float) height);
      if (rect.l <= rect.r && rect.t <= rect.b) {
        boxes.emplace_back(INVALID_ID, rect, maxConfidence, maxLabel, O_INVALID);
      }
    }
  }
  return boxes;
}

Rect TfLiteYoloV5Classifier::reconstructBox(float x, float y, float w, float h,
                                            float imageWidth, float imageHeight) const {
  x *= (float) inputSize.width;
  y *= (float) inputSize.height;
  w *= (float) inputSize.width;
  h *= (float) inputSize.height;
  float gain = std::min((float) inputSize.width / (float) imageWidth,
                        (float) inputSize.height / (float) imageHeight);
  float xPad = ((float) inputSize.width - (float) imageWidth * gain) / 2;
  float yPad = ((float) inputSize.height - (float) imageHeight * gain) / 2;
  float newL = std::max(0.0f, ((x - w / 2 - xPad) / gain));
  float newT = std::max(0.0f, ((y - h / 2 - yPad) / gain));
  float newR = std::min(imageWidth, ((x + w / 2 - xPad) / gain));
  float newB = std::min(imageHeight, ((y + h / 2 - yPad) / gain));
  assert(0 <= newL && 0 <= newT && newL <= newR && newT <= newB);
  return {newL, newT, newR, newB};
}

Device TfLiteYoloV5Classifier::device() const {
  return GPU;
}

} // namespace md
