#include "mondrian/Worker.hpp"

#include "mondrian/InferenceEngine.hpp"
#include "mondrian/Log.hpp"

namespace md {

Worker::Worker(InferenceEngine* engine,
               Device device,
               std::map<std::pair<int, bool>, Classifier*> classifierMap,
               bool draw,
               JNIEnv* env,
               jobject app)
    : engine_(engine),
      device_(device),
      classifierMap_(std::move(classifierMap)),
      estimatedEndTime_(NowMicros()),
      stop_(false),
      env_(env),
      app_(reinterpret_cast<jobject>(env->NewGlobalRef(app))),
      draw_(draw) {
  if (draw) {
    maxPackedCanvasSize_ = (*classifierMap_.rbegin()).first.first;
    env_->GetJavaVM(&jvm_);
    class_MondrianApp_ = reinterpret_cast<jclass>(env_->NewGlobalRef(env_->FindClass(
        "hcs/offloading/mondrian/MondrianApp")));
    MondrianApp_drawOutput_ = env_->GetMethodID(
        class_MondrianApp_, "drawOutput", "(JLjava/util/List;J)V");
    class_ArrayList_ = reinterpret_cast<jclass>(
        env_->NewGlobalRef(env_->FindClass("java/util/ArrayList")));
    ArrayList_init_ = env_->GetMethodID(class_ArrayList_, "<init>", "()V");
    ArrayList_add_ = env_->GetMethodID(class_ArrayList_, "add", "(ILjava/lang/Object;)V");
    class_BoundingBox_ = reinterpret_cast<jclass>(
        env_->NewGlobalRef(env_->FindClass("hcs/offloading/mondrian/BoundingBox")));
    BoundingBox_init_ = env_->GetMethodID(class_BoundingBox_, "<init>", "(IIIIFI)V");
  }

  thread_ = std::thread([this]() { work(); });
}

Worker::~Worker() {
  stop_ = true;
  cv_.notify_all();
  thread_.join();
}

void Worker::work() {
  while (!stop_) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this]() { return stop_ || !inputs_.empty(); });
    if (stop_) break;

    // Prepare input
    time_us start = NowMicros();
    auto [rgbMat, size, isFullFrame, key] = std::move(inputs_.front());
    inputs_.pop_front();
    lock.unlock();

    // Inference
    time_us inferenceStart = NowMicros();
    auto boxes = classifierMap_[{size, isFullFrame}]->recognizeImage(rgbMat);
    time_us inferenceEnd = NowMicros();

    // Enqueue & draw result
    engine_->enqueueResult(key, {boxes, {inferenceStart, inferenceEnd}, device_});
    if (draw_) {
      drawInferenceResult(rgbMat, boxes, isFullFrame);
    }
    time_us end = NowMicros();

    updateLatency(size, isFullFrame, end - start);
    updateRemainingTime();
  }
}

void Worker::updateLatency(int size, bool isFullFrame, md::time_us newLatency) {
  std::lock_guard<std::mutex> lock(mtx_);
  latencyMap_[{size, isFullFrame}] = (
      3 * newLatency + 7 * latencyMap_[{size, isFullFrame}]
  ) / 10;
}

void Worker::updateRemainingTime() {
  std::lock_guard<std::mutex> lock(mtx_);
  estimatedEndTime_ = NowMicros();
  for (const auto& input : inputs_) {
    estimatedEndTime_ += latencyMap_[{std::get<1>(input), std::get<2>(input)}];
  }
}

void Worker::enqueue(const cv::Mat& rgbMat,
                     const int inputSize,
                     const bool isFullFrame,
                     const Key key) {
  std::unique_lock<std::mutex> lock(mtx_);
  inputs_.emplace_back(rgbMat, inputSize, isFullFrame, key);
  estimatedEndTime_ += latencyMap_[{inputSize, isFullFrame}];
  lock.unlock();
  cv_.notify_one();
}

std::map<std::pair<int, bool>, time_us> Worker::latencyMap() {
  std::lock_guard<std::mutex> lock(mtx_);
  return latencyMap_;
}

time_us Worker::remainingTime() {
  std::lock_guard<std::mutex> lock(mtx_);
  return estimatedEndTime_ - NowMicros();
}

void Worker::profileLatency(int warmupRuns, int numRuns) {
  for (auto& it : classifierMap_) {
    auto& [size, isFullFrame] = it.first;
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
  }
}

void Worker::drawInferenceResult(const cv::Mat& rgbMat,
                                 const std::vector<BoundingBox>& boxes,
                                 const bool isFullFrame) {
  if (jvm_->AttachCurrentThread(&env_, nullptr) != 0) {
    return;
  }
  jobject jBoxes = env_->NewObject(class_ArrayList_, ArrayList_init_);
  for (int i = 0; i < boxes.size(); i++) {
    const md::BoundingBox& b = boxes.at(i);
    jobject box = env_->NewObject(
        class_BoundingBox_, BoundingBox_init_,
        int(std::round(b.loc.l)),
        int(std::round(b.loc.t)),
        int(std::round(b.loc.r)),
        int(std::round(b.loc.b)),
        b.confidence,
        b.label);
    env_->CallVoidMethod(jBoxes, ArrayList_add_, i, box);
  }
  if (isFullFrame) {
    auto* jRgbMat = new cv::Mat();
    rgbMat.copyTo(*jRgbMat);
    env_->CallVoidMethod(app_, MondrianApp_drawOutput_, (long) jRgbMat, jBoxes, (long) device_);
  } else {
    assert(rgbMat.rows <= maxPackedCanvasSize_ && rgbMat.cols <= maxPackedCanvasSize_);
    auto* jRgbMat = new cv::Mat(maxPackedCanvasSize_, maxPackedCanvasSize_,
                                CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Rect rect(0, 0, rgbMat.cols, rgbMat.rows);
    rgbMat.copyTo((*jRgbMat)(rect));
    env_->CallVoidMethod(app_, MondrianApp_drawOutput_, (long) jRgbMat, jBoxes, (long) device_);
  }
  jvm_->DetachCurrentThread();
}

} // namespace md
