#include "mondrian/InferenceEngine.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/Worker.hpp"
#include "mondrian/model/TfLiteYoloV4Classifier.hpp"
#include "mondrian/model/TfLiteYoloV5Classifier.hpp"
#include "mondrian/model/TfLiteYoloV5ClassifierDSP.hpp"

#include <cmath>

namespace md {

InferenceEngine::InferenceEngine(const InferenceEngineConfig& config,
                                 JNIEnv* env,
                                 jobject app)
    : config_(config) {
  for (Device device : config.DEVICES) {
    if (config.MODEL == "YOLO_V4" && device == GPU) {
      addClassifiers<TfLiteYoloV4Classifier>(device, config, env, app);
    } else if (config.MODEL == "YOLO_V5" && device == GPU) {
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

  // classifiers_ for packed canvas inference
  bool forFullFrame = false;
  for (const auto& inputSize : config.INPUT_SIZES) {
    std::unique_ptr<Classifier> classifier = std::make_unique<T>(
        config.DATASET, inputSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD,
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
        config.CONF_THRESHOLD,
        config.IOU_THRESHOLD,
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
                              Device device,
                              int inputSize,
                              bool isFullFrame,
                              int key) {
  workers_[device]->enqueue(rgbMat, inputSize, isFullFrame, key);
}

Result InferenceEngine::getResults(int key) {
  std::unique_lock<std::mutex> resultLock(resultMtx_);
  resultCv_.wait(resultLock, [this, key]() {
    return results_.find(key) != results_.end();
  });
  auto result = results_.at(key);
  results_.erase(key);
  return result;
}

void InferenceEngine::enqueueResult(const int handle, const Result& result) {
  std::unique_lock<std::mutex> resultLock(resultMtx_);
  results_.emplace(handle, result);
  resultLock.unlock();
  resultCv_.notify_all();
}

std::map<Device, std::map<std::pair<int, bool>, time_us>> InferenceEngine::latencyTable() const {
  std::map<Device, std::map<std::pair<int, bool>, time_us>> latencyTable;
  for (const auto& [device, worker] : workers_) {
    latencyTable[device] = worker->latencyMap();
  }
  return latencyTable;
}

} // namespace md
