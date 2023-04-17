#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <jni.h>

#include <map>
#include <queue>

#include "strm/Config.hpp"
#include "strm/impl/Worker.hpp"
#include "strm/impl/models/Classifier.hpp"

namespace rm {

class InferenceEngine {
  friend Worker;

 public:
  InferenceEngine(const InferenceEngineConfig& config,
                  JavaVM* vm, JNIEnv* env, jobject emulator);

  void enqueue(const cv::Mat& rgbMat, Device device, int inputSize, bool isFullFrame,
               int key);

  Result getResults(int key);

  std::map<Device, std::map<std::tuple<int, bool>, time_us>> getInferenceTimeTable() const;

  std::vector<int> getInputSizes() const;

 private:
  void enqueueResults(const int handle, const Result& boxes);

  template<typename T>
  void addClassifiers(Device device, const InferenceEngineConfig& config,
                      JavaVM* vm, JNIEnv* env, jobject emulator);

  const InferenceEngineConfig mConfig;

  std::map<Device, std::unique_ptr<Worker>> workers;
  std::vector<std::unique_ptr<Classifier>> classifiers;

  std::mutex resultMtx;
  std::condition_variable resultCv;
  std::map<int, Result> results;
};

} // namespace rm

#endif // INFERENCE_ENGINE_H
