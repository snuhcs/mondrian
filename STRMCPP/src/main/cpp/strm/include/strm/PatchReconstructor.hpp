#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include <queue>
#include <thread>

#include "Config.hpp"
#include "DataType.hpp"
#include "InferenceEngine.hpp"
#include "PatchReconstructorCallback.hpp"

namespace rm {

class PatchReconstructor {
 public:
  PatchReconstructor(PatchReconstructorConfig config,
                     InferenceEngine* inferenceEngine,
                     PatchReconstructorCallback* callback);

  ~PatchReconstructor();

  void enqueue(const MixedFrame& item);

 private:
  void process(MixedFrame& item);
  MixedFrame takeItem();
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
