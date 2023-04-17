#include "strm/impl/Worker.hpp"

#include "strm/InferenceEngine.hpp"
#include "strm/Log.hpp"

namespace rm {

Worker::Worker(InferenceEngine* engine, Device device, std::map<std::tuple<int, bool>, Classifier*> classifierMap,
               bool draw, JavaVM* vm, JNIEnv* env, jobject emulator)
        : engine(engine), device(device), classifierMap(std::move(classifierMap)), isClosed(false),
          jvm(vm), env(env), emulator(reinterpret_cast<jobject>(env->NewGlobalRef(emulator))),
          draw(draw) {
  if (draw) {
    class_Emulator = reinterpret_cast<jclass>(env->NewGlobalRef(
            env->FindClass("hcs/offloading/strm/Emulator")));
    Emulator_drawOutput = env->GetMethodID(class_Emulator, "drawOutput", "(JLjava/util/List;)V");
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
  auto startTime = NowMicros();
  while (!isClosed.load()) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return isClosed.load() || !inputs.empty(); });
    if (isClosed.load()) {
      lock.unlock();
      break;
    }
    auto[mat, size, isFullFrame, key] = std::move(inputs.front());
    inputs.pop();
    lock.unlock();

    Result boxTimeDevice = classifierMap[{size, isFullFrame}]->recognizeImage(mat);
    engine->enqueueResults(key, boxTimeDevice);
    if (draw) {
      drawInferenceResult(mat, std::get<0>(boxTimeDevice));
    }
  }
}

void Worker::enqueue(const cv::Mat& mat, int inputSize, bool isFullFrame, int key) {
  std::unique_lock<std::mutex> lock(mtx);
  inputs.push({mat, inputSize, isFullFrame, key});
  lock.unlock();
  cv.notify_one();
}

std::map<std::tuple<int, bool>, time_us> Worker::getInferenceTimes() {
  std::map<std::tuple<int, bool>, time_us> timeTable;
  for (auto&[inputSize_isFullFrame, classifier] : classifierMap) {
    auto [inputSize, isFullFrame] = inputSize_isFullFrame;
    assert (classifier->getInferenceTime() > 0);
    timeTable[{inputSize, isFullFrame}] = classifier->getInferenceTime();
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
