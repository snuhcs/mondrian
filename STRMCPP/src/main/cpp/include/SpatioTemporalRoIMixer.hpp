#ifndef SPATIO_TEMPORAL_ROI_MIXER_HPP_
#define SPATIO_TEMPORAL_ROI_MIXER_HPP_

#include <atomic>
#include <map>
#include <string>

#include "Config.hpp"
#include "ResizeProfile.hpp"
#include "RoIPrioritizer.hpp"
#include "InferenceEngine.hpp"
#include "Dispatcher.hpp"
#include "RoIExtractor.hpp"
#include "PatchMixer.hpp"
#include "PatchReconstructor.hpp"

namespace rm {

class SpatioTemporalRoIMixer : ConsumerCallback<MixedFrame> {
public:
    SpatioTemporalRoIMixer(const STRMConfig& config,
                           ResizeProfile* resizeProfile,
                           RoIPrioritizer* roIPrioritizer,
                           InferenceEngine* inferenceEngine)
                           : mResizeProfile(resizeProfile),
                             mRoIPrioritizer(roIPrioritizer),
                             mInferenceEngine(inferenceEngine),
                             mPatchReconstructor(std::make_unique<PatchReconstructor>(
                                     config.patchReconstructorConfig, inferenceEngine, (ConsumerCallback<MixedFrame>*) this)),
                             mPatchMixer(std::make_unique<PatchMixer>(
                                     config.patchMixerConfig, inferenceEngine, &mPatchReconstructor)),
                             mDispatcherConfig(config.dispatcherConfig),
                             mRoIExtractorConfig(config.roIExtractorConfig),
                             isClosed(false) {}

    void enqueueImage(const std::string& key, int frameIndex, const cv::Mat* mat);
    std::vector<BoundingBox> getResults(const std::string& key, int frameIndex);
    void addSource(const std::string& key);
    void removeSource(const std::string& key);
    void close();
private:
    void onProcessEnd(const MixedFrame& mixedFrame) override;

    std::atomic_bool isClosed;

    const ResizeProfile* mResizeProfile;
    const RoIPrioritizer* mRoIPrioritizer;
    const InferenceEngine* mInferenceEngine;

    const std::unique_ptr<PatchMixer> mPatchMixer;
    const std::unique_ptr<PatchReconstructor> mPatchReconstructor;
    std::map<std::string, std::unique_ptr<Dispatcher>> mDispatchers;
    std::mutex mDispatchersMtx;

    const DispatcherConfig mDispatcherConfig;
    const RoIExtractorConfig mRoIExtractorConfig;
};

}

#endif // SPATIO_TEMPORAL_ROI_MIXER_HPP_
