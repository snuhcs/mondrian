#ifndef YOLO_V4_CLASSIFIER_HPP_
#define YOLO_V4_CLASSIFIER_HPP_

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/interpreter_builder.h"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"

#include "strm/DataType.hpp"

namespace rm {

class YoloV4Classifier {
 public:
  YoloV4Classifier() {
    auto model = tflite::FlatBufferModel::BuildFromFile("/data/local/tmp/models/yolov4-960.tflite");
    if (model == nullptr) {
      return;
    }
    auto resolver = std::make_unique<tflite::ops::builtin::BuiltinOpResolver>();
    if (tflite::InterpreterBuilder(*model, *resolver)(&tfLite, 1)) {
      return;
    }

    TfLiteGpuDelegateOptionsV2 gpu_opts = TfLiteGpuDelegateOptionsV2Default();
    gpu_opts.inference_priority1 = TFLITE_GPU_INFERENCE_PRIORITY_MIN_LATENCY;
    gpu_opts.inference_priority2 = TFLITE_GPU_INFERENCE_PRIORITY_MIN_MEMORY_USAGE;
    gpu_opts.inference_priority3 = TFLITE_GPU_INFERENCE_PRIORITY_MAX_PRECISION;
    gpu_opts.experimental_flags |= TFLITE_GPU_EXPERIMENTAL_FLAGS_ENABLE_QUANT;
    gpu_opts.max_delegated_partitions = 100;
    tflite::Interpreter::TfLiteDelegatePtr gpu_delegate = tflite::Interpreter::TfLiteDelegatePtr(
        TfLiteGpuDelegateV2Create(&gpu_opts), &TfLiteGpuDelegateV2Delete);

    if (tfLite->ModifyGraphWithDelegate(std::move(gpu_delegate))) {
      return;
    }
    if (tfLite->AllocateTensors()) {
      return;
    }
  }

  std::vector<BoundingBox> recognizeImage(cv::Mat mat, int originalWidth, int originalHeight) {
    return nms(getDetectionsForFull(mat, originalWidth, originalHeight));
  }

 private:
  std::vector<BoundingBox> getDetectionsForFull(cv::Mat byteBuffer, int originalWidth, int originalHeight) {
    std::vector<BoundingBox> detections;

    assert(tfLite->inputs().size() == 1);
    float* input = tfLite->typed_tensor<float>(0);
    std::memcpy(input, byteBuffer.data, byteBuffer.total() * byteBuffer.elemSize());

    tfLite->Invoke();

    float* bboxes = tfLite->typed_output_tensor<float>(0);
    float* confidences = tfLite->typed_output_tensor<float>(1);

    for (int i = 0; i < OUTPUT_WIDTH; i++) {
      float maxConfidence = 0;
      int detectedClass = -1;
      for (int c = 0; c < NUM_LABELS; c++) {
        if (confidences[i * NUM_LABELS + c] > maxConfidence) {
          detectedClass = c;
          maxConfidence = confidences[c];
        }
      }
      if (detectedClass == 0 && maxConfidence > CONF_THRESHOLD) {
        float xPos = bboxes[i * 4 + 0];
        float yPos = bboxes[i * 4 + 1];
        float w = bboxes[i * 4 + 2];
        float h = bboxes[i * 4 + 3];
        Rect rect(
            (int) (std::max(0, (int) (xPos - w / 2)) * originalWidth / INPUT_SIZE),
            (int) (std::max(0, (int) (yPos - h / 2)) * originalHeight / INPUT_SIZE),
            (int) (std::min(INPUT_SIZE - 1, (int) (xPos + w / 2)) * originalWidth / INPUT_SIZE),
            (int) (std::min(INPUT_SIZE - 1, (int) (yPos + h / 2)) * originalHeight / INPUT_SIZE));
        detections.push_back(BoundingBox(rect, maxConfidence, "person"));
      }
    }
    return detections;
  }

  std::vector<BoundingBox> nms(std::vector<BoundingBox> boxes) {
    std::vector<BoundingBox> nmsList;

    for (int k = 0; k < NUM_LABELS; k++) {
      if (k != 0) {
        continue;
      }

      auto comp = [](const BoundingBox& l, const BoundingBox& r) -> bool {
        return l.confidence > r.confidence;
      };
      std::set<BoundingBox, decltype(comp)> sortedBoxes(comp);

      for (BoundingBox box : boxes) {
        if (box.labelName == "person") {
          sortedBoxes.insert(box);
        }
      }

      while (sortedBoxes.size() > 0) {
        auto startIt = sortedBoxes.begin();
        const BoundingBox& max = *startIt;
        nmsList.push_back(max);
        sortedBoxes.erase(startIt);

        for (auto it = sortedBoxes.begin(); it != sortedBoxes.end();) {
          if (max.location.iou(it->location) >= IOU_THRESHOLD) {
            it = sortedBoxes.erase(it);
          } else {
            it++;
          }
        }
      }
    }
    return nmsList;
  }

  const int NUM_LABELS = 80;
  const int INPUT_SIZE = 960;
  const int OUTPUT_WIDTH = (INPUT_SIZE / 32) * (INPUT_SIZE / 32) * 63;
  const float CONF_THRESHOLD = 0.1;
  const float IOU_THRESHOLD = 0.3;

  std::unique_ptr<tflite::Interpreter> tfLite;
};

}

#endif // YOLO_V4_CLASSIFIER_HPP_
