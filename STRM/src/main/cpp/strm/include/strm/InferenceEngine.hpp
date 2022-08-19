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
                  JavaVM* vm, JNIEnv* env, jobject strm);

  void enqueue(const cv::Mat mat, Device device, int inputSize, int key);

  Result getResults(int key);

  std::map<Device, std::map<int, time_us>> getInferenceTimeTable() const;

  std::vector<int> getInputSizes() const;

 private:
  void enqueueResults(const int handle, const Result& boxes);

  void drawInferenceResult(const cv::Mat& mat, const std::vector<BoundingBox>& boxes);

  template<typename T>
  void addClassifiers(Device device, const InferenceEngineConfig& config);

  const InferenceEngineConfig mConfig;

  std::map<Device, std::unique_ptr<Worker>> workers;
  std::vector<std::unique_ptr<Classifier>> classifiers;

  std::mutex resultMtx;
  std::condition_variable resultCv;
  std::map<int, Result> results;

  JavaVM* jvm;
  JNIEnv* env;
  jobject strm;
  jclass class_SpatioTemporalRoIMixer;
  jmethodID SpatioTemporalRoIMixer_drawInferenceResult;
  jclass class_ArrayList;
  jmethodID ArrayList_init;
  jmethodID ArrayList_add;
  jclass class_BoundingBox;
  jmethodID BoundingBox_init;
};

} // namespace rm

#endif // INFERENCE_ENGINE_H
