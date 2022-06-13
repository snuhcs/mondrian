#ifndef SPATIO_TEMPORAL_ROI_MIXER_HPP_
#define SPATIO_TEMPORAL_ROI_MIXER_HPP_

#include <fstream>
#include <map>
#include <set>
#include <string>

#include "strm/Config.hpp"
#include "strm/Dispatcher.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/ResizeProfile.hpp"
#include "strm/RoIExtractor.hpp"
#include "strm/PatchMixer.hpp"
#include "strm/PatchReconstructor.hpp"
#include "strm/Utils.hpp"

namespace rm {

class SpatioTemporalRoIMixer {
 public:
  SpatioTemporalRoIMixer(const STRMConfig& config,
                         const ResizeProfile* resizeProfile,
                         InferenceEngine* inferenceEngine);

  ~SpatioTemporalRoIMixer();

  int enqueueImage(const std::string& key, const cv::Mat& mat);

  std::vector<BoundingBox> getResults(const std::string& key, int frameIndex);

 private:
  void process();

  const STRMConfig mConfig;
  const std::unique_ptr<Logger> mLogger;

  std::unique_ptr<RoIExtractor> mRoIExtractor;
  std::unique_ptr<PatchMixer> mPatchMixer;
  std::unique_ptr<PatchReconstructor> mPatchReconstructor;

  InferenceEngine* mInferenceEngine;

  std::atomic_bool mbIsClosed;
  std::thread mThread;

  std::set<std::string> mKeys;
  std::map<int, Frame> mFrames;
  std::condition_variable mFramesCv;
  std::mutex mFramesMtx;

  int mCountMixedFrameInference;
  bool mUseInferenceResults;
  std::shared_ptr<Frame> mPrevFrame = nullptr;
  std::mutex mResultsMtx;
  std::condition_variable mResultsCv;
};

} // namespace rm

#endif // SPATIO_TEMPORAL_ROI_MIXER_HPP_
