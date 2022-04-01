#include "strm/SpatioTemporalRoIMixer.hpp"

#include <utility>

namespace rm {

void SpatioTemporalRoIMixer::onProcessEnd(MixedFrame& mixedFrame) {
  std::set<std::string> keys;
  for (Frame* frame : mixedFrame.packedFrames) {
    keys.insert(frame->key);
  }
  for (const std::string& key : keys) {
    mDispatchers.at(key)->notifyResults();
  }
}

void SpatioTemporalRoIMixer::enqueueImage(
        const std::string& key, int frameIndex, const cv::Mat* mat) {
  assert(mat != nullptr);
  std::lock_guard<std::mutex> dispatchersLock(mDispatchersMtx);
  if (mDispatchers.find(key) != mDispatchers.end()) {
    mDispatchers.at(key)->enqueue(new Frame(key, frameIndex, mat));
  }
}

std::vector<BoundingBox>
SpatioTemporalRoIMixer::getResults(const std::string& key, int frameIndex) {
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  if (mDispatchers.find(key) != mDispatchers.end()) {
    return mDispatchers.at(key)->getResults(frameIndex);
  }
  return {};
}

void SpatioTemporalRoIMixer::addSource(const std::string& key) {
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  if (mDispatchers.find(key) == mDispatchers.end()) {
    mDispatchers.insert(std::make_pair(key, std::make_unique<Dispatcher>(
            mDispatcherConfig, mRoIExtractorConfig,
            mResizeProfile, mRoIPrioritizer, mInferenceEngine,
            mPatchMixer.get())));
  }
}

void SpatioTemporalRoIMixer::removeSource(const std::string& key) {
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  mDispatchers.erase(mDispatchers.find(key));
}

void SpatioTemporalRoIMixer::close() {
  isClosed.store(true);
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  mDispatchers.clear();
}

} // namespace rm
