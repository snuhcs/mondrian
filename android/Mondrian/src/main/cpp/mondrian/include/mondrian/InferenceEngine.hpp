#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <jni.h>

#include <map>
#include <queue>

#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/Worker.hpp"

namespace md {

class Classifier;

std::string str(const LatencyTable& latencyTable);

std::string str(const std::map<Device, time_us>& remainingTimes);

class InferenceEngine {
 public:
  InferenceEngine(const InferenceEngineConfig& config, JNIEnv* env, jobject app);

  void profileLatency() const;

  void enqueue(const cv::Mat& rgbMat,
               Device device,
               int inputSize,
               bool isFullFrame,
               Key key);

  void enqueueResult(const Key key, const Result& result);

  Result getResults(Key key);

  LatencyTable latencyTable() const;

  std::map<Device, time_us> remainingTimes() const;

 private:
  template<typename T>
  void addClassifiers(Device device,
                      const InferenceEngineConfig& config,
                      JNIEnv* env,
                      jobject app);

  const InferenceEngineConfig config_;

  std::map<Device, std::unique_ptr<Worker>> workers_;
  std::vector<std::unique_ptr<Classifier>> classifiers_;

  std::mutex resultMtx_;
  std::condition_variable resultCv_;
  std::map<Key, Result> results_;
};

} // namespace md

#endif // INFERENCE_ENGINE_H
