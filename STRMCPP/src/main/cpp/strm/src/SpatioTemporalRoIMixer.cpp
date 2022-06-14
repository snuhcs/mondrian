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
      mInferenceEngine(inferenceEngine), mMixedFrameSize(inferenceEngine->getInputSize()),
      mRoIExtractor(new RoIExtractor(config.roIExtractorConfig, resizeProfile, mMixedFrameSize)) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mPatchReconstructor = std::make_unique<PatchReconstructor>(config.patchReconstructorConfig);

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
  time_us startTime, roiGettingTime;
  while (!mbStop) {
    startTime = NowMicros();
    std::vector<Frame*> frames = mRoIExtractor->getExtractedFrames();
    roiGettingTime = NowMicros();

    // TODO: Handle leftInferenceTime = mConfig.LATENCY_SLO_MS - (roiGettingTime - startTime)
    std::vector<MixedFrame> mixedFrames = PatchMixer::pack(frames, mMixedFrameSize);
    std::vector<int> handles(mixedFrames.size());
    std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                   [this](const MixedFrame& mixedFrame) {
                     return mInferenceEngine->enqueue(mixedFrame.packedMat, false);
                   });
    for (int i = 0; i < mixedFrames.size(); i++) {
      std::vector<BoundingBox> results = mInferenceEngine->getResults(handles[i]);
      mixedFrames[i].packedMat.release();
      mPatchReconstructor->reconstructResults(mixedFrames[i], results);
      {
        std::lock_guard<std::mutex> lock(mFrameBuffersMtx);
        for (Frame* frame : mixedFrames[i].frames) {
          mFrameBuffers.at(frame->key)->pop();
        }
      }
      {
        std::lock_guard<std::mutex> lock(mResultsMtx);
        for (Frame* frame : mixedFrames[i].frames) {
          mResults[{frame->key, frame->frameIndex}] = frame->boxes;
        }
      }
    }
    mResultsCv.notify_all();

    std::this_thread::sleep_for(
        std::chrono::microseconds(NowMicros() - startTime - mConfig.LATENCY_SLO_MS * 1000));
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
