#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "Config.hpp"
#include "DataType.hpp"
#include "Log.hpp"
#include "PatchReconstructorCallback.hpp"
#include "InferenceEngine.hpp"

namespace rm {

class PatchReconstructor {
 public:
  PatchReconstructor(PatchReconstructorConfig config,
                     InferenceEngine* inferenceEngine,
                     PatchReconstructorCallback* callback);

  void process(MixedFrame& item);

  void enqueue(const MixedFrame& item);

  MixedFrame takeItem();

 private:
  static void updateMixedFrameInferenceResults(
      MixedFrame& mixedFrame, int matchPadding, float useIoUThreshold);

  static void updateRoIInferenceResults(MixedFrame& mixedFrame);

  PatchReconstructorConfig mConfig;
  InferenceEngine* mInferenceEngine;

  std::atomic_bool isClosed;
  std::thread mThread;
  PatchReconstructorCallback* mCallback;

  int mMaxNumItems;
  std::queue<MixedFrame> mItems;
  std::condition_variable mItemsCV;
  std::mutex mItemsMtx;
};

} // namespace rm

#endif // PATCH_RECONSTRUCTOR_HPP_
