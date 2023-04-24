#include "mondrian/Worker.hpp"

#include "mondrian/InferenceEngine.hpp"
#include "mondrian/Log.hpp"

namespace md {

Worker::Worker(InferenceEngine* engine, Device device,
               std::map<std::tuple<int, bool>, Classifier*> classifierMap,
               bool draw, JNIEnv* env, jobject app)
        : engine(engine), device(device), classifierMap(std::move(classifierMap)), isClosed(false),
          env(env), app(reinterpret_cast<jobject>(env->NewGlobalRef(app))),
          draw(draw) {
  if (draw) {
    env->GetJavaVM(&jvm);
    class_MondrianApp = reinterpret_cast<jclass>(env->NewGlobalRef(
            env->FindClass("hcs/offloading/mondrian/MondrianApp")));
    MondrianApp_drawOutput = env->GetMethodID(class_MondrianApp, "drawOutput", "(JLjava/util/List;)V");
    class_ArrayList = reinterpret_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/util/ArrayList")));
    ArrayList_init = env->GetMethodID(class_ArrayList, "<init>", "()V");
    ArrayList_add = env->GetMethodID(class_ArrayList, "add", "(ILjava/lang/Object;)V");
    class_BoundingBox = reinterpret_cast<jclass>(env->NewGlobalRef(
            env->FindClass("hcs/offloading/mondrian/BoundingBox")));
    BoundingBox_init = env->GetMethodID(class_BoundingBox, "<init>", "(IIIIFI)V");
  }

  thread = std::thread([this]() { work(); });
}

Worker::~Worker() {
  isClosed.store(true);
  cv.notify_all();
  thread.join();
}

void Worker::work() {
  while (!isClosed.load()) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return isClosed.load() || !inputs.empty(); });
    if (isClosed.load()) {
      lock.unlock();
      break;
    }
    auto[rgbMat, size, isFullFrame, key] = std::move(inputs.front());
    inputs.pop();
    lock.unlock();

    Result boxTimeDevice = classifierMap[{size, isFullFrame}]->recognizeImage(rgbMat);
    engine->enqueueResults(key, boxTimeDevice);
    if (draw) {
      drawInferenceResult(rgbMat, std::get<0>(boxTimeDevice));
    }
  }
}

void Worker::enqueue(const cv::Mat& rgbMat, int inputSize, bool isFullFrame, int key) {
  std::unique_lock<std::mutex> lock(mtx);
  inputs.push({rgbMat, inputSize, isFullFrame, key});
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

void Worker::drawInferenceResult(const cv::Mat& rgbMat, const std::vector<BoundingBox>& boxes) {
  if (jvm->AttachCurrentThread(&env, nullptr) != 0) {
    return;
  }
  jobject jBoxes = env->NewObject(class_ArrayList, ArrayList_init);
  for (int i = 0; i < boxes.size(); i++) {
    const md::BoundingBox& b = boxes.at(i);
    jobject box = env->NewObject(class_BoundingBox, BoundingBox_init,
                                 int(std::round(b.location.left)),
                                 int(std::round(b.location.top)),
                                 int(std::round(b.location.right)),
                                 int(std::round(b.location.bottom)),
                                 b.confidence, b.label);
    env->CallVoidMethod(jBoxes, ArrayList_add, i, box);
  }
  auto* jRgbMat = new cv::Mat();
  rgbMat.copyTo(*jRgbMat);
  env->CallVoidMethod(app, MondrianApp_drawOutput, (long) jRgbMat, jBoxes);
  jvm->DetachCurrentThread();
}

} // namespace md
