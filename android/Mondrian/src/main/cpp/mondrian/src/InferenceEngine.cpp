#include "mondrian/InferenceEngine.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/Worker.hpp"
#include "mondrian/model/TfLiteYoloV5Classifier.hpp"
#include "mondrian/model/TfLiteYoloV5ClassifierDSP.hpp"

#include <cmath>

namespace md {

InferenceEngine::InferenceEngine(const InferenceEngineConfig& config,
                                 JNIEnv* env,
                                 jobject app)
    : config_(config) {
  for (Device device : config.DEVICES) {
    if (config.MODEL == "YOLO_V5" && device == GPU) {
      addClassifiers<TfLiteYoloV5Classifier>(device, config, env, app);
    } else if (config.MODEL == "YOLO_V5" && device == DSP) {
      addClassifiers<TfLiteYoloV5ClassifierDSP>(device, config, env, app);
    } else {
      LOGE("Running %s model on %s is not supported yet",
           config.MODEL.c_str(), str(device).c_str());
    }
  }
}

template<typename T>
void InferenceEngine::addClassifiers(Device device,
                                     const InferenceEngineConfig& config,
                                     JNIEnv* env,
                                     jobject app) {
  std::map<std::pair<int, bool>, Classifier*> classifierMap;

  // classifiers for packed canvas inference
  bool forFullFrame = false;
  for (const auto& inputSize : config.INPUT_SIZES) {
    std::unique_ptr<Classifier> classifier = std::make_unique<T>(
        config.DATASET, inputSize, config.CONF_THRES, config.IOU_THRES,
        config.USE_TINY, forFullFrame);
    classifierMap[{inputSize, forFullFrame}] = classifier.get();
    classifiers_.push_back(std::move(classifier));
  }

  if (device == GPU) {
    // classifier for full frame inference
    // identical with above code block inside the for loop
    int inputSize = config.FULL_FRAME_SIZE;
    forFullFrame = true;
    std::unique_ptr<Classifier> classifier = std::make_unique<T>(
        config.DATASET,
        inputSize,
        config.CONF_THRES,
        config.IOU_THRES,
        config.USE_TINY,
        forFullFrame);
    classifierMap[{inputSize, forFullFrame}] = classifier.get();
    classifiers_.push_back(std::move(classifier));
  }

  workers_[device] = std::make_unique<Worker>(
      this, device, classifierMap, config_.DRAW_INFERENCE_RESULT, env, app);
}

void InferenceEngine::profileLatency() const {
  for (const auto& [device, worker] : workers_) {
    workers_.at(device)->profileLatency(config_.PROFILE_WARMUPS, config_.PROFILE_RUNS);
  }
}

void InferenceEngine::enqueue(const cv::Mat& rgbMat,
                              const Device device,
                              const int inputSize,
                              const bool isFullFrame,
                              const Key key) {
  workers_[device]->enqueue(rgbMat, inputSize, isFullFrame, key);
}

Result InferenceEngine::getResults(Key key) {
  std::unique_lock<std::mutex> resultLock(resultMtx_);
  resultCv_.wait(resultLock, [this, key]() {
    return results_.find(key) != results_.end();
  });
  auto result = results_.at(key);
  results_.erase(key);
  return result;
}

void InferenceEngine::enqueueResult(const Key key, const Result& result) {
  std::unique_lock<std::mutex> resultLock(resultMtx_);
  results_.emplace(key, result);
  resultLock.unlock();
  resultCv_.notify_all();
}

LatencyTable InferenceEngine::latencyTable() const {
  LatencyTable latencyTable;
  for (const auto& [device, worker] : workers_) {
    latencyTable[device] = worker->latencyMap();
  }
  return latencyTable;
}

std::map<Device, time_us> InferenceEngine::remainingTimes() const {
  std::map<Device, time_us> times;
  for (const auto& [device, worker] : workers_) {
    times[device] = worker->remainingTime();
  }
  return times;
}

std::string str(const LatencyTable& latencyTable) {
  std::stringstream ss;
  for (auto& [device, sizeIsFullLatency] : latencyTable) {
    for (auto& [sizeIsFull, latency] : sizeIsFullLatency) {
      ss << str(device) << "_" << sizeIsFull.first << "=" << latency << " ";
    }
  }
  return ss.str();
}

std::string str(const std::map<Device, time_us>& remainingTimes) {
  std::stringstream ss;
  for (auto& [device, remainingTime] : remainingTimes) {
    ss << str(device) << "=" << remainingTime << " ";
  }
  return ss.str();
}

}  // namespace md
