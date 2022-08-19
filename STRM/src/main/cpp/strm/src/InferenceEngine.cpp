#include "strm/InferenceEngine.hpp"

#include "strm/Log.hpp"
#include "strm/impl/Worker.hpp"
#include "strm/impl/models/MnnYoloV4Classifier.hpp"
#include "strm/impl/models/TfLiteYoloV4Classifier.hpp"
#include "strm/impl/models/TfLiteYoloV5Classifier.hpp"
#include "strm/impl/models/TfLiteYoloV5ClassifierDSP.hpp"

#include <cmath>

namespace rm {

InferenceEngine::InferenceEngine(
    const InferenceEngineConfig& config, JavaVM* vm, JNIEnv* env, jobject strm)
    : mConfig(config), jvm(vm), env(env),
      strm(reinterpret_cast<jobject>(env->NewGlobalRef(strm))) {
  class_SpatioTemporalRoIMixer = reinterpret_cast<jclass>(env->NewGlobalRef(
      env->FindClass("hcs/offloading/strm/Emulator")));
  SpatioTemporalRoIMixer_drawInferenceResult = env->GetMethodID(
      class_SpatioTemporalRoIMixer, "drawInferenceResult", "(JLjava/util/List;)V");
  class_ArrayList = reinterpret_cast<jclass>(env->NewGlobalRef(
      env->FindClass("java/util/ArrayList")));
  ArrayList_init = env->GetMethodID(class_ArrayList, "<init>", "()V");
  ArrayList_add = env->GetMethodID(class_ArrayList, "add", "(ILjava/lang/Object;)V");
  class_BoundingBox = reinterpret_cast<jclass>(env->NewGlobalRef(
      env->FindClass("hcs/offloading/strm/BoundingBox")));
  BoundingBox_init = env->GetMethodID(class_BoundingBox, "<init>", "(IIIIIFIIZ)V");

  for (Device device: config.DEVICES) {
    if (device == GPU) {
      if (config.MODEL == "YOLO_V4" && config.RUNTIME == "MNN") {
        addClassifiers<MnnYoloV4Classifier>(device, config);
      } else if (config.MODEL == "YOLO_V4" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV4Classifier>(device, config);
      } else if (config.MODEL == "YOLO_V5" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV5Classifier>(device, config);
      } else {
        LOGE("Running %s model with %s runtime on GPU is not supported yet",
             config.MODEL.c_str(), config.RUNTIME.c_str());
      }
    } else if (device == DSP) {
      if (config.MODEL == "YOLO_V5" && config.RUNTIME == "TFLITE") {
        addClassifiers<TfLiteYoloV5ClassifierDSP>(device, config);
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
void InferenceEngine::addClassifiers(Device device, const InferenceEngineConfig& config) {
  std::map<int, Classifier*> classifierMap;
  for (const auto& inputSize : config.INPUT_SIZES) {
    std::unique_ptr<Classifier> classifier = std::make_unique<T>(
        inputSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);
    LOGD("Profiling %s %d size started", device == GPU ? "GPU" : "DSP", inputSize);
    time_us initialLatency = classifier->profileInferenceTime();
    LOGD("Profiling %s %d size ended    // %lld", device == GPU ? "GPU" : "DSP", inputSize,
         initialLatency);
    classifier->setInferenceTime(initialLatency);
    classifierMap[inputSize] = classifier.get();
    classifiers.push_back(std::move(classifier));
  }
  workers[device] = std::make_unique<Worker>(this, device, classifierMap);
}

void InferenceEngine::enqueue(const cv::Mat mat, Device device, int inputSize, int key) {
  workers[device]->enqueue(mat, inputSize, key);
}

std::vector<BoundingBox> InferenceEngine::getResults(int key) {
  std::unique_lock<std::mutex> resultLock(resultMtx);
  resultCv.wait(resultLock, [this, key]() {
    return results.find(key) != results.end();
  });
  std::vector<BoundingBox> boxes = results.at(key);
  results.erase(key);
  return boxes;
}

void InferenceEngine::enqueueResults(int key, const std::vector<BoundingBox>& boxes) {
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.emplace(key, boxes);
  resultLock.unlock();
  resultCv.notify_all();
}

void InferenceEngine::drawInferenceResult(const cv::Mat& mat,
                                          const std::vector<BoundingBox>& boxes) {
  if (!mConfig.DRAW_INFERENCE_RESULT) {
    return;
  }
  if (jvm->AttachCurrentThread(&env, nullptr) != 0) {
    return;
  }

  jobject jBoxes = env->NewObject(class_ArrayList, ArrayList_init);
  for (int i = 0; i < boxes.size(); i++) {
    const rm::BoundingBox& b = boxes.at(i);
    jobject box = env->NewObject(class_BoundingBox, BoundingBox_init,
                                 b.id,
                                 int(std::round(b.location.left)), int(std::round(b.location.top)),
                                 int(std::round(b.location.right)),
                                 int(std::round(b.location.bottom)),
                                 b.confidence, b.label, int(b.origin), (b.srcRoI == nullptr));
    env->CallVoidMethod(jBoxes, ArrayList_add, i, box);
  }
  auto* jMat = new cv::Mat();
  mat.copyTo(*jMat);
  env->CallVoidMethod(strm, SpatioTemporalRoIMixer_drawInferenceResult, (long) jMat, jBoxes);

  jvm->DetachCurrentThread();
}

std::map<Device, std::map<int, time_us>> InferenceEngine::getInferenceTimeTable() const {
  std::map<Device, std::map<int, time_us>> inferenceTimeTable;
  // TODO: fill inferenceTimeTable for each device, each input size (with time unit us)
  for (const auto&[device, worker] : workers) {
    inferenceTimeTable[device] = worker->getInferenceTimes();
  }
  return inferenceTimeTable;
}

std::vector<int> InferenceEngine::getInputSizes() const {
  return mConfig.INPUT_SIZES;
}

} // namespace rm
