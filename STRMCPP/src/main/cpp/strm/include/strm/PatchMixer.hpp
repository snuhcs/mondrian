#ifndef PATCH_MIXER_HPP_
#define PATCH_MIXER_HPP_

#include <mutex>
#include <set>
#include <vector>

#include <opencv2/core/mat.hpp>

#include "Config.hpp"
#include "DataType.hpp"
#include "Log.hpp"
#include "InferenceEngine.hpp"
#include "PatchReconstructor.hpp"

namespace rm {

class PatchMixer {
 public:
  enum Status {
    // each status means status of "patchmixer"
    ONGOING = 1,
    DONE_BUT_DROPPED_FEW_ROIS = 2,
    DONE_BUT_NEED_REPROCESS = 3,
  };

  PatchMixer(PatchMixerConfig config, InferenceEngine* inferenceEngine,
             PatchReconstructor* patchReconstructor);

  Status tryPackAndEnqueueMixedFrame(const std::shared_ptr<Frame>& currFrame);

 private:
  void reset();

  int countPackedFrame(const std::string& key);

  static bool canFit(std::pair<int, int> wh, const Rect& rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<int, int> wh, const Rect& rect);

  void enqueueMixedFrame(MixedFrame mixedFrame);

  int mixedFrameIndex = 0;
  PatchMixerConfig mConfig;
  std::set<std::string> mFinishedKeys;
  std::vector<std::shared_ptr<Frame>> mPackedFrames;
  std::vector<Rect> mFreeRects;

  InferenceEngine* mInferenceEngine;
  PatchReconstructor* mPatchReconstructor;

  std::mutex mPatchMixerMtx;
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
