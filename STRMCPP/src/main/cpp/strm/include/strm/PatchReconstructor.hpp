#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "strm/Config.hpp"
#include "strm/DataType.hpp"
#include "strm/InferenceEngine.hpp"

namespace rm {

class PatchReconstructor {
 public:
  PatchReconstructor(const PatchReconstructorConfig& config);

  void reconstructResults(
      MixedFrame& mixedFrame, const std::vector<BoundingBox>& results) ;

 private:
  PatchReconstructorConfig mConfig;
};

} // namespace rm

#endif // PATCH_RECONSTRUCTOR_HPP_
