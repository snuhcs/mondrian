#ifndef SPATIO_TEMPORAL_ROI_MIXER_HPP_
#define SPATIO_TEMPORAL_ROI_MIXER_HPP_

#include <map>
#include <string>

#include "Config.hpp"
#include "ResizeProfiler.hpp"
#include "RoIPrioritizer.hpp"
#include "InferenceEngine.hpp"
#include "Dispatcher.hpp"
#include "RoIExtractor.hpp"
#include "PatchMixer.hpp"
#include "PatchReconstructor.hpp"

namespace rm {

class SpatioTemporalRoIMixer : public PatchReconstructorCallback {
 public:
  SpatioTemporalRoIMixer(const STRMConfig& config,
                         const ResizeProfiler* resizeProfiler,
                         const RoIPrioritizer* roIPrioritizer,
                         InferenceEngine* inferenceEngine);

  ~SpatioTemporalRoIMixer();

  void enqueueImage(const std::string& key, const cv::Mat mat);

  std::vector<BoundingBox> getResults(const std::string& key, int frameIndex);

  void removeSource(const std::string& key);

  void notifyMixedInferenceResults(const MixedFrame& mixedFrame) override;

 private:
  const ResizeProfiler* mResizeProfiler;
  const RoIPrioritizer* mRoIPrioritizer;
  InferenceEngine* mInferenceEngine;

  std::unique_ptr<PatchMixer> mPatchMixer;
  std::unique_ptr<PatchReconstructor> mPatchReconstructor;
  std::map<std::string, std::unique_ptr<Dispatcher>> mDispatchers;
  std::mutex mDispatchersMtx;

  const DispatcherConfig mDispatcherConfig;
  const RoIExtractorConfig mRoIExtractorConfig;
};

} // namespace rm

#endif // SPATIO_TEMPORAL_ROI_MIXER_HPP_
