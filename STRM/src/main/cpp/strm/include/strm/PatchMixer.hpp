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
                                Frame* fullFrameTarget, RoIResizer* roiResizer, float maxRoISize,
                                bool probe, int numProbeSteps, float probeStepSize,
                                bool roiWiseInference) const;

  std::vector<MixedFrame> packRoIs(std::vector<RoI*>& candidateRoIs, int mixedFrameSize,
                                   int maxNumMixedFrames) const;

  static bool tryPackRoI(const std::pair<float, float>& resizedWH,
                         std::map<int, std::vector<Rect>>& freeRectsMap,
                         bool firstMatch, RoI* pRoI = nullptr,
                         std::map<int, std::set<RoI*>>* packedRoIsMap = nullptr,
                         bool emulatedBatch = false);

 private:
  static void addProbeRoIs(std::map<std::string, SortedFrames>& frames,
                           const Frame* fullFrameTarget, int numProbeSteps, float probeStepSize);

  static std::vector<RoI*> collectRoIs(std::map<std::string, SortedFrames>& frames,
                                       const Frame* fullFrameTarget);

  static void prioritizeRoIs(std::map<std::string, SortedFrames>& frames,
                             const Frame* fullFrameTarget);

  static bool canFit(std::pair<float, float> wh, const Rect& rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<float, float> wh, const Rect& rect);

  static const float HIGH_PRIORITY;

  PatchMixerConfig mConfig;
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
