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

  int enqueue(const cv::Mat mat, Device device, int inputSize, int key);

  std::pair<int, std::vector<BoundingBox>> getResults(const int handle);

  std::map<Device, std::map<int, time_us>> getInferenceTimeUs() const;

  std::vector<int> getInputSizes() const;

 private:
  std::tuple<int, const cv::Mat, const int> getInput();

  void enqueueResults(const int handle, const std::vector<BoundingBox>& boxes);

  void drawInferenceResult(const cv::Mat& mat, const std::vector<BoundingBox>& boxes);

  template <typename T>
  void initClassifiers(const InferenceEngineConfig& config);

  const InferenceEngineConfig mConfig;

  std::map<Device, std::unique_ptr<Worker>> workers;
  std::vector<std::unique_ptr<Classifier>> classifiers;

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

  int mHandle;
  std::queue<std::tuple<int, const cv::Mat, const int>> inputs;
  std::mutex inputMtx;
  std::condition_variable inputCv;
  std::map<int, std::vector<BoundingBox>> results;
  std::mutex resultMtx;
  std::condition_variable resultCv;
};

} // namespace rm

#endif // INFERENCE_ENGINE_H
