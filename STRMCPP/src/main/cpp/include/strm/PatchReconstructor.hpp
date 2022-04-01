#ifndef PATCH_RECONSTRUCTOR_HPP_
#define PATCH_RECONSTRUCTOR_HPP_

#include "Config.hpp"
#include "Consumer.hpp"
#include "DataType.hpp"
#include "InferenceEngine.hpp"

namespace rm {

class PatchReconstructor : public Consumer<MixedFrame> {
 public:
  PatchReconstructor(PatchReconstructorConfig config,
                     InferenceEngine* inferenceEngine,
                     ConsumerCallback<MixedFrame>* callback)
          : Consumer<MixedFrame>(config.MAX_QUEUE_SIZE, callback),
            mConfig(config),
            mInferenceEngine(inferenceEngine) {};

  void process(MixedFrame& item) override;

 private:
  static void updateMixedFrameInferenceResults(
          MixedFrame& mixedFrame, int matchPadding, float useIoUThreshold);

  static void updateRoIInferenceResults(MixedFrame& mixedFrame);

  PatchReconstructorConfig mConfig;
  InferenceEngine* mInferenceEngine;
};

} // namespace rm

#endif // PATCH_RECONSTRUCTOR_HPP_
