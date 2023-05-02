#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <jni.h>

#include <map>
#include <queue>

#include "mondrian/Config.hpp"
#include "mondrian/Worker.hpp"
#include "mondrian/model/Classifier.hpp"

namespace md {

using Result = std::tuple<std::vector<BoundingBox>, std::pair<time_us, time_us>, Device>;

class InferenceEngine {
  friend Worker;

 public:
  InferenceEngine(const InferenceEngineConfig& config, JNIEnv* env, jobject app);

  void profileLatency() const;

  void enqueue(const cv::Mat& rgbMat, Device device, int inputSize, bool isFullFrame,
               int key);

  Result getResults(int key);

  std::map<Device, std::map<std::pair<int, bool>, time_us>> latencyTable() const;

 private:
  void enqueueResult(const int handle, const Result& result);

  template<typename T>
  void addClassifiers(Device device, const InferenceEngineConfig& config,
                      JNIEnv* env, jobject app);

  const InferenceEngineConfig mConfig;

  std::map<Device, std::unique_ptr<Worker>> workers;
  std::vector<std::unique_ptr<Classifier>> classifiers;

  std::mutex resultMtx;
  std::condition_variable resultCv;
  std::map<int, Result> results;
};

} // namespace md

#endif // INFERENCE_ENGINE_H
