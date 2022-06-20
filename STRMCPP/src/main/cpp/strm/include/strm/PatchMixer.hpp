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
  static std::vector<MixedFrame> pack(const std::map<std::string, SortedFrames>& frames,
                                      const Frame* fullFrameTarget,
                                      int mixedFrameSize, int numMixedFrames,
                                      bool probing);

 private:
  static void tryPackRoIs(std::vector<RoI*>& parentRoIs, int mixedFrameSize);

  static bool canFit(std::pair<int, int> wh, const Rect& rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<int, int> wh, const Rect& rect);
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
