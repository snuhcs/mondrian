#ifndef PATCH_RECONSTRUCTOR_CALLBACK_
#define PATCH_RECONSTRUCTOR_CALLBACK_

#include "DataType.hpp"

namespace rm {

class PatchReconstructorCallback {
 public:
  virtual void onProcessEnd(MixedFrame& item) = 0;
};

}

#endif // PATCH_RECONSTRUCTOR_CALLBACK_
