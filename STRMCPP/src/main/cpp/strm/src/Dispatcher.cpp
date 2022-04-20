#include "strm/Dispatcher.hpp"

#include <cassert>

#include "strm/Log.hpp"

namespace rm {

Dispatcher::Dispatcher(const std::string& key,
                       const DispatcherConfig& config,
                       const RoIExtractorConfig& roIExtractorConfig,
                       const ResizeProfile* resizeProfile,
                       const RoIPrioritizer* roIPrioritizer,
                       InferenceEngine* inferenceEngine,
                       PatchMixer* patchMixer)
    : mKey(key),
      mTag(key.substr(key.size() - 8)),
      mConfig(config),
      mRoIExtractor(new RoIExtractor(roIExtractorConfig)),
      mResizeProfile(resizeProfile),
      mRoIPrioritizer(roIPrioritizer),
      mInferenceEngine(inferenceEngine),
      mPatchMixer(patchMixer),
      mMaxNumItems(config.MAX_QUEUE_SIZE),
      isClosed(false),
      mCountMixedFrameInference(INT_MAX),
      mUseInferenceResults(true),
      mPrevFrame(nullptr) {
  LOGD("Dispatcher%s()", mTag.c_str());
  mThread = std::thread([this]() {
    while (!isClosed.load()) {
      std::shared_ptr<Frame> frame = getFrameToProcess();
      process(frame);
    }
  });
};

Dispatcher::~Dispatcher() {
  isClosed.store(true);
  mThread.join();
};

int Dispatcher::enqueue(const cv::Mat mat) {
  LOGD("Dispatcher%s::enqueue(Mat(%d, %d, %d))", mTag.c_str(), mat.cols, mat.rows, mat.channels());
  std::unique_lock<std::mutex> lock(mFramesMtx);
  mFramesCv.wait(lock, [this] {
    return mEnqueuedFrameIndex - mProcessedFrameIndex < mMaxNumItems;
  });
  int frameIndex = mEnqueuedFrameIndex++;
  mFrames.insert(std::make_pair(frameIndex, std::make_shared<Frame>(mKey, frameIndex, mat)));
  LOGD("Dispatcher%s::enqueue(%d) end", mTag.c_str(), frameIndex);
  lock.unlock();
  mFramesCv.notify_all();
  return frameIndex;
}

std::shared_ptr<Frame> Dispatcher::getFrameToProcess() {
  LOGD("Dispatcher%s::getFrameToProcess()", mTag.c_str());
  std::unique_lock<std::mutex> lock(mFramesMtx);
  mFramesCv.wait(lock, [this] {
    return mEnqueuedFrameIndex > mProcessedFrameIndex;
  });
  std::shared_ptr<Frame> frame = mFrames.at(mProcessedFrameIndex++);
  lock.unlock();
  mFramesCv.notify_all();
  return frame;
}

void Dispatcher::process(const std::shared_ptr<Frame>& currFrame) {
  LOGD("Dispatcher%s::process(%d)", mTag.c_str(), currFrame->frameIndex);
  assert(currFrame != nullptr);
  assert(mPrevFrame == nullptr || mPrevFrame->frameIndex + 1 == currFrame->frameIndex);
  /* Cases
   * 1. Full frame inference
   * 2. Mixed frame inference
   *   2.1. If multiple frames from the source stream packed into mixed frame,
   *        PatchMixer will exclude the last packed frame from the packed frames.
   *        We have to re-pack the frame into next mixed frame.
   *   2.2. Else wait for mixed frame result and continue to process next frame.
   */
  if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
    mCountMixedFrameInference = 0;
    mUseInferenceResults = true;
    int handle = mInferenceEngine->enqueue(currFrame->mat, true);
    std::vector<BoundingBox> results = mInferenceEngine->getResults(handle);
    currFrame->boxes.insert(currFrame->boxes.end(), results.begin(), results.end());
    currFrame->isResultReady.store(true);
    notifyResults();
  } else {
    std::vector<BoundingBox> prevResults = getPrevBoxes(mUseInferenceResults);
    mRoIExtractor->process(std::make_pair(std::make_pair(mPrevFrame, currFrame), prevResults));
    std::sort(currFrame->rois.begin(), currFrame->rois.end(),
              [this](const RoI& lhs, const RoI& rhs) -> bool {
                return mRoIPrioritizer->priority(lhs) < mRoIPrioritizer->priority(rhs);
              });
    for (auto& roi : currFrame->rois) {
      roi.scale = mResizeProfile->getScale(roi.labelName, roi.location.width(),
                                           roi.location.height(), roi.minOriginLength);
    }
    PatchMixer::Status status = mPatchMixer->tryPackAndEnqueueMixedFrame(currFrame.get());
    LOGD("PatchMixer::Status: %d", status);
    if (status == PatchMixer::CONTINUE_PACKING) {
      mUseInferenceResults = false;
    } else if (status == PatchMixer::FINISHED) {
      mCountMixedFrameInference++;
      mUseInferenceResults = true;
    } else if (status == PatchMixer::FINISHED_AND_PROCESS_LAST_FRAME_AGAIN) {
      mCountMixedFrameInference++;
      mUseInferenceResults = true;
      process(currFrame);
    } else {
      // TODO: Error handling
    }
  }
  LOGD("Dispatcher%s::process(%d) end, %d, %d", mTag.c_str(), currFrame->frameIndex,
       mCountMixedFrameInference, mUseInferenceResults);
  mPrevFrame = currFrame;
}

std::vector<BoundingBox> Dispatcher::getPrevBoxes(bool useInferenceResults) {
  LOGD("Dispatcher%s::getPrevBoxes()", mTag.c_str());
  std::vector<BoundingBox> prevResults;
  if (!mRoIExtractor->useOpticalFlowRoIs()) {
    return prevResults;
  }
  if (useInferenceResults) {
    std::unique_lock<std::mutex> lock(mResultsMtx);
    mResultsCv.wait(lock, [this]() {
      return mPrevFrame->isResultReady.load();
    });
    for (const BoundingBox& box : mPrevFrame->boxes) {
      prevResults.emplace_back(Rect(
          box.location.left - mConfig.ROI_PADDING,
          box.location.top - mConfig.ROI_PADDING,
          box.location.right + mConfig.ROI_PADDING,
          box.location.bottom + mConfig.ROI_PADDING), box.confidence, box.labelName);
    }
  } else {
    for (const RoI& roi : mPrevFrame->opticalFlowRoIs) {
      prevResults.emplace_back(roi.location, 1.0, roi.labelName);
    }
  }
  LOGD("Dispatcher::getPrevBoxes() end : %d", (int) prevResults.size());
  return prevResults;
}

void Dispatcher::notifyResults() {
  LOGD("Dispatcher::notifyResults()");
  mResultsCv.notify_all();
}

std::vector<BoundingBox> Dispatcher::getResults(int frameIndex) {
  LOGD("Dispatcher::getResults(%d)", frameIndex);
  std::unique_lock<std::mutex> lock(mResultsMtx);
  mResultsCv.wait(lock, [this, &frameIndex]() {
    return mFrames.find(frameIndex) != mFrames.end() &&
           mFrames.at(frameIndex)->isResultReady.load();
  });
  LOGD("Dispatcher::getResults(%d) end", frameIndex);
  std::vector<BoundingBox> boxes = mFrames.at(frameIndex)->boxes;
  mFrames.erase(frameIndex);
  return boxes;
}

} // namespace rm
