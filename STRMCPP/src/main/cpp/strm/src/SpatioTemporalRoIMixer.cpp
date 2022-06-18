#include "strm/SpatioTemporalRoIMixer.hpp"

#include <memory>
#include <utility>

#include "strm/Interpolator.hpp"

namespace rm {

FrameBuffer::FrameBuffer(const std::string& key, int capacity)
    : key(key), shortKey(key.substr(key.size() - 5, 1)), capacity(capacity), count(0) {
  frames.resize(capacity);
}

Frame* FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this, frameIndex]() { return frames[frameIndex % capacity].get() == nullptr; });
  Frame* prevFrame = frameIndex == 0 ? nullptr : frames[(frameIndex - 1) % capacity].get();
  frames[frameIndex % capacity] = std::make_unique<Frame>(key, frameIndex, mat, prevFrame,
                                                          NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[frameIndex % capacity].get();
  }
  Frame* currFrame = frames[frameIndex % capacity].get();
  lock.unlock();
  LOGD("FrameBuffer%s::enqueue (%d)", shortKey.c_str(), frameIndex);
  return currFrame;
}

void FrameBuffer::freeImage(const std::vector<int>& frameIndices) {
  std::unique_lock<std::mutex> lock(mtx);
  for (int frameIndex : frameIndices) {
    frames[frameIndex % capacity].reset();
  }
  lock.unlock();
  cv.notify_all();
  LOGD("FrameBuffer%s::freeImage(%2lu)", shortKey.c_str(), frameIndices.size());
}

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               ResizeProfile* resizeProfile,
                                               InferenceEngine* inferenceEngine,
                                               int numSourceVideo)
    : mConfig(config), mbStop(false),
      mLogger(new Logger("/data/data/hcs.offloading.edgedevicecpp/execution_log.csv")),
      mInferenceEngine(inferenceEngine),
      mNumSourceVideos(numSourceVideo),
      mRoIExtractor(new RoIExtractor(config.roIExtractorConfig, resizeProfile,
                                     inferenceEngine->getInputSizes()[0])),
      mPatchReconstructor(new PatchReconstructor(config.patchReconstructorConfig, resizeProfile)) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mThread = std::thread([this]() { work(); });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbStop = true;
  mThread.join();
}

