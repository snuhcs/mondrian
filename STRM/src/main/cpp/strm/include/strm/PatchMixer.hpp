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

class RoIExtractor;

class PatchMixer {
  friend RoIExtractor;
 public:
  PatchMixer(const PatchMixerConfig& config);

  std::tuple<std::vector<MixedFrame>, Frame*, MultiStream, Stream> packRoIs(
      MultiStream& frames, int fullFrameStreamIndex, int frameSize, int numFrames,
      bool allowInterpolation, bool roiWiseInference, bool probe, int numProbeSteps,
      float probeStepSize);

  static bool tryPackRoI(const std::pair<float, float>& resizedWH,
                         std::map<int, std::vector<Rect>>& freeRectsMap,
                         bool firstMatch, RoI* pRoI = nullptr,
                         std::map<int, std::set<RoI*>>* packedRoIsMap = nullptr,
                         bool emulatedBatch = false);

 private:
  static Frame* getFullFrameTarget(const MultiStream& selectedFrames,
                                   int fullFrameStreamIndex);

  static std::vector<Frame*> addProbeRoIs(MultiStream& frames, const Frame* fullFrameTarget,
                                          int numProbeSteps, float probeStepSize);

  static std::vector<RoI*> collectRoIs(MultiStream& frames, const Frame* fullFrameTarget);

  static void prioritizeRoIs(MultiStream& frames, const Frame* fullFrameTarget);

  static bool canFit(std::pair<float, float> wh, const Rect& rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<float, float> wh, const Rect& rect);

  static const float HIGHEST_PRIORITY;
  static const float HIGHER_PRIORITY;
  static const float HIGH_PRIORITY;

  PatchMixerConfig mConfig;
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
