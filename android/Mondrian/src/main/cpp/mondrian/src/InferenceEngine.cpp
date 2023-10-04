#include "mondrian/InferenceEngine.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/Worker.hpp"
#include "mondrian/model/TfLiteYoloV5Classifier.hpp"
#include "mondrian/model/TfLiteYoloV5ClassifierDSP.hpp"

#include <cmath>

namespace md {

InferenceEngine::InferenceEngine(const InferenceEngineConfig& config,
                                 JNIEnv* env,
                                 jobject app,
                                 chrome_tracer::ChromeTracer* tracer)
    : config_(config), tracer_(tracer) {
  int maxPackedCanvasSize = -1;
  for (const auto& [device, workerConfig] : config_.WORKER_CONFIGS) {
    for (const auto& inputSize : workerConfig.INPUT_SIZES) {
      maxPackedCanvasSize = std::max(maxPackedCanvasSize, inputSize);
    }
  }

  for (Device device : DEVICES) {
    if (device == Device::GPU) {
      addWorker<TfLiteYoloV5Classifier>(device, maxPackedCanvasSize, env, app);
    } else if (device == Device::DSP) {
      addWorker<TfLiteYoloV5ClassifierDSP>(device, maxPackedCanvasSize, env, app);
    } else {
      LOGE("%s device is not supported yet", str(device).c_str());
    }
  }
}

template<typename T>
void InferenceEngine::addWorker(Device device,
                                int maxPackedCanvasSize,
                                JNIEnv* env,
                                jobject app) {
  std::map<std::pair<int, bool>, Classifier*> classifierMap;

  // classifiers for full frame inference
  if (device == config_.FULL_DEVICE) {
    std::unique_ptr<Classifier> classifier = std::make_unique<T>(
        config_.FULL_MODEL,
        config_.FULL_FRAME_SIZE,
        true,
        config_.FULL_DATASET,
        config_.CONF_THRES,
        config_.IOU_THRES);
    classifierMap[{config_.FULL_FRAME_SIZE, true}] = classifier.get();
    classifiers_.push_back(std::move(classifier));
  }

  // classifiers for packed canvas inference
  if (config_.WORKER_CONFIGS.find(device) != config_.WORKER_CONFIGS.end()) {
    const WorkerConfig& workerConfig = config_.WORKER_CONFIGS.at(device);
    for (const auto& inputSize : workerConfig.INPUT_SIZES) {
      std::unique_ptr<Classifier> classifier = std::make_unique<T>(
          workerConfig.MODEL,
          inputSize,
          false,
          workerConfig.DATASET,
          config_.CONF_THRES,
          config_.IOU_THRES);
      classifierMap[{inputSize, false}] = classifier.get();
      classifiers_.push_back(std::move(classifier));
    }
  }

  workers_[device] = std::make_unique<Worker>(
      this,
      device,
      classifierMap,
      config_.DRAW_INFERENCE_RESULT,
      maxPackedCanvasSize,
      env,
      app,
      tracer_);
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

Result InferenceEngine::getResult(Key key, bool isCheckedKey) {
  std::unique_lock<std::mutex> resultLock(resultMtx_);
  if (isCheckedKey) {
    assert(results_.find(key) != results_.end());
  } else {
    resultCv_.wait(resultLock, [this, key]() {
      return results_.find(key) != results_.end();
    });
  }
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

void InferenceEngine::waitForAnyResults() {
  std::unique_lock<std::mutex> resultLock(resultMtx_);
  resultCv_.wait(resultLock, [this]() {
    return !results_.empty();
  });
}

bool InferenceEngine::isReady(Key key) {
  std::lock_guard<std::mutex> resultLock(resultMtx_);
  return results_.find(key) != results_.end();
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
