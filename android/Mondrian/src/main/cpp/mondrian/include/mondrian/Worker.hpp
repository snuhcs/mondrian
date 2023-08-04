#ifndef IMPL_WORKER_HPP_
#define IMPL_WORKER_HPP_

#include <jni.h>

#include <list>
#include <thread>

#include "mondrian/model/Classifier.hpp"

namespace md {

class InferenceEngine;

using Key = std::pair<int, int>;
using Result = std::tuple<std::vector<BoundingBox>, std::pair<time_us, time_us>, Device>;
using LatencyTable = std::map<Device, std::map<std::pair<int, bool>, time_us>>;

class Worker {
 public:
  Worker(InferenceEngine* engine, Device device,
         std::map<std::pair<int, bool>, Classifier*> classifierMap,
         bool draw, JNIEnv* env, jobject app);

  ~Worker();

  void enqueue(const cv::Mat& rgbMat, int inputSize, bool isFullFrame, Key key);

  std::map<std::pair<int, bool>, time_us> latencyMap();

  time_us remainingTime();

  void profileLatency(int warmupRuns, int numRuns);

 private:
  void work();

  void updateLatency(int size, bool isFullFrame, time_us newLatency);

  void updateRemainingTime();

  void drawInferenceResult(const cv::Mat& rgbMat,
                           const std::vector<BoundingBox>& boxes,
                           bool isFullFrame);

  const Device device_;
  const bool draw_;

  std::mutex mtx_;
  std::condition_variable cv_;
  std::list<std::tuple<const cv::Mat, int, bool, Key>> inputs_;

  InferenceEngine* engine_;
  std::map<std::pair<int, bool>, Classifier*> classifierMap_;
  std::map<std::pair<int, bool>, time_us> latencyMap_;
  time_us estimatedEndTime_;

  bool stop_;
  std::thread thread_;

  int maxPackedCanvasSize_;
  JavaVM* jvm_;
  JNIEnv* env_;
  jobject app_;
  jclass class_MondrianApp_;
  jmethodID MondrianApp_drawOutput_;
  jclass class_ArrayList_;
  jmethodID ArrayList_init_;
  jmethodID ArrayList_add_;
  jclass class_BoundingBox_;
  jmethodID BoundingBox_init_;
};

} // namespace md

#endif // IMPL_WORKER_HPP_
