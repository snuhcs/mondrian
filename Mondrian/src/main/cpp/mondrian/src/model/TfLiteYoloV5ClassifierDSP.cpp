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

TfLiteYoloV5ClassifierDSP::TfLiteYoloV5ClassifierDSP(std::string dataset, int inputSize,
                                                     float confidenceThreshold, float iouThreshold,
                                                     bool isTiny, bool forFullFrame)
    : Classifier(NUM_LABELS, inputSize, (inputSize / 64) * (inputSize / 64) * 252,
                 confidenceThreshold, iouThreshold),
      delegate(nullptr, [](TfLiteDelegate* d) {}) {
  // TODO : use forFullFrame
  std::stringstream ss;
  ss << "/data/local/tmp/models/yolov5" << (isTiny ? "s-" : "x-") << inputSize << "-int8.tflite";
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
  options.cache_dir = "/data/data/hcs.offloading.mondrian/";
  options.model_token = ss.str().c_str();
  options.max_number_delegated_partitions = 0; // Unlimited partition
  options.accelerator_name = "qti-dsp";
  delegate = std::move(tflite::Interpreter::TfLiteDelegatePtr(
      new tflite::StatefulNnApiDelegate(options),
      [](TfLiteDelegate* tfLiteDelegate) {
        delete reinterpret_cast<tflite::StatefulNnApiDelegate*>(tfLiteDelegate);
      }));

  if (interpreter->ModifyGraphWithDelegate(delegate.get()) != kTfLiteOk) {
    LOGE("YoloV5 delegate application failed");
  } else {
    LOGD("YoloV5 delegate applied");
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

  TfLiteTensor* inputTensor = interpreter->tensor(inputTensorIndices[0]);
  TfLiteTensor* outputTensor = interpreter->tensor(outputTensorIndices[0]);
  assert(inputTensor->quantization.type == kTfLiteAffineQuantization &&
         outputTensor->quantization.type == kTfLiteAffineQuantization);
  auto* inputQuantization = (TfLiteAffineQuantization*) inputTensor->quantization.params;
  auto* outputQuantization = (TfLiteAffineQuantization*) outputTensor->quantization.params;
  assert(inputQuantization->scale->size == 1 &&
         inputQuantization->zero_point->size == 1 &&
         inputQuantization->quantized_dimension == 0);
  assert(outputQuantization->scale->size == 1 &&
         outputQuantization->zero_point->size == 1 &&
         outputQuantization->quantized_dimension == 0);
  inputBias = inputQuantization->zero_point->data[0];
  outputBias = outputQuantization->zero_point->data[0];
  inputScale = inputQuantization->scale->data[0];
  outputScale = outputQuantization->scale->data[0];

  input = inputTensor->data.int8;
  outputs = outputTensor->data.int8;
}

cv::Mat TfLiteYoloV5ClassifierDSP::preprocess(const cv::Mat& rgbMat) {
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

void TfLiteYoloV5ClassifierDSP::inference(const cv::Mat& mat) {
  assert(mat.cols == inputSize.width && mat.rows == inputSize.height && mat.type() == CV_32FC3);
  mat /= inputScale;
  cv::Mat quantizedMat;
  mat.convertTo(quantizedMat, CV_8SC3);
  quantizedMat += inputBias;
  assert(quantizedMat.cols == inputSize.width && quantizedMat.rows == inputSize.height &&
         quantizedMat.type() == CV_8SC3);
  std::memcpy((void*) input, (void*) quantizedMat.data, inputSize.area() * quantizedMat.elemSize());
  interpreter->Invoke();
}

std::vector<BoundingBox> TfLiteYoloV5ClassifierDSP::recognizeImage(const cv::Mat& rgbMat) {
  inference(preprocess(rgbMat));

  std::vector<BoundingBox> detections;
  for (int i = 0; i < outputSize; i++) {
    const int8_t* box = &outputs[i * 85];
    const int8_t* classConfidences = &outputs[i * 85 + 5];
    float maxConfidence = 0;
    int maxLabel = -1;
    for (int label = 0; label < numLabels; label++) {
      float confidence = float(classConfidences[label] - outputBias) * outputScale;
      if (maxConfidence < confidence) {
        maxLabel = label;
        maxConfidence = confidence;
      }
    }
    maxConfidence *= float(outputs[i * 85 + 4]) * outputScale;
    if (maxLabel == 0 && maxConfidence > confidenceThreshold) {
      detections.push_back(BoundingBox(
          INVALID_ID,
          reconstructBox(float(box[0] - outputBias) * outputScale,
                         float(box[1] - outputBias) * outputScale,
                         float(box[2] - outputBias) * outputScale,
                         float(box[3] - outputBias) * outputScale,
                         rgbMat.cols, rgbMat.rows),
          maxConfidence, maxLabel, O_INVALID));
    }
  }
  return nms(detections, numLabels, iouThreshold);
}

Rect TfLiteYoloV5ClassifierDSP::reconstructBox(float x, float y, float w, float h,
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

} // namespace md
