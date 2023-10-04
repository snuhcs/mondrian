#ifndef IMPL_WORKER_HPP_
#define IMPL_WORKER_HPP_

#include <jni.h>

#include <list>
#include <map>
#include <thread>

#include "chrome_tracer/tracer.h"

#include "mondrian/model/Classifier.hpp"

namespace md {

class InferenceEngine;

using Key = std::pair<int, int>;
using LatencyTable = std::map<Device, std::map<std::pair<int, bool>, time_us>>;

struct Input {
  const cv::Mat rgbMat;
  int size;
  bool full;
  Key key;
};

struct Result {
  std::vector<BoundingBox> boxes;
  time_us detectionStart;
  time_us detectionEnd;
  Device device;
};

class Worker {
 public:
  Worker(InferenceEngine* engine,
         Device device,
         std::map<std::pair<int, bool>, Classifier*> classifierMap,
         bool draw,
         int maxPackedCanvasSize,
         JNIEnv* env,
         jobject app,
         chrome_tracer::ChromeTracer* tracer);

  ~Worker();

  void enqueue(const cv::Mat& rgbMat,
               const int inputSize,
               const bool isFullFrame,
               const Key key);

  std::map<std::pair<int, bool>, time_us> latencyMap();

  time_us remainingTime();

  void profileLatency(int warmupRuns, int numRuns);

 private:
  void work();

  void updateLatency(int size, bool isFullFrame, time_us newLatency);

  void updateRemainingTime();

  void drawInferenceResult(const cv::Mat& rgbMat,
                           const std::vector<BoundingBox>& boxes,
                           const bool isFullFrame);

  const Device device_;
  const bool draw_;

  std::mutex mtx_;
  std::condition_variable cv_;
  std::list<Input> inputs_;

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

  const std::string tag_;
  chrome_tracer::ChromeTracer* tracer_;
};

} // namespace md

#endif // IMPL_WORKER_HPP_
