#include "mondrian/InferenceEngine.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/Worker.hpp"
#include "mondrian/model/MnnYoloV4Classifier.hpp"
#include "mondrian/model/TfLiteYoloV4Classifier.hpp"
#include "mondrian/model/TfLiteYoloV5Classifier.hpp"
#include "mondrian/model/TfLiteYoloV5ClassifierDSP.hpp"

#include <cmath>

namespace md {

InferenceEngine::InferenceEngine(const InferenceEngineConfig& config,
                                 JNIEnv* env, jobject app)
    : mConfig(config) {
  for (Device device: config.DEVICES) {
    if (device == GPU) {
      if (config.MODEL == "YOLO_V4" && config.RUNTIME == "MNN") {
        addClassifiers<MnnYoloV4Classifier>(device, config, env, app);
      } else if (config.MODEL == "YOLO_V4" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV4Classifier>(device, config, env, app);
      } else if (config.MODEL == "YOLO_V5" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV5Classifier>(device, config, env, app);
      } else {
        LOGE("Running %s model with %s runtime on GPU is not supported yet",
             config.MODEL.c_str(), config.RUNTIME.c_str());
      }
    } else if (device == DSP) {
      if (config.MODEL == "YOLO_V5" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV5ClassifierDSP>(device, config, env, app);
      } else {
        LOGE("Running %s model with %s runtime on DSP is not supported yet",
             config.MODEL.c_str(), config.RUNTIME.c_str());
      }
    } else {
      LOGE("Device %d is not supported yet", device);
    }
  }
}

template<typename T>
void InferenceEngine::addClassifiers(Device device, const InferenceEngineConfig& config,
                                     JNIEnv* env, jobject app) {
  std::map<std::pair<int, bool>, Classifier*> classifierMap;

  // classifiers for packed canvas inference
  bool forFullFrame = false;
  for (const auto& inputSize: config.INPUT_SIZES) {
    std::unique_ptr<Classifier> classifier = std::make_unique<T>(
        config.DATASET, inputSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD,
        config.USE_TINY, forFullFrame);
    classifierMap[{inputSize, forFullFrame}] = classifier.get();
    classifiers.push_back(std::move(classifier));
  }

  // classifier for full frame inference
  // identical with above code block inside the for loop
  int inputSize = config.FULL_FRAME_SIZE;
  forFullFrame = true;
  std::unique_ptr<Classifier> classifier = std::make_unique<T>(
      config.DATASET, inputSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD,
      config.USE_TINY, forFullFrame);
  classifierMap[{inputSize, forFullFrame}] = classifier.get();
  classifiers.push_back(std::move(classifier));

  workers[device] = std::make_unique<Worker>(
      this, device, classifierMap, mConfig.DRAW_INFERENCE_RESULT, env, app);
}

void InferenceEngine::profileLatency() const {
  for (const auto&[device, worker]: workers) {
    workers.at(device)->profileLatency(mConfig.PROFILE_WARMUPS, mConfig.PROFILE_RUNS);
  }
}

void InferenceEngine::enqueue(const cv::Mat& rgbMat, Device device, int inputSize, bool isFullFrame,
                              int key) {
  workers[device]->enqueue(rgbMat, inputSize, isFullFrame, key);
}

Result InferenceEngine::getResults(int key) {
  std::unique_lock<std::mutex> resultLock(resultMtx);
  resultCv.wait(resultLock, [this, key]() {
    return results.find(key) != results.end();
  });
  auto result = results.at(key);
  results.erase(key);
  return result;
}

void InferenceEngine::enqueueResult(const int handle, const Result& result) {
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.emplace(handle, result);
  resultLock.unlock();
  resultCv.notify_all();
}

std::map<Device, std::map<std::pair<int, bool>, time_us>> InferenceEngine::latencyTable() const {
  std::map<Device, std::map<std::pair<int, bool>, time_us>> latencyTable;
  for (const auto&[device, worker]: workers) {
    latencyTable[device] = worker->latencyMap();
  }
  return latencyTable;
}

} // namespace md
