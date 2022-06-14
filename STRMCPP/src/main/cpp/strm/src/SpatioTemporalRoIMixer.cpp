#include "strm/SpatioTemporalRoIMixer.hpp"

namespace rm {

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               const ResizeProfile* resizeProfile,
                                               const RoIPrioritizer* roIPrioritizer,
                                               InferenceEngine* inferenceEngine)
    : mLogger(new Logger("/data/data/hcs.offloading.edgedevicecpp/execution_log.csv")),
      mResizeProfile(resizeProfile),
      mRoIPrioritizer(roIPrioritizer),
      mInferenceEngine(inferenceEngine),
      mDispatcherConfig(config.dispatcherConfig),
      mRoIExtractorConfig(config.roIExtractorConfig) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mPatchReconstructor = std::make_unique<PatchReconstructor>(
      config.patchReconstructorConfig, inferenceEngine, (PatchReconstructorCallback*) this);
  mPatchMixer = std::make_unique<PatchMixer>(
      config.patchMixerConfig, inferenceEngine, mPatchReconstructor.get());
  mRoIExtractorConfig.MAX_MERGED_ROI_SIZE = *std::min_element(inferenceEngine->getInputSizes().begin(), inferenceEngine->getInputSizes().end());
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  mDispatchers.clear();
}

void SpatioTemporalRoIMixer::notifyMixedInferenceResults(const MixedFrame& mixedFrame) {
  LOGD("SpatioTemporalRoIMixer::notifyMixedInferenceResults");
  std::set<std::string> keys;
  for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
    keys.insert(frame->key);
  }
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  for (const std::string& key : keys) {
    if (mDispatchers.find(key) != mDispatchers.end()) {
      mDispatchers.at(key)->notifyResults();
    }
  }
}

int SpatioTemporalRoIMixer::enqueueImage(
    const std::string& key, const cv::Mat& mat) {
  LOGD("SpatioTemporalRoIMixer::enqueueImage(%s, Mat(%d, %d, %d))",
       key.c_str(), mat.cols, mat.rows, mat.channels());
  assert(!mat.empty());
  std::unique_lock<std::mutex> dispatchersLock(mDispatchersMtx);
  tryAddDispatcher(key);
  Dispatcher* dispatcher = mDispatchers.at(key).get();
  dispatchersLock.unlock();
  return dispatcher->enqueue(mat);
}

std::vector<BoundingBox>
SpatioTemporalRoIMixer::getResults(const std::string& key, int frameIndex) {
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d)", key.c_str(), frameIndex);
  std::unique_lock<std::mutex> dispatcherLock(mDispatchersMtx);
  tryAddDispatcher(key);
  Dispatcher* dispatcher = mDispatchers.at(key).get();
  dispatcherLock.unlock();
  return dispatcher->getResults(frameIndex);
}

void SpatioTemporalRoIMixer::removeSource(const std::string& key) {
  LOGD("SpatioTemporalRoIMixer::removeSource(%s)", key.c_str());
  std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
  mDispatchers.erase(mDispatchers.find(key));
}

void SpatioTemporalRoIMixer::tryAddDispatcher(const std::string& key) {
  if (mDispatchers.find(key) == mDispatchers.end()) {
    mDispatchers.insert(std::make_pair(key, std::make_unique<Dispatcher>(
            key, mDispatcherConfig, mRoIExtractorConfig,
            mResizeProfile, mRoIPrioritizer, mInferenceEngine,
            mPatchMixer.get(), mLogger.get())));
  }
}

} // namespace rm
