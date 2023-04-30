#ifndef IMPL_WORKER_HPP_
#define IMPL_WORKER_HPP_

#include <jni.h>

#include <queue>
#include <thread>

#include "mondrian/model/Classifier.hpp"

namespace md {

class InferenceEngine;

class Worker {
 public:
  Worker(InferenceEngine* engine, Device device, std::map<std::tuple<int, bool>, Classifier*> classifierMap,
         bool draw, JNIEnv* env, jobject app);

  ~Worker();

  void enqueue(const cv::Mat& rgbMat, int inputSize, bool isFullFrame, int key);

  std::map<std::tuple<int, bool>, time_us> getInferenceTimes();

 private:
  void work();

  void drawInferenceResult(const cv::Mat& rgbMat, const std::vector<BoundingBox>& boxes);

  const Device device;
  const bool draw;

  std::mutex mtx;
  std::condition_variable cv;
  std::queue<std::tuple<const cv::Mat, int, bool, int>> inputs;

  InferenceEngine* engine;
  std::map<std::tuple<int, bool>, Classifier*> classifierMap;

  std::atomic_bool isClosed;
  std::thread thread;

  JavaVM* jvm;
  JNIEnv* env;
  jobject app;
  jclass class_MondrianApp;
  jmethodID MondrianApp_drawOutput;
  jclass class_ArrayList;
  jmethodID ArrayList_init;
  jmethodID ArrayList_add;
  jclass class_BoundingBox;
  jmethodID BoundingBox_init;
};

} // namespace md

#endif // IMPL_WORKER_HPP_
