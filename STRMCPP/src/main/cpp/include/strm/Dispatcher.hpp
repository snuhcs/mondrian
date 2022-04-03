#ifndef DISPATCHER_HPP_
#define DISPATCHER_HPP_

#include <climits>
#include <map>
#include <utility>
#include <vector>

#include "DataType.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "PatchReconstructorCallback.hpp"
#include "RoIExtractor.hpp"
#include "PatchMixer.hpp"
#include "RoIPrioritizer.hpp"
#include "ResizeProfile.hpp"
#include "InferenceEngine.hpp"

namespace rm {

class Dispatcher {
 public:
  Dispatcher(DispatcherConfig config,
             RoIExtractorConfig roIExtractorConfig,
             ResizeProfile* resizeProfile,
             RoIPrioritizer* roIPrioritizer,
             InferenceEngine* inferenceEngine,
             PatchMixer* patchMixer);

  ~Dispatcher();

  void notifyResults();

  std::vector<BoundingBox> getResults(int frameIndex);

  void process(Frame*& currFrame);

  void enqueue(Frame* item);

  Frame* takeItem();

 private:
  std::vector<BoundingBox> getPrevBoxes();

  int mCountMixedFrameInference;
  bool mUseInferenceResults;
  Frame* mPrevFrame;
  std::mutex mtx;
  std::condition_variable cv;

  DispatcherConfig mConfig;
  std::map<int, std::unique_ptr<Frame>> mFrames;
  std::unique_ptr<RoIExtractor> mRoIExtractor;
  RoIPrioritizer* mRoIPrioritizer;
  ResizeProfile* mResizeProfile;
  InferenceEngine* mInferenceEngine;
  PatchMixer* mPatchMixer;

  std::atomic_bool isClosed;
  std::thread mThread;

  int mMaxNumItems;
  std::queue<Frame*> mItems;
  std::condition_variable mItemsCV;
  std::mutex mItemsMtx;
};

} // namespace rm

#endif // DISPATCHER_HPP_
