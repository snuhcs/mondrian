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
    : mConfig(config), mHandle(0), jvm(vm), env(env),
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

  for (int i = 0; i < config.NUM_WORKERS; i++) {
    if (config.MODEL == "YOLO_V4" && config.RUNTIME == "MNN") {
      initClassifiers<MnnYoloV4Classifier>(config);
    } else if (config.MODEL == "YOLO_V4" && config.RUNTIME == "TFLITE") {
      initClassifiers<TfLiteYoloV4Classifier>(config);
    } else if (config.MODEL == "YOLO_V5" && config.RUNTIME == "TFLITE") {
//      initClassifiers<TfLiteYoloV5Classifier>(config);
      initClassifiers<TfLiteYoloV5ClassifierDSP>(config);
    } else {
      LOGE("Running %s model with %s runtime is not supported yet",
           config.MODEL.c_str(), config.RUNTIME.c_str());
    }
  }
}

template<typename T>
void InferenceEngine::initClassifiers(const InferenceEngineConfig& config) {
  std::map<int, Classifier*> classifierMap;
  for (const auto& device : config.DEVICES) {
    for (const auto& inputSize : config.INPUT_SIZES) {
      std::unique_ptr<Classifier> classifier = std::make_unique<T>(
          inputSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);
      classifier->setInferenceTimeMs(classifier->profileInferenceTime());
      classifierMap[inputSize] = classifier.get();
      classifiers.push_back(std::move(classifier));
    }
    workers[device] = std::make_unique<Worker>(this, classifierMap);
  }
}

int InferenceEngine::enqueue(const cv::Mat mat, Device device, int inputSize, int key) {
  std::unique_lock<std::mutex> inputLock(inputMtx);
  int handle = mHandle++;
  inputs.push(std::make_tuple(handle, mat, inputSize));
  inputLock.unlock();
  inputCv.notify_all();
  return handle;
}

std::pair<int, std::vector<BoundingBox>> InferenceEngine::getResults(const int handle) {
  std::unique_lock<std::mutex> resultLock(resultMtx);
  resultCv.wait(resultLock, [this, handle]() {
    return results.find(handle) != results.end();
  });
  std::vector<BoundingBox> boxes = results.at(handle);
  results.erase(results.find(handle));
  return {0, boxes};
}

std::tuple<int, const cv::Mat, const int> InferenceEngine::getInput() {
  std::unique_lock<std::mutex> inputLock(inputMtx);
  inputCv.wait(inputLock, [this]() {
    return !inputs.empty();
  });
  auto input = inputs.front();
  inputs.pop();
  return input;
}

void
InferenceEngine::enqueueResults(const int handle, const std::vector<BoundingBox>& boxes) {
  std::unique_lock<std::mutex> resultLock(resultMtx);
  results.emplace(handle, boxes);
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
                                 int(std::round(b.location.left)), int(std::round(b.location.top)), int(std::round(b.location.right)),
                                                                                                     int(std::round(b.location.bottom)),
                                 b.confidence, b.label, int(b.origin), (b.srcRoI == nullptr));
    env->CallVoidMethod(jBoxes, ArrayList_add, i, box);
  }
  auto* jMat = new cv::Mat();
  mat.copyTo(*jMat);
  env->CallVoidMethod(strm, SpatioTemporalRoIMixer_drawInferenceResult, (long) jMat, jBoxes);

  jvm->DetachCurrentThread();
}

std::map<Device, std::map<int,time_us>> InferenceEngine::getInferenceTimeUs() const {
  std::map<Device, std::map<int, time_us>> inferenceTimeTable;
  // TODO: fill inferenceTimeTable for each device, each input size (with time unit US)
  for (const auto& [device, worker] : workers) {
    inferenceTimeTable[device] = worker->getInferenceTimeUs();
  }
  return inferenceTimeTable;
}

std::vector<int> InferenceEngine::getInputSizes() const {
  return mConfig.INPUT_SIZES;
}

} // namespace rm
