#ifndef DISPATCHER_HPP_
#define DISPATCHER_HPP_

#include <climits>
#include <map>
#include <utility>
#include <vector>

#include "Config.hpp"
#include "DataType.hpp"
#include "ResizeProfile.hpp"
#include "RoIPrioritizer.hpp"
#include "InferenceEngine.hpp"
#include "RoIExtractor.hpp"
#include "PatchMixer.hpp"
#include "PatchReconstructorCallback.hpp"

namespace rm {

class Dispatcher {
 public:
  Dispatcher(const std::string& key,
             const DispatcherConfig& config,
             const RoIExtractorConfig& roIExtractorConfig,
             const ResizeProfile* resizeProfile,
             const RoIPrioritizer* roIPrioritizer,
             InferenceEngine* inferenceEngine,
             PatchMixer* patchMixer);

  ~Dispatcher();

  int enqueue(const cv::Mat& mat);

  std::shared_ptr<Frame> getFrameToProcess();

  void process(const std::shared_ptr<Frame>& currFrame);

  void notifyResults();

  std::vector<BoundingBox> getResults(int frameIndex);

 private:
  std::vector<BoundingBox> getPrevBoxes();

  const std::string mKey;
  const std::string mTag;

  int mCountMixedFrameInference;
  bool mUseInferenceResults;
  std::shared_ptr<Frame> mPrevFrame = nullptr;
  std::mutex mResultsMtx;
  std::condition_variable mResultsCv;

  DispatcherConfig mConfig;
  std::unique_ptr<RoIExtractor> mRoIExtractor;
  const RoIPrioritizer* mRoIPrioritizer;
  const ResizeProfile* mResizeProfile;
  InferenceEngine* mInferenceEngine;
  PatchMixer* mPatchMixer;

  std::atomic_bool isClosed;
  std::thread mThread;

  int mMaxNumItems;
  int mEnqueuedFrameIndex = 0;
  int mProcessedFrameIndex = 0;
  std::map<int, std::shared_ptr<Frame>> mFrames;
  std::condition_variable mFramesCv;
  std::mutex mFramesMtx;
};

} // namespace rm

#endif // DISPATCHER_HPP_
