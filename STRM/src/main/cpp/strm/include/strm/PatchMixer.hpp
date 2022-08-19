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
      MultiStream& frames, int fullFrameStreamIndex,
      const std::vector<InferenceInfo>& inferencePlan,
      bool allowInterpolation, bool roiWiseInference, RoIResizer* roiResizer);

  bool tryPackRoI(const std::pair<float, float>& resizedWH,
                  std::vector<FreeRects>& freeRectsList,
                  bool firstMatch, RoI* pRoI = nullptr,
                  std::map<int, std::set<RoI*>>* packedRoIsMap = nullptr) const;

 private:
  std::tuple<Frame*, std::vector<Frame*>, std::vector<RoI*>> preparePack(
      MultiStream& selectedFrames, int fullFrameStreamIndex, RoIResizer* mRoIResizer) const;

  static void splitFrames(MultiStream& selectedFrames, Stream& droppedFrames,
                          int numPackableFrames);

  static Frame* getFullFrameTarget(const MultiStream& selectedFrames,
                                   int fullFrameStreamIndex);

  static std::vector<Frame*> addProbeRoIs(MultiStream& frames, const Frame* fullFrameTarget,
                                          RoIResizer* mRoIResizer);

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
