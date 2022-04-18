#ifndef PATCH_RECONSTRUCTOR_CALLBACK_
#define PATCH_RECONSTRUCTOR_CALLBACK_

#include "strm/DataType.hpp"

namespace rm {

class PatchReconstructorCallback {
 public:
  virtual void notifyMixedInferenceResults(const MixedFrame& item) = 0;
};

} // namespace rm

#endif // PATCH_RECONSTRUCTOR_CALLBACK_
