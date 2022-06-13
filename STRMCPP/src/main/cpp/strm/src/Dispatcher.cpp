#include "strm/Dispatcher.hpp"

#include <cassert>

#include "strm/Log.hpp"

namespace rm {

Dispatcher::~Dispatcher() {
  isClosed.store(true);
  mThread.join();
}

int Dispatcher::enqueue(const cv::Mat& mat) {
  LOGD("Dispatcher%s::enqueuePDJob(Mat(%d, %d, %d))", mTag.c_str(), mat.cols, mat.rows, mat.channels());
  const time_us enqueueTime = NowMicros();
  std::unique_lock<std::mutex> lock(mFramesMtx);
  mFramesCv.wait(lock, [this] {
    return mEnqueuedFrameIndex - mProcessedFrameIndex < mMaxNumItems;
  });
  int frameIndex = mEnqueuedFrameIndex++;
  mFrames.insert(std::make_pair(
      frameIndex, std::make_shared<Frame>(mKey, frameIndex, mat, enqueueTime)));
  LOGD("Dispatcher%s::enqueuePDJob(%d) end", mTag.c_str(), frameIndex);
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
  currFrame->dispatcherProcessStartTime = NowMicros();
  if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
    mCountMixedFrameInference = 0;
    mUseInferenceResults = true;
    currFrame->fullFrameEnqueueTime = NowMicros();
    int handle = mInferenceEngine->enqueuePDJob(currFrame->mat, true);
    std::vector<BoundingBox> results = mInferenceEngine->getResults(handle);
    currFrame->fullFrameGetResultsTime = NowMicros();
    currFrame->boxes.insert(currFrame->boxes.end(), results.begin(), results.end());
    currFrame->isResultReady.store(true);
    notifyResults();
  } else {
    currFrame->rois = mRoIExtractor->process(mPrevFrame.get(), currFrame.get(),
                                             getPrevBoxes(mUseInferenceResults));

    PatchMixer::Status status = mPatchMixer->tryPackAndEnqueueMixedFrame(currFrame);
    LOGD("PatchMixer::Status: %d", status);

    if (status == PatchMixer::ONGOING) {
      mUseInferenceResults = false;
    } else {
      mUseInferenceResults = true;
      mCountMixedFrameInference++;
      if (status == PatchMixer::DONE_BUT_NEED_REPROCESS) {
        process(currFrame);
      }
    }
  }
  currFrame->dispatcherProcessEndTime = NowMicros();
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
  std::shared_ptr<Frame> frame = mFrames.at(frameIndex);
  std::vector<BoundingBox> boxes = frame->boxes;
  mFrames.erase(frameIndex);
  mLogger->log(frame.get());
  return boxes;
}

} // namespace rm
