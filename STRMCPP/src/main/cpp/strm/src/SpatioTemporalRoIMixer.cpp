#include "strm/SpatioTemporalRoIMixer.hpp"

namespace rm {

SpatioTemporalRoIMixer::SpatioTemporalRoIMixer(const STRMConfig& config,
                                               const ResizeProfile* resizeProfile,
                                               InferenceEngine* inferenceEngine)
    : mConfig(config),
      mLogger(new Logger("/data/data/hcs.offloading.edgedevicecpp/execution_log.csv")),
      mInferenceEngine(inferenceEngine),
      mbIsClosed(false) {
  LOGD("SpatioTemporalRoIMixer()");
  mLogger->logHeader();
  mPatchReconstructor = std::make_unique<PatchReconstructor>(config.patchReconstructorConfig, inferenceEngine);
  mPatchMixer = std::make_unique<PatchMixer>(config.patchMixerConfig, inferenceEngine, mPatchReconstructor.get());
  mRoIExtractor = std::make_unique<RoIExtractor>(config.roIExtractorConfig, resizeProfile);

  mThread = std::thread([this](){
    while (!mbIsClosed.load()) {
      process();
    }
  });
}

SpatioTemporalRoIMixer::~SpatioTemporalRoIMixer() {
  mbIsClosed.store(true);
  mThread.join();
}

int SpatioTemporalRoIMixer::enqueueImage(
    const std::string& key, const cv::Mat& mat) {
  LOGD("SpatioTemporalRoIMixer::enqueueImage(%s, Mat(%d, %d, %d))",
       key.c_str(), mat.cols, mat.rows, mat.channels());
  assert(!mat.empty());
  return 0;
}

std::vector<BoundingBox> SpatioTemporalRoIMixer::getResults(const std::string& key, int frameIndex) {
  LOGD("SpatioTemporalRoIMixer::getResults(%s, %d)", key.c_str(), frameIndex);
  return std::vector<BoundingBox>();
}

void SpatioTemporalRoIMixer::process() {

}

} // namespace rm
