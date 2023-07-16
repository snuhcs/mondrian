#include "mondrian/Worker.hpp"

#include "mondrian/InferenceEngine.hpp"
#include "mondrian/Log.hpp"

namespace md {

Worker::Worker(InferenceEngine* engine, Device device,
               std::map<std::pair<int, bool>, Classifier*> classifierMap,
               bool draw, JNIEnv* env, jobject app)
    : engine(engine), device(device), classifierMap_(std::move(classifierMap)), isClosed(false),
      env(env), app(reinterpret_cast<jobject>(env->NewGlobalRef(app))), draw(draw) {
  if (draw) {
    maxPackedCanvasSize = (*classifierMap_.rbegin()).first.first;
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
    // Prepare input
    time_us start = NowMicros();
    auto[rgbMat, size, isFullFrame, key] = std::move(inputs.front());
    inputs.pop();
    lock.unlock();

    // Inference
    time_us inferenceStart = NowMicros();
    auto boxes = classifierMap_[{size, isFullFrame}]->recognizeImage(rgbMat);
    time_us inferenceEnd = NowMicros();

    // Enqueue & draw result
    engine->enqueueResult(key, {boxes, {inferenceStart, inferenceEnd}, device});
    if (draw) {
      drawInferenceResult(rgbMat, boxes, isFullFrame);
    }
    time_us end = NowMicros();

    // Update latency
    time_us origLatency = latencyMap_[{size, isFullFrame}];
    time_us newLatency = end - start;
    time_us estimatedLatency = (3 * newLatency + 7 * origLatency) / 10;
    latencyMap_[{size, isFullFrame}] = estimatedLatency;
  }
}

void Worker::enqueue(const cv::Mat& rgbMat, int inputSize, bool isFullFrame, int key) {
  std::unique_lock<std::mutex> lock(mtx);
  inputs.push({rgbMat, inputSize, isFullFrame, key});
  lock.unlock();
  cv.notify_one();
}

void Worker::profileLatency(int warmupRuns, int numRuns) {
  for (auto& it: classifierMap_) {
    auto&[size, isFullFrame] = it.first;
    auto* classifier = it.second;
    for (int i = 0; i < warmupRuns; i++) {
      cv::Mat rgbMat(size, size, CV_8UC3);
      classifier->recognizeImage(rgbMat);
    }
    cv::Mat rgbMat(size, size, CV_8UC3);
    time_us total = 0;
    for (int i = 0; i < numRuns; i++) {
      time_us start = NowMicros();
      classifier->recognizeImage(rgbMat);
      time_us end = NowMicros();
      total += end - start;
    }
    time_us avg = total / numRuns;
    latencyMap_[{size, isFullFrame}] = avg;
    LOGV("Profiling latency (%dx%d, %s) : %lld",
         size, size, isFullFrame ? "Full" : "Pack", avg);
  }
}

void Worker::drawInferenceResult(const cv::Mat& rgbMat,
                                 const std::vector<BoundingBox>& boxes,
                                 bool isFullFrame) {
  if (jvm->AttachCurrentThread(&env, nullptr) != 0) {
    return;
  }
  jobject jBoxes = env->NewObject(class_ArrayList, ArrayList_init);
  for (int i = 0; i < boxes.size(); i++) {
    const md::BoundingBox& b = boxes.at(i);
    jobject box = env->NewObject(class_BoundingBox, BoundingBox_init,
                                 int(std::round(b.loc.l)),
                                 int(std::round(b.loc.t)),
                                 int(std::round(b.loc.r)),
                                 int(std::round(b.loc.b)),
                                 b.confidence, b.label);
    env->CallVoidMethod(jBoxes, ArrayList_add, i, box);
  }
  if (isFullFrame) {
    auto* jRgbMat = new cv::Mat();
    rgbMat.copyTo(*jRgbMat);
    env->CallVoidMethod(app, MondrianApp_drawOutput, (long) jRgbMat, jBoxes);
  } else {
    assert(rgbMat.rows <= maxPackedCanvasSize && rgbMat.cols <= maxPackedCanvasSize);
    auto* jRgbMat = new cv::Mat(maxPackedCanvasSize, maxPackedCanvasSize,
                                CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Rect rect(0, 0, rgbMat.cols, rgbMat.rows);
    rgbMat.copyTo((*jRgbMat)(rect));
    env->CallVoidMethod(app, MondrianApp_drawOutput, (long) jRgbMat, jBoxes);
  }
  jvm->DetachCurrentThread();
}

} // namespace md
