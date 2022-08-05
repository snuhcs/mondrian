#ifndef PATCH_MIXER_HPP_
#define PATCH_MIXER_HPP_

#include <map>
#include <mutex>
#include <set>
#include <vector>

#include <opencv2/core/mat.hpp>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/Log.hpp"
#include "strm/InferenceEngine.hpp"
#include "strm/PatchReconstructor.hpp"

namespace rm {

class PatchMixer {
 public:
  PatchMixer(const PatchMixerConfig& config);

  std::vector<RoI*> prepareRoIs(std::map<std::string, SortedFrames>& frames,
                                Frame* fullFrameTarget, RoIResizer* roiResizer, int maxRoISize,
                                bool probe, int numProbeSteps, int probeStepSize) const;

  std::vector<MixedFrame> packRoIs(std::vector<RoI*>& candidateRoIs, int mixedFrameSize,
                                   int maxNumMixedFrames) const;

 private:
  static void resizeRoIs(std::map <std::string, SortedFrames>& frames, RoIResizer* roiResizer);

  static void initParentRoIs(std::map<std::string, SortedFrames>& frames);

  static void mergeRoIs(std::map<std::string, SortedFrames>& frames, int maxRoISize,
                        float mergeThreshold);

  static void addProbeRoIs(std::map<std::string, SortedFrames>& frames,
                           const Frame* fullFrameTarget, int numProbeSteps, int probeStepSize);

  static std::vector<RoI*> collectRoIs(std::map<std::string, SortedFrames>& frames,
                                       const Frame* fullFrameTarget);

  static void prioritizeRoIs(std::map<std::string, SortedFrames>& frames,
                             const Frame* fullFrameTarget);

  static bool canFit(std::pair<int, int> wh, const Rect& rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<int, int> wh, const Rect& rect);

  static const float HIGH_PRIORITY;

  PatchMixerConfig mConfig;
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
