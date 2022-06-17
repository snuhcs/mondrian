#ifndef PATCH_MIXER_HPP_
#define PATCH_MIXER_HPP_

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
  static std::vector<MixedFrame> pack(const std::set<Frame*>& frames,
                                      const std::set<Frame*>& lastFrames,
                                      int mixedFrameSize, int numMixedFrames);

 private:
  static void tryPackRoI(RoI* roi,
                         std::map<int, std::vector<Rect>>& freeRectsMap,
                         std::map<int, std::vector<RoI*>>& packedRoIs,
                         std::vector<RoI*>& droppedRoIs);

  static bool canFit(std::pair<int, int> wh, const Rect& rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<int, int> wh, const Rect& rect);

  static int mMixedFrameIndex;
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
