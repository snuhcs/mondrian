#ifndef IMPL_CUSTOM_INFERENCE_ENGINE_H
#define IMPL_CUSTOM_INFERENCE_ENGINE_H

#include <jni.h>

#include <map>
#include <queue>

#include "strm/InferenceEngine.hpp"
#include "strm/impl/ImplConfig.hpp"
#include "strm/impl/models/Classifier.hpp"

namespace rm {

class Worker;

class CustomInferenceEngine : public InferenceEngine {
  friend Worker;

 public:
  CustomInferenceEngine(const InferenceEngineConfig& config,
                        JavaVM* vm, JNIEnv* env, jobject strm, bool draw);

  int enqueue(const cv::Mat mat, const bool isFull) override;

  std::vector<BoundingBox> getResults(const int handle) override;

  const cv::Size& getInputSize() const override;

  long long getInferenceTimeMs() override;

 private:
  std::tuple<int, const cv::Mat, bool> getInput();

  void enqueueResults(const int handle, const std::vector<BoundingBox>& boxes);

  void drawInferenceResult(const cv::Mat& mat, const std::vector<BoundingBox>& boxes);

  template <typename T>
  void initClassifiers(const InferenceEngineConfig& config);

  std::vector<std::unique_ptr<Worker>> workers;
  std::vector<std::unique_ptr<Classifier>> classifiers;
  std::vector<std::unique_ptr<Classifier>> fullClassifiers;

  const bool draw;
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
  std::queue<std::tuple<int, const cv::Mat, bool>> inputs;
  std::mutex inputMtx;
  std::condition_variable inputCv;
  std::map<int, std::vector<BoundingBox>> results;
  std::mutex resultMtx;
  std::condition_variable resultCv;
};

} // namespace rm

#endif // IMPL_CUSTOM_INFERENCE_ENGINE_H
