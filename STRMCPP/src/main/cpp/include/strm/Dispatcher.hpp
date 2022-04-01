#ifndef DISPATCHER_HPP_
#define DISPATCHER_HPP_

#include <climits>
#include <map>
#include <utility>
#include <vector>

#include "DataType.hpp"
#include "Config.hpp"
#include "Consumer.hpp"
#include "RoIExtractor.hpp"
#include "PatchMixer.hpp"
#include "RoIPrioritizer.hpp"
#include "ResizeProfile.hpp"
#include "InferenceEngine.hpp"

namespace rm {

class Dispatcher : public Consumer<Frame*> {
 public:
  Dispatcher(DispatcherConfig config,
             RoIExtractorConfig roIExtractorConfig,
             ResizeProfile* resizeProfile,
             RoIPrioritizer* roIPrioritizer,
             InferenceEngine* inferenceEngine,
             PatchMixer* patchMixer)
          : Consumer<Frame*>(config.MAX_QUEUE_SIZE, nullptr),
            mConfig(std::move(config)),
            mRoIExtractor(roIExtractorConfig),
            mResizeProfile(resizeProfile),
            mRoIPrioritizer(roIPrioritizer),
            mInferenceEngine(inferenceEngine),
            mPatchMixer(patchMixer) {}

  void notifyResults();

  std::vector<BoundingBox> getResults(int frameIndex);

 private:
  void process(Frame*& currFrame) override;

  std::vector<BoundingBox> getPrevBoxes();

  int mCountMixedFrameInference = INT_MAX;
  bool mUseInferenceResults = true;
  Frame* mPrevFrame;
  std::mutex mtx;
  std::condition_variable cv;

  DispatcherConfig mConfig;
  std::map<int, std::unique_ptr<Frame>> mFrames;
  RoIExtractor mRoIExtractor;
  RoIPrioritizer* mRoIPrioritizer;
  ResizeProfile* mResizeProfile;
  InferenceEngine* mInferenceEngine;
  PatchMixer* mPatchMixer;
};

} // namespace rm

#endif // DISPATCHER_HPP_
