#ifndef SPATIO_TEMPORAL_ROI_MIXER_HPP_
#define SPATIO_TEMPORAL_ROI_MIXER_HPP_

#include <atomic>
#include <map>
#include <memory>
#include <string>

#include "Config.hpp"
#include "Log.hpp"
#include "ResizeProfile.hpp"
#include "RoIPrioritizer.hpp"
#include "InferenceEngine.hpp"
#include "Dispatcher.hpp"
#include "RoIExtractor.hpp"
#include "PatchMixer.hpp"
#include "PatchReconstructor.hpp"

namespace rm {

class SpatioTemporalRoIMixer : PatchReconstructorCallback {
 public:
  SpatioTemporalRoIMixer(const STRMConfig& config,
                         ResizeProfile* resizeProfile,
                         RoIPrioritizer* roIPrioritizer,
                         InferenceEngine* inferenceEngine)
          : mResizeProfile(std::move(resizeProfile)),
            mRoIPrioritizer(std::move(roIPrioritizer)),
            mInferenceEngine(std::move(inferenceEngine)),
            mDispatcherConfig(config.dispatcherConfig),
            mRoIExtractorConfig(config.roIExtractorConfig),
            isClosed(false) {
    LOGD("SpatioTemporalRoIMixer()");
    mPatchReconstructor = std::make_unique<PatchReconstructor>(
            config.patchReconstructorConfig, inferenceEngine, (PatchReconstructorCallback*) this);
    mPatchMixer = std::make_unique<PatchMixer>(
            config.patchMixerConfig, inferenceEngine, mPatchReconstructor.get());
  }

  ~SpatioTemporalRoIMixer() {
    delete mResizeProfile;
    delete mRoIPrioritizer;
    delete mInferenceEngine;
  }

  void enqueueImage(const std::string& key, int frameIndex, const cv::Mat* mat);

  std::vector<BoundingBox> getResults(const std::string& key, int frameIndex);

  void removeSource(const std::string& key);

  void close();

  void onProcessEnd(MixedFrame& mixedFrame) override;

 private:
  std::atomic_bool isClosed;

  ResizeProfile* mResizeProfile;
  RoIPrioritizer* mRoIPrioritizer;
  InferenceEngine* mInferenceEngine;

  std::unique_ptr<PatchMixer> mPatchMixer;
  std::unique_ptr<PatchReconstructor> mPatchReconstructor;
  std::map<std::string, std::unique_ptr<Dispatcher>> mDispatchers;
  std::mutex mDispatchersMtx;

  const DispatcherConfig mDispatcherConfig;
  const RoIExtractorConfig mRoIExtractorConfig;
};

}

#endif // SPATIO_TEMPORAL_ROI_MIXER_HPP_