void SpatioTemporalRoIMixer::work() {
  int scheduleID = 1;
  int fullFrameInferenceStreamIndex = 0;
  time_us scheduleInterval = mConfig.LATENCY_SLO_MS * 1000 / 2;
  time_us startTime, roiGettingTime, fullFrameInferenceTime, mixedFrameInferenceTime;
  time_us remainingTime;

  std::unique_lock<std::mutex> startLock(mStartMtx);
  mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mNumSourceVideos; });
  mStartMtx.unlock();
  std::this_thread::sleep_for(std::chrono::microseconds(scheduleInterval));

  while (!mbStop) {
    LOGD("========== Schedule %d Start ==========", scheduleID);
    startTime = NowMicros();
    std::map<std::string, SortedFrames> frames = mRoIExtractor->getExtractedFrames();
    roiGettingTime = NowMicros();

    std::stringstream ss;
    for (const auto& it : frames) {
      ss << it.first.substr(it.first.size() - 5, 1) << ": ";
      if (!it.second.empty()) {
        ss << (*it.second.begin())->frameIndex << " ~ " << (*it.second.rbegin())->frameIndex << ", ";
      }
    }
    LOGD("SpatioTemporalRoIMixer::work() getExtractedFrames() took %lu us  // %s",
         roiGettingTime - startTime, ss.str().c_str());

    if (frames.empty()) {
      if (scheduleInterval > roiGettingTime - startTime) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(scheduleInterval - (roiGettingTime - startTime)));
      }
      LOGD("========== Schedule %d End   ==========", scheduleID++);
      continue;
    }

    // Full Frame Inference
    Frame* fullFrameTarget = nullptr;
    if (scheduleID % mConfig.FULL_FRAME_INTERVAL) {
      fullFrameTarget = getFullFrameInferenceFrame(frames, fullFrameInferenceStreamIndex++);
      LOGD("Full Frame Target: %s, %d", fullFrameTarget->shortKey.c_str(), fullFrameTarget->frameIndex);
      fullFrameInference(fullFrameTarget);
    }
    fullFrameInferenceTime = NowMicros();
    remainingTime = scheduleInterval < (fullFrameInferenceTime - startTime) ? 0 :
                    scheduleInterval - (fullFrameInferenceTime - startTime);
    long long inferenceTimeUs = mInferenceEngine->getInferenceTimeMs() * 1000;
    LOGD(
        "SpatioTemporalRoIMixer::work() fullFrameInference() took %lu us, %lu us remains, inference takes %lld us",
        fullFrameInferenceTime - roiGettingTime, remainingTime, inferenceTimeUs);

    // Pack RoIs into mixed frames
    // TODO : handle numMixedFrames <= 0 case
    int numMixedFrames = remainingTime > inferenceTimeUs ? (int) (remainingTime / inferenceTimeUs) : 1;
    std::vector<MixedFrame> mixedFrames = PatchMixer::pack(
        frames, fullFrameTarget, mInferenceEngine->getInputSizes()[0], numMixedFrames);

    // Inference Mixed Frames
    std::vector<int> handles;
    std::transform(mixedFrames.begin(), mixedFrames.end(), std::back_inserter(handles),
                   [this](const MixedFrame& mixedFrame) {
                     return mInferenceEngine->enqueue(mixedFrame.packedMat);
                   });

    // Get Mixed Frame results sequentially
    SortedFrames processedFrames;
    for (int i = 0; i < mixedFrames.size(); i++) {
      std::vector<BoundingBox> results = mInferenceEngine->getResults(handles[i]);
      mixedFrames[i].packedMat.release();
      mPatchReconstructor->reconstructResults(mixedFrames[i], results);

      // Notify results of processed frames
      for (Frame* frame : mixedFrames[i].getPackedFrames()) {
        if (frame->isAllRoIPrepared() && processedFrames.find(frame) == processedFrames.end()) {
          processedFrames.insert(frame);
          frame->isBoxesReady = true;
        }
      }
      mRoIExtractor->notify();
    }

    // Interpolate results
    Interpolator::interpolate(frames);

    // Notify results of rest of the frames
    for (auto& it : frames) {
      for (Frame* frame : it.second) {
        if (frame != fullFrameTarget && processedFrames.find(frame) == processedFrames.end()) {
          frame->isBoxesReady = true;
        }
      }
    }
    mRoIExtractor->notify();

    // Update results for system output
    std::unique_lock<std::mutex> resultLock(mResultsMtx);
    for (const auto& it : frames) {
      for (Frame* frame : it.second) {
        if (frame != fullFrameTarget) {
          mResults[{frame->key, frame->frameIndex}] = frame->boxes;
        }
      }
    }
    resultLock.unlock();
    mResultsCv.notify_all();

    // Free frames for memory optimization
    std::unique_lock<std::mutex> framesLock(mFrameBuffersMtx);
    for (auto& it : frames) {
      if (it.second.empty()) {
        continue;
      }
      Frame* firstFrame = *it.second.begin();
      Frame* lastFrame = *it.second.rbegin();
      std::vector<int> frameIndices;
      if (firstFrame->prevFrame != nullptr) {
        frameIndices.push_back(firstFrame->prevFrame->frameIndex);
      }
      for (Frame* frame : it.second) {
        if (frame != lastFrame) {
          frameIndices.push_back(frame->frameIndex);
        }
      }
      mFrameBuffers.at(it.first)->freeImage(frameIndices);
    }
    framesLock.unlock();

    mixedFrameInferenceTime = NowMicros();
    LOGD("SpatioTemporalRoIMixer::work() %lu mixedFrameInferences took %lu us",
         mixedFrames.size(), mixedFrameInferenceTime - fullFrameInferenceTime);

    LOGD("========== Schedule %d End (getRoI %lu | full %lu | mix %lu) ==========", scheduleID++,
         roiGettingTime - startTime, fullFrameInferenceTime - roiGettingTime,
         mixedFrameInferenceTime - fullFrameInferenceTime);

    if (scheduleInterval > mixedFrameInferenceTime - startTime) {
      std::this_thread::sleep_for(
          std::chrono::microseconds(scheduleInterval - (mixedFrameInferenceTime - startTime)));
    }
  }
}

