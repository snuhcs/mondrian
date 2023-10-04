#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <jni.h>

#include <map>
#include <queue>

#include "chrome_tracer/tracer.h"
#include "opencv2/core/mat.hpp"

#include "mondrian/Config.hpp"
#include "mondrian/Worker.hpp"

namespace md {

class Classifier;

std::string str(const LatencyTable& latencyTable);

std::string str(const std::map<Device, time_us>& remainingTimes);

class InferenceEngine {
 public:
  InferenceEngine(const InferenceEngineConfig& config,
                  JNIEnv* env,
                  jobject app,
                  chrome_tracer::ChromeTracer* tracer);

  void profileLatency() const;

  void enqueue(const cv::Mat& rgbMat,
               const Device device,
               const int inputSize,
               const bool isFullFrame,
               const Key key);

  void enqueueResult(const Key key, const Result& result);

  Result getResult(Key key, bool isCheckedKey);

  void waitForAnyResults();

  bool isReady(Key key);

  LatencyTable latencyTable() const;

  std::map<Device, time_us> remainingTimes() const;

 private:
  template<typename T>
  void addWorker(Device device, int maxPackedCanvasSize, JNIEnv* env, jobject app);

  const InferenceEngineConfig config_;

  std::map<Device, std::unique_ptr<Worker>> workers_;
  std::vector<std::unique_ptr<Classifier>> classifiers_;

  std::mutex resultMtx_;
  std::condition_variable resultCv_;
  std::map<Key, Result> results_;

  chrome_tracer::ChromeTracer* tracer_;
};

} // namespace md

#endif // INFERENCE_ENGINE_H
