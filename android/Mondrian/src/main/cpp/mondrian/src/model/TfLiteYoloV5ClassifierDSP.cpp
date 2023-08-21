#include "mondrian/model/TfLiteYoloV5ClassifierDSP.hpp"

#include <map>
#include <set>

#include "opencv2/imgproc.hpp"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/nnapi/nnapi_util.h"
#include "tensorflow/lite/delegates/xnnpack/xnnpack_delegate.h"
#include "tensorflow/lite/delegates/nnapi/nnapi_delegate.h"

#include "mondrian/Config.hpp"
#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

TfLiteYoloV5ClassifierDSP::TfLiteYoloV5ClassifierDSP(const std::string& modelName,
                                                     const int inputSize,
                                                     const bool forFullFrame,
                                                     const std::string& dataset,
                                                     const float confThres,
                                                     const float iouThres)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 32) * (inputSize / 32) * 63,
                 confThres, iouThres) {
  std::stringstream ss;
  ss << "/data/local/tmp/models/";
  if (dataset != "pretrained") {
    ss << dataset << "-"
       << (forFullFrame ? "full" : "pack") << "-";
  }
  ss << modelName << "-"
     << inputSize << "-"
     << "int8.tflite";

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

  tflite::StatefulNnApiDelegate::Options options = tflite::StatefulNnApiDelegate::Options();
  options.accelerator_name = "qti-dsp";
  delegate = new tflite::StatefulNnApiDelegate(options);

  if (interpreter->ModifyGraphWithDelegate(delegate) != kTfLiteOk) {
    LOGE("YoloV5 delegate application failed");
  } else {
    LOGD("YoloV5 delegate applied");
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
  input = inputTensor->data.uint8;
  outputs = outputTensor->data.uint8;

  auto* inputQuantization = (TfLiteAffineQuantization*) inputTensor->quantization.params;
  auto* outputQuantization = (TfLiteAffineQuantization*) outputTensor->quantization.params;
  assert(inputTensor->quantization.type == kTfLiteAffineQuantization
             && inputQuantization->scale->size == 1
             && inputQuantization->zero_point->size == 1
             && inputQuantization->quantized_dimension == 0);
  assert(outputTensor->quantization.type == kTfLiteAffineQuantization
             && outputQuantization->scale->size == 1
             && outputQuantization->zero_point->size == 1
             && outputQuantization->quantized_dimension == 0);

  int inputBias = inputQuantization->zero_point->data[0];
  float inputScale = inputQuantization->scale->data[0];
  assert(inputBias == 0);
  assert(std::abs(1.0 / inputScale - 255.0) < 1.0);

  outputBias = outputQuantization->zero_point->data[0];
  outputScale = outputQuantization->scale->data[0];
  LOGD("[TfLiteYoloV5ClassifierDSP] outputBias=%d outputScale=%f", outputBias, outputScale);
}

cv::Mat TfLiteYoloV5ClassifierDSP::preprocess(const cv::Mat& rgbMat) const {
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
  return inputTensor;
}

void TfLiteYoloV5ClassifierDSP::inference(const cv::Mat& inputTensor) const {
  assert(inputTensor.cols == inputSize.width);
  assert(inputTensor.rows == inputSize.height);
  assert(inputTensor.type() == CV_8UC3);
  std::memcpy((void*) input, (void*) inputTensor.data, inputSize.area() * inputTensor.elemSize());
  interpreter->Invoke();
}

std::vector<BoundingBox> TfLiteYoloV5ClassifierDSP::postprocess(int width, int height) const {
  std::vector<BoundingBox> boxes;
  for (int i = 0; i < outputSize; i++) {
    uint8_t maxConfidenceQuant = 0;
    int maxLabel = -1;
    for (int label = 0; label < numLabels; label++) {
      uint8_t confidenceQuant = outputs[i * 85 + 5 + label];
      if (maxConfidenceQuant < confidenceQuant) {
        maxConfidenceQuant = confidenceQuant;
        maxLabel = label;
      }
    }
    float maxConfidence = dequantize(maxConfidenceQuant) * dequantize(outputs[i * 85 + 4]);
    if (maxLabel == 0 && maxConfidence > confThres) {
      Rect rect = reconstructBox((float) dequantize(outputs[i * 85 + 0]),
                                 (float) dequantize(outputs[i * 85 + 1]),
                                 (float) dequantize(outputs[i * 85 + 2]),
                                 (float) dequantize(outputs[i * 85 + 3]),
                                 (float) width, (float) height);
      if (rect.l <= rect.r && rect.t <= rect.b) {
        boxes.emplace_back(INVALID_OID, -1, rect, maxConfidence, maxLabel, Origin::INVALID);
      }
    }
  }
  return boxes;
}

Rect TfLiteYoloV5ClassifierDSP::reconstructBox(float x, float y, float w, float h,
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

Device TfLiteYoloV5ClassifierDSP::device() const {
  return Device::DSP;
}

float TfLiteYoloV5ClassifierDSP::dequantize(uint8_t value) const {
  return (float) (value - outputBias) * outputScale;
}

} // namespace md
