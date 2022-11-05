#include "strm/impl/Worker.hpp"

#include "strm/Log.hpp"
#include "strm/InferenceEngine.hpp"

namespace rm {

Worker::Worker(InferenceEngine* engine, Device device, std::map<int, Classifier*> classifierMap,
               bool draw, JavaVM* vm, JNIEnv* env, jobject emulator)
    : engine(engine), device(device), classifierMap(std::move(classifierMap)), isClosed(false),
      jvm(vm), env(env), emulator(reinterpret_cast<jobject>(env->NewGlobalRef(emulator))),
      draw(draw) {
  if (draw) {
    class_Emulator = reinterpret_cast<jclass>(env->NewGlobalRef(
        env->FindClass("hcs/offloading/strm/Emulator")));
    assert(device == GPU || device == DSP);
    if (device == GPU) {
      Emulator_drawOutput = env->GetMethodID(
          class_Emulator, "drawOutput0", "(JLjava/util/List;)V");
    } else {
      Emulator_drawOutput = env->GetMethodID(
          class_Emulator, "drawOutput1", "(JLjava/util/List;)V");
    }
    class_ArrayList = reinterpret_cast<jclass>(env->NewGlobalRef(
        env->FindClass("java/util/ArrayList")));
    ArrayList_init = env->GetMethodID(class_ArrayList, "<init>", "()V");
    ArrayList_add = env->GetMethodID(class_ArrayList, "add", "(ILjava/lang/Object;)V");
    class_BoundingBox = reinterpret_cast<jclass>(env->NewGlobalRef(
        env->FindClass("hcs/offloading/strm/BoundingBox")));
    BoundingBox_init = env->GetMethodID(class_BoundingBox, "<init>", "(IIIIIFIIZ)V");
  }

  thread = std::thread([this]() { work(); });
}

Worker::~Worker() {
  isClosed.store(true);
  cv.notify_all();
  thread.join();
}

void Worker::work() {
  std::thread::id id = std::this_thread::get_id();
  auto startTime = NowMicros();
  while (!isClosed.load()) {
    LOGD("Worker::work() before acquire : %lld", NowMicros() - startTime);
    std::unique_lock<std::mutex> lock(mtx);
    LOGD("Worker::work() after  acquire : %lld", NowMicros() - startTime);
    cv.wait(lock, [this]() { return isClosed.load() || !inputs.empty(); });
    LOGD("Worker::work() after  enqueued: %lld", NowMicros() - startTime);
    if (isClosed.load()) {
      lock.unlock();
      break;
    }
    auto[mat, size, key] = std::move(inputs.front());
    inputs.pop();
    lock.unlock();
    LOGD("Worker::work() after  unlock  : %lld", NowMicros() - startTime);

    Result boxTimeDevice = classifierMap[size]->recognizeImage(mat);
    LOGD("Worker::work() recognizeImage : %lld", NowMicros() - startTime);
    engine->enqueueResults(key, boxTimeDevice);
    LOGD("Worker::work() enqueue Results: %lld", NowMicros() - startTime);
    if (draw) {
      drawInferenceResult(mat, std::get<0>(boxTimeDevice));
    }
    LOGD("Worker::work() draw Result    : %lld", NowMicros() - startTime);
  }
}

void Worker::enqueue(const cv::Mat& mat, int inputSize, int key) {
  std::unique_lock<std::mutex> lock(mtx);
  inputs.push({mat, inputSize, key});
  lock.unlock();
  cv.notify_one();
}

std::map<int, time_us> Worker::getInferenceTimes() {
  std::map<int, time_us> timeTable;
  for (auto&[inputSize, classifier] : classifierMap) {
    assert (classifier->getInferenceTime() > 0);
    timeTable[inputSize] = classifier->getInferenceTime();
  }
  return timeTable;
}

void Worker::drawInferenceResult(const cv::Mat& mat, const std::vector<BoundingBox>& boxes) {
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
  env->CallVoidMethod(emulator, Emulator_drawOutput, (long) jMat, jBoxes);
  jvm->DetachCurrentThread();
}

} // namespace rm
