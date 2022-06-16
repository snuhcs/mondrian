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
  static std::vector<MixedFrame> pack(const std::vector<Frame*>& frames, const cv::Size& size,
                                      int numMixedFrames);

 private:
  static bool canFit(std::pair<int, int> wh, const Rect& rect);

  static std::pair<Rect, Rect> splitFreeRect(std::pair<int, int> wh, const Rect& rect);

  static int mMixedFrameIndex;
};

} // namespace rm

#endif // PATCH_MIXER_HPP_
