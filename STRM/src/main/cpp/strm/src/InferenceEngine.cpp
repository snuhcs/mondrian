#include "strm/InferenceEngine.hpp"

#include "strm/Log.hpp"
#include "strm/impl/Worker.hpp"
#include "strm/impl/models/MnnYoloV4Classifier.hpp"
#include "strm/impl/models/TfLiteYoloV4Classifier.hpp"
#include "strm/impl/models/TfLiteYoloV5Classifier.hpp"
#include "strm/impl/models/TfLiteYoloV5ClassifierDSP.hpp"

#include <cmath>

namespace rm {

InferenceEngine::InferenceEngine(const InferenceEngineConfig& config,
                                 JavaVM* vm, JNIEnv* env, jobject emulator)
    : mConfig(config) {
  for (Device device: config.DEVICES) {
    if (device == GPU) {
      if (config.MODEL == "YOLO_V4" && config.RUNTIME == "MNN") {
        addClassifiers<MnnYoloV4Classifier>(device, config, vm, env, emulator);
      } else if (config.MODEL == "YOLO_V4" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV4Classifier>(device, config, vm, env, emulator);
      } else if (config.MODEL == "YOLO_V5" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV5Classifier>(device, config, vm, env, emulator);
      } else {
        LOGE("Running %s model with %s runtime on GPU is not supported yet",
             config.MODEL.c_str(), config.RUNTIME.c_str());
      }
    } else if (device == DSP) {
      if (config.MODEL == "YOLO_V5" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV5ClassifierDSP>(device, config, vm, env, emulator);
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
                                     JavaVM* vm, JNIEnv* env, jobject emulator) {
  std::map<std::tuple<int, bool>, Classifier*> classifierMap;

  // classifiers for packed canvas inference
  bool forFullFrame = false;
  for (const auto& inputSize : config.INPUT_SIZES) {
    std::unique_ptr<Classifier> classifier = std::make_unique<T>(
            config.DATASET, inputSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD,
            config.USE_TINY, forFullFrame);
    LOGD("Profiling %s %d size started", device == GPU ? "GPU" : "DSP", inputSize);
    time_us initialLatency = classifier->profileInferenceTime(config.PROFILE_WARMUPS,
                                                              config.PROFILE_RUNS);
    LOGD("Profiling %s %d size ended    // %lld", device == GPU ? "GPU" : "DSP", inputSize,
         initialLatency);
    classifier->setInferenceTime(initialLatency);
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
  LOGD("Profiling %s %d size started", device == GPU ? "GPU" : "DSP", inputSize);
  time_us initialLatency = classifier->profileInferenceTime(config.PROFILE_WARMUPS,
                                                            config.PROFILE_RUNS);
  LOGD("Profiling %s %d size ended    // %lld", device == GPU ? "GPU" : "DSP", inputSize,
       initialLatency);
  classifier->setInferenceTime(initialLatency);
  classifierMap[{inputSize, forFullFrame}] = classifier.get();
  classifiers.push_back(std::move(classifier));

  workers[device] = std::make_unique<Worker>(this, device, classifierMap,
                                             mConfig.DRAW_INFERENCE_RESULT, vm, env, emulator);
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
  auto boxTimeDevice = results.at(key);
  results.erase(key);
  return boxTimeDevice;
}

void InferenceEngine::enqueueResults(int key, const Result& boxTimes) {
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.emplace(key, boxTimes);
  resultLock.unlock();
  resultCv.notify_all();
}

std::map<Device, std::map<std::tuple<int, bool>, time_us>> InferenceEngine::getInferenceTimeTable() const {
  std::map<Device, std::map<std::tuple<int, bool>, time_us>> inferenceTimeTable;
  for (const auto&[device, worker] : workers) {
    inferenceTimeTable[device] = worker->getInferenceTimes();
  }
  return inferenceTimeTable;
}

std::vector<int> InferenceEngine::getInputSizes() const {
  return mConfig.INPUT_SIZES;
}

} // namespace rm
