#include "strm/SpatioTemporalRoIMixer.hpp"

#include <utility>

namespace rm {

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               const ResizeProfile* resizeProfile,
                                               const RoIPrioritizer* roIPrioritizer,
                                               InferenceEngine* inferenceEngine)
    : mResizeProfile(resizeProfile),
      mRoIPrioritizer(roIPrioritizer),
      mInferenceEngine(inferenceEngine),
      mDispatcherConfig(config.dispatcherConfig),
      mRoIExtractorConfig(config.roIExtractorConfig) {
  LOGD("SpatioTemporalRoIMixer()");
  mPatchReconstructor = std::make_unique<PatchReconstructor>(
      config.patchReconstructorConfig, inferenceEngine, (PatchReconstructorCallback*) this);
  mPatchMixer = std::make_unique<PatchMixer>(
      config.patchMixerConfig, inferenceEngine, mPatchReconstructor.get());
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  mDispatchers.clear();
}

void SpatioTemporalRoIMixer::onProcessEnd(const MixedFrame& mixedFrame) {
  LOGD("SpatioTemporalRoIMixer::onProcessEnd");
  std::set<std::string> keys;
  for (Frame* frame : mixedFrame.packedFrames) {
    keys.insert(frame->key);
  }
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  for (const std::string& key : keys) {
    if (mDispatchers.find(key) != mDispatchers.end()) {
      mDispatchers.at(key)->notifyResults();
    }
  }
}

void SpatioTemporalRoIMixer::enqueueImage(
    const std::string& key, const cv::Mat mat) {
  LOGD("SpatioTemporalRoIMixer::enqueueImage(%s, Mat(%d, %d, %d))",
       key.c_str(), mat.cols, mat.rows, mat.channels());
  assert(!mat.empty());
  std::unique_lock<std::mutex> dispatchersLock(mDispatchersMtx);
  if (mDispatchers.find(key) == mDispatchers.end()) {
    mDispatchers.insert(std::make_pair(key, std::make_unique<Dispatcher>(
        key, mDispatcherConfig, mRoIExtractorConfig,
        mResizeProfile, mRoIPrioritizer, mInferenceEngine,
        mPatchMixer.get())));
  }
  Dispatcher* dispatcher = mDispatchers.at(key).get();
  dispatchersLock.unlock();
  dispatcher->enqueue(mat);
}

std::vector<BoundingBox>
SpatioTemporalRoIMixer::getResults(const std::string& key, int frameIndex) {
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d)", key.c_str(), frameIndex);
  std::unique_lock<std::mutex> dispatcherLock(mDispatchersMtx);
  if (mDispatchers.find(key) == mDispatchers.end()) {
    mDispatchers.insert(std::make_pair(key, std::make_unique<Dispatcher>(
        key, mDispatcherConfig, mRoIExtractorConfig,
        mResizeProfile, mRoIPrioritizer, mInferenceEngine,
        mPatchMixer.get())));
  }
  Dispatcher* dispatcher = mDispatchers.at(key).get();
  dispatcherLock.unlock();
  return dispatcher->getResults(frameIndex);
}

void SpatioTemporalRoIMixer::removeSource(const std::string& key) {
  LOGD("SpatioTemporalRoIMixer::removeSource(%s)", key.c_str());
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  mDispatchers.erase(mDispatchers.find(key));
}

} // namespace rm
