#include "SpatioTemporalRoIMixer.hpp"

namespace rm {

void SpatioTemporalRoIMixer::onProcessEnd(const MixedFrame& mixedFrame) {
    std::map<std::string, std::vector<int>> groupedFrames;
    for (Frame* frame : mixedFrame.packedFrames) {
        groupedFrames.at(frame->key).push_back(frame->frameIndex);
    }
    for (const auto& kv : groupedFrames) {
        std::lock_guard<std::mutex> dispatchersLock(mDispatchersMtx);
        if (mDispatchers.find(kv.first) != mDispatchers.end()) {
            mDispatchers.at(kv.first)->notifyResults(kv.second);
        }
    }
}

void SpatioTemporalRoIMixer::enqueueImage(
        const std::string& key, int frameIndex, const cv::Mat* mat) {
    assert(mat != nullptr);
    std::lock_guard<std::mutex> dispatchersLock(mDispatchersMtx);
    if (mDispatchers.find(key) != mDispatchers.end()) {
        mDispatchers.at(key)->enqueue(Frame(key, frameIndex, mat));
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
        mDispatchers.insert(std::make_unique<Dispatcher>(
                mDispatcherConfig, mRoIExtractorConfig,
                mResizeProfile, mRoIPrioritizer, mInferenceEngine,
                mPatchMixer.get()));
    }
}

void SpatioTemporalRoIMixer::removeSource(const std::string& key) {
    std::lock_guard<std::mutex> dispatcherLock(mDispatchersMtx);
    mDispatchers.erase(mDispatchers.find(key));
}

void SpatioTemporalRoIMixer::close() {
    isClosed.store(true);
}

} // namespace rm
