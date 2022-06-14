#include "strm/SpatioTemporalRoIMixer.hpp"

#include <memory>
#include <utility>

namespace rm {

FrameBuffer::FrameBuffer(const std::string& key, int capacity,
                         const cv::Mat& firstMat, const std::vector<BoundingBox>& firstBoxes)
    : key(key), capacity(capacity), begin(0), end(1) {
  frames.resize(capacity);
  frames[0] = std::make_unique<Frame>(
      key, 0, firstMat, nullptr, NowMicros());
  frames[0]->boxes = firstBoxes;
  frames[0]->isResultReady = true;
}

int FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this]() { return end - begin < capacity; });
  Frame* prevFrame = getFrame(end = 1);
  frames[end % capacity] = std::make_unique<Frame>(
      key, end, mat, prevFrame, NowMicros());
  prevFrame->nextFrame = frames[end % capacity].get();
  return end++;
}

void FrameBuffer::pop() {
  std::lock_guard<std::mutex> lock(mtx);
  frames[begin % capacity].reset();
  begin++;
  cv.notify_one();
}

Frame* FrameBuffer::getFrame(int frameIndex) {
  std::lock_guard<std::mutex> lock(mtx);
  return frames[frameIndex % capacity].get();
}

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               const ResizeProfile* resizeProfile,
                                               InferenceEngine* inferenceEngine)
    : mConfig(config), mbStop(false),
      mLogger(new Logger("/data/data/hcs.offloading.edgedevicecpp/execution_log.csv")),
      mInferenceEngine(inferenceEngine),
      mRoIExtractor(new RoIExtractor(config.roIExtractorConfig, resizeProfile)) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mPatchReconstructor = std::make_unique<PatchReconstructor>(config.patchReconstructorConfig,
                                                             inferenceEngine);
  mPatchMixer = std::make_unique<PatchMixer>(config.patchMixerConfig, inferenceEngine,
                                             mPatchReconstructor.get());

  mThread = std::thread([this]() {
    LOGD("SpatioTemporalRoIMixer::work() start at %lu us", NowMicros());
    work();
  });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mThread.join();
}

void SpatioTemporalRoIMixer::work() {
  std::this_thread::sleep_for(std::chrono::milliseconds(mConfig.LATENCY_SLO_MS));
  time_us startTime = NowMicros();
  time_us roiCollectionTime;
  time_us leftInferenceTime;
  while (!mbStop) {
    std::vector<RoI> rois = mRoIExtractor->getRoIs();
    roiCollectionTime = NowMicros();
    leftInferenceTime = mConfig.LATENCY_SLO_MS - (roiCollectionTime - startTime);
    startTime = roiCollectionTime;

//     std::vector<MixedFrame> mixedFrames = mPatchMixer.schedule(rois, leftInferenceTime);
  }
}

int SpatioTemporalRoIMixer::enqueueImage(
    const std::string& key, const cv::Mat& mat) {
  LOGD("SpatioTemporalRoIMixer::enqueueImage(%s, Mat(%d, %d, %d))",
       key.c_str(), mat.cols, mat.rows, mat.channels());
  assert(!mat.empty());
  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(key) == mFrameBuffers.end()) {
    std::vector<BoundingBox> firstBoxes = mInferenceEngine->getResults(
        mInferenceEngine->enqueue(mat, true));
    mFrameBuffers[key] = std::make_unique<FrameBuffer>(key, mConfig.BUFFER_SIZE, mat, firstBoxes);
    lock.unlock();

    std::lock_guard<std::mutex> resultLock(mResultsMtx);
    mResults[std::make_pair(key, 0)] = std::move(firstBoxes);
    mResultsCv.notify_all();
    return 0;
  } else {
    return mFrameBuffers.at(key)->enqueue(mat);
  }
}

std::vector<BoundingBox> SpatioTemporalRoIMixer::getResults(const std::string& key,
                                                            int frameIndex) {
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d)", key.c_str(), frameIndex);
  std::unique_lock<std::mutex> lock(mResultsMtx);
  std::pair<std::string, int> resultKey = std::make_pair(key, frameIndex);
  auto it = mResults.find(resultKey);
  mResultsCv.wait(lock,
                  [this, &resultKey]() { return mResults.find(resultKey) != mResults.end(); });
  std::vector<BoundingBox> results = std::move(mResults.at(resultKey));
  mResults.erase(mResults.find(resultKey));
  return results;
}

} // namespace rm