void SpatioTemporalRoIMixer::fullFrameInference(Frame* frame) {
  mRoIExtractor->preprocess(frame);
  std::vector<RoI> emptyRoIs;
  // TODO: use currFrame->childRoIs for ID mapping
  assert(frame->isFullFrameTarget);
  frame->boxes = mInferenceEngine->getResults(mInferenceEngine->enqueue(frame->mat));
  mPatchReconstructor->matchBoxesWithRoIs(true, frame->childRoIs, frame->boxes);
  frame->isBoxesReady = true;
  mRoIExtractor->notify();

  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  mResults[{frame->key, frame->frameIndex}] = frame->boxes;
  resultLock.unlock();
  mResultsCv.notify_all();
}

Frame* SpatioTemporalRoIMixer::getFullFrameInferenceFrame(
    const std::map<std::string, SortedFrames>& frames,
    int fullFrameInferenceStreamIndex) {
  std::vector<std::string> nonEmptyStreamKeys;
  for (const auto& it : frames) {
    if (!it.second.empty()) {
      nonEmptyStreamKeys.push_back(it.first);
    }
  }
  fullFrameInferenceStreamIndex %= (int) nonEmptyStreamKeys.size();
  return *frames.at(nonEmptyStreamKeys[fullFrameInferenceStreamIndex]).rbegin();
}

int SpatioTemporalRoIMixer::enqueueImage(const std::string& key, const cv::Mat& mat) {
  assert(!mat.empty());
  std::unique_lock<std::mutex> lock(mFrameBuffersMtx);
  if (mFrameBuffers.find(key) == mFrameBuffers.end()) {
    mFrameBuffers[key] = std::make_unique<FrameBuffer>(key, mConfig.BUFFER_SIZE);
  }
  FrameBuffer* frameBuffer = mFrameBuffers.at(key).get();
  lock.unlock();

  Frame* frame = frameBuffer->enqueue(mat);
  if (frame->frameIndex == 0) {
    mRoIExtractor->preprocess(frame);
    frame->useInferenceResultForOF = true;
    frame->isFullFrameTarget = true;
    fullFrameInference(frame);
  } else {
    if (frame->frameIndex == 1) {
      std::unique_lock<std::mutex> startLock(mStartMtx);
      mNumStartedFrameBuffers++;
      mStartCv.wait(startLock, [this]() { return mNumStartedFrameBuffers == mNumSourceVideos; });
      mStartMtx.unlock();
      mStartCv.notify_all();
      LOGD("Start %s video at %lu us", key.c_str(), NowMicros());
    }
    mRoIExtractor->enqueue(frame);
  }
  return frame->frameIndex;
}

std::vector<BoundingBox> SpatioTemporalRoIMixer::getResults(const std::string& key,
                                                            int frameIndex) {
  std::unique_lock<std::mutex> resultLock(mResultsMtx);
  std::pair<std::string, int> resultKey = std::make_pair(key, frameIndex);
  mResultsCv.wait(resultLock,
                  [this, &resultKey]() { return mResults.find(resultKey) != mResults.end(); });
  std::vector<BoundingBox> results = std::move(mResults.at(resultKey));
  mResults.erase(mResults.find(resultKey));
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %4d)    // %lu boxes",
       key.substr(key.size() - 5, 1).c_str(), frameIndex, results.size());
  return results;
}

} // namespace rm
