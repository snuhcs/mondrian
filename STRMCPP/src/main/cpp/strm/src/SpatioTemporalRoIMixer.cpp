#include "strm/SpatioTemporalRoIMixer.hpp"

#include <memory>
#include <utility>

namespace rm {

FrameBuffer::FrameBuffer(std::string key, int capacity, std::unique_ptr<Frame> firstFrame)
    : key(std::move(key)), capacity(capacity), begin(0), end(1) {
  frames.resize(capacity);
  frames[0] = std::move(firstFrame);
}

int FrameBuffer::enqueue(const cv::Mat& mat) {
  LOGD("FrameBuffer::enqueue() start");
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this]() { return end - begin < capacity; });
  Frame* prevFrame = frames[(end - 1) % capacity].get();
  frames[end % capacity] = std::make_unique<Frame>(
      key, end, mat, prevFrame, NowMicros());
  prevFrame->nextFrame = frames[end % capacity].get();
  LOGD("FrameBuffer::enqueue() end %d %d", begin, end);
  return end++;
}

void FrameBuffer::pop() {
  LOGD("FrameBuffer::pop() start");
  std::lock_guard<std::mutex> lock(mtx);
  frames[begin % capacity].reset();
  begin++;
  cv.notify_all();
  LOGD("FrameBuffer::pop() end");
}

Frame* FrameBuffer::getFrame(int frameIndex) {
  LOGD("FrameBuffer::getFrame(%d) start", frameIndex);
  std::lock_guard<std::mutex> lock(mtx);
  return frames[frameIndex % capacity].get();
  LOGD("FrameBuffer::getFrame(%d) end", frameIndex);
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

  mThread = std::thread([this]() { work(); });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mThread.join();
}

void SpatioTemporalRoIMixer::work() {
  LOGD("SpatioTemporalRoIMixer::work(), %lu us", NowMicros());
  time_us startTime, roiGettingTime;
  while (!mbStop) {
    startTime = NowMicros();
    std::vector<Frame*> frames = mRoIExtractor->getExtractedFrames();
    LOGD("Extracted frames: %lu", frames.size());
    if (frames.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(mConfig.LATENCY_SLO_MS / 2));
      continue;
    }
    roiGettingTime = NowMicros();

    // TODO: Handle leftInferenceTime = mConfig.LATENCY_SLO_MS - (roiGettingTime - startTime)
    std::vector<MixedFrame> mixedFrames = PatchMixer::pack(frames, mMixedFrameSize);
    std::vector<int> handles;
    std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                   [this](const MixedFrame& mixedFrame) {
                     return mInferenceEngine->enqueue(mixedFrame.packedMat, false);
                   });
    for (int i = 0; i < mixedFrames.size(); i++) {
      std::vector<BoundingBox> results = mInferenceEngine->getResults(handles[i]);
      mixedFrames[i].packedMat.release();
      mPatchReconstructor->reconstructResults(mixedFrames[i], results);
    }

    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    for (auto& mixedFrame : mixedFrames) {
      for (Frame* frame : mixedFrame.frames) {
        mResults[{frame->key, frame->frameIndex}] = frame->boxes;
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    for (auto& mixedFrame : mixedFrames) {
      std::map<std::string, Frame*> lastFrames;
      for (Frame* frame : mixedFrame.frames) {
        if (lastFrames.find(frame->key) == lastFrames.end() ||
            lastFrames.at(frame->key)->frameIndex < frame->frameIndex) {
          lastFrames[frame->key] = frame;
        }
      }
      for (auto& lastFrame : lastFrames) {
        lastFrame.second->updateBoxesToTrackWithInferenceResult();
      }
    }

    for (auto& mixedFrame : mixedFrames) {
      std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
      for (Frame* frame : mixedFrame.frames) {
        mFrameBuffers.at(frame->key)->pop();
      }
      framesLock.unlock();
    }

    std::this_thread::sleep_for(
        std::chrono::microseconds(mConfig.LATENCY_SLO_MS * 1000 / 2 - (NowMicros() - startTime)));
  }
}

int SpatioTemporalRoIMixer::enqueueImage(
    const std::string& key, const cv::Mat& mat) {
  LOGD("SpatioTemporalRoIMixer::enqueueImage(%s, Mat(%d, %d, %d))",
       key.substr(key.size() - 8).c_str(), mat.cols, mat.rows, mat.channels());
  assert(!mat.empty());
  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(key) == mFrameBuffers.end()) {
    LOGD("First Frame Inference Start: %s", key.c_str());
    std::vector<BoundingBox> firstBoxes = mInferenceEngine->getResults(
        mInferenceEngine->enqueue(mat, true));
    LOGD("First Frame Inference End  : %s, %lu Boxes", key.c_str(), firstBoxes.size());

    std::unique_ptr<Frame> firstFrame = std::make_unique<Frame>(key, 0, mat, nullptr, NowMicros());
    mRoIExtractor->preprocess(firstFrame.get());
    firstFrame->boxes = firstBoxes;
    firstFrame->updateBoxesToTrackWithInferenceResult();
    mFrameBuffers[key] = std::make_unique<FrameBuffer>(key, mConfig.BUFFER_SIZE, std::move(firstFrame));
    lock.unlock();

    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    mResults[{key, 0}] = std::move(firstBoxes);
    resultLock.unlock();
    mResultsCv.notify_all();
    return 0;
  } else {
    int frameIndex = mFrameBuffers.at(key)->enqueue(mat);
    mRoIExtractor->enqueue(mFrameBuffers.at(key)->getFrame(frameIndex));
    return frameIndex;
  }
}

std::vector<BoundingBox> SpatioTemporalRoIMixer::getResults(const std::string& key,
                                                            int frameIndex) {
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d) start",
       key.substr(key.size() - 8).c_str(), frameIndex);
  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  std::pair<std::string, int> resultKey = std::make_pair(key, frameIndex);
  auto it = mResults.find(resultKey);
  mResultsCv.wait(resultLock,
                  [this, &resultKey]() { return mResults.find(resultKey) != mResults.end(); });
  std::vector<BoundingBox> results = std::move(mResults.at(resultKey));
  mResults.erase(mResults.find(resultKey));
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d) end %lu",
       key.substr(key.size() - 8).c_str(), frameIndex, results.size());
  return results;
}

} // namespace rm
