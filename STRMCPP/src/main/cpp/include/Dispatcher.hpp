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

class Dispatcher : public Consumer<Frame> {
public:
    Dispatcher(DispatcherConfig config,
               RoIExtractorConfig roIExtractorConfig,
               ResizeProfile* resizeProfile,
               RoIPrioritizer* roIPrioritizer,
               InferenceEngine* inferenceEngine,
               PatchMixer* patchMixer)
            : Consumer<Frame>(config.MAX_QUEUE_SIZE, nullptr),
              mConfig(std::move(config)),
              mRoIExtractor(roIExtractorConfig),
              mResizeProfile(resizeProfile),
              mRoIPrioritizer(roIPrioritizer),
              mInferenceEngine(inferenceEngine),
              mPatchMixer(patchMixer) {}

    virtual void process(Frame& currFrame) override;
    void notifyResults(const int frameIndex);
    void notifyResults(const std::vector<int>& frameIndices);
    std::vector<BoundingBox> getResults(int frameIndex);

private:
    std::vector<BoundingBox> getPrevBoxes();
    void setPrevBoxesWithRoIs(std::vector<BoundingBox>& prevBoxes);

    int mCountMixedFrameInference = INT_MAX;
    Frame* mPrevFrame;
    std::vector<BoundingBox>* mPrevResults;

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
