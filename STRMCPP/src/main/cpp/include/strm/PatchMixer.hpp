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
    CONTINUE_PACKING = 1,
    FINISHED = 2,
    FINISHED_AND_PROCESS_LAST_FRAME_AGAIN = 3,
  };

  PatchMixer(PatchMixerConfig config, InferenceEngine* inferenceEngine,
             PatchReconstructor* patchReconstructor);

  Status tryPackAndEnqueueMixedFrame(Frame* currFrame);

 private:
  void reset();

  int countPackedFrame(const std::string& key);

  static cv::Mat getMixedImage(const std::vector<Frame*>& frames, int mixedFrameSize);

  static bool canFit(std::pair<int, int> wh, Rect rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<int, int> wh, Rect rect);

  PatchMixerConfig mConfig;
  std::set<std::string> mFinishedKeys;
  std::vector<Frame*> mPackedFrames;
  std::vector<Rect> mFreeRects;

  InferenceEngine* mInferenceEngine;
  PatchReconstructor* mPatchReconstructor;

  std::mutex mPatchMixerMtx;
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
