#ifndef CPP_INFERENCE_ENGINE_H
#define CPP_INFERENCE_ENGINE_H

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model_builder.h"

#include "strm/InferenceEngine.hpp"

namespace rm {

class CppInferenceEngine : public InferenceEngine {
 public:
  CppInferenceEngine() {
    auto model = tflite::FlatBufferModel::BuildFromFile()
    interpreter.reset(new tflite::Interpreter());
  }

  int enqueue(const cv::Mat& mat, bool isFull) {

  }

  std::vector<BoundingBox> getResults(const int handle) {

  }

 private:
  std::unique_ptr<tflite::Interpreter> interpreter;
};

}

#endif // CPP_INFERENCE_ENGINE_H
