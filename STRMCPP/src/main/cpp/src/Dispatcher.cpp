#include "strm/Dispatcher.hpp"

#include <cassert>

#include "strm/Log.hpp"

namespace rm {

void Dispatcher::process(Frame*& currFrame) {
  LOGD("Dispatcher::process %d %d",
       currFrame != nullptr ? currFrame->frameIndex : -1,
       mPrevFrame != nullptr ? mPrevFrame->frameIndex : -1);
  assert(currFrame != nullptr && mPrevFrame == nullptr || mPrevFrame->frameIndex + 1 == currFrame->frameIndex);
  /* Cases
   * 1. Full frame inference
   * 2. Mixed frame inference
   *   2.1. If multiple frames from the source stream packed into mixed frame,
   *        PatchMixer will exclude the last packed frame from the packed frames.
   *        We have to re-pack the frame into next mixed frame.
   *   2.2. Else wait for mixed frame result and continue to process next frame.
   */
  mFrames.insert(std::make_pair(currFrame->frameIndex, std::unique_ptr<Frame>(currFrame)));
  if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
    mCountMixedFrameInference = 0;
    mUseInferenceResults = true;
    int handle = mInferenceEngine->enqueue(currFrame->mat->clone(), true);
    std::vector<BoundingBox> results = mInferenceEngine->getResults(handle);
    currFrame->boxes.insert(currFrame->boxes.end(), results.begin(), results.end());
    notifyResults();
  } else {
    std::vector<BoundingBox> prevResults = getPrevBoxes();
    mRoIExtractor.process(std::make_pair(std::make_pair(mPrevFrame, currFrame), prevResults));
    std::sort(currFrame->rois.begin(), currFrame->rois.end(), [this](const RoI& lhs, const RoI& rhs) -> bool {
      return mRoIPrioritizer->priority(lhs) < mRoIPrioritizer->priority(rhs);
    });
    for (auto& roi : currFrame->rois) {
      roi.scale = mResizeProfile->getScale(roi.labelName, roi.location.width(),
                                           roi.location.height(), roi.minOriginLength);
    }
    PatchMixer::Status status = mPatchMixer->tryPackAndEnqueueMixedFrame(currFrame);
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
  mPrevFrame = currFrame;
}

std::vector<BoundingBox> Dispatcher::getPrevBoxes() {
  LOGD("Dispatcher::getPrevBoxes");
  std::vector<BoundingBox> prevResults;
  if (!mRoIExtractor.useOpticalFlowRoIs()) {
    return prevResults;
  }
  std::unique_lock<std::mutex> lock(mtx);
  if (mUseInferenceResults) {
    cv.wait(lock, [this](){
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
  return prevResults;
}

void Dispatcher::notifyResults() {
  LOGD("Dispatcher::notifyResults");
  cv.notify_all();
}

std::vector<BoundingBox> Dispatcher::getResults(int frameIndex) {
  LOGD("Dispatcher::getResults Start %d", frameIndex);
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this, &frameIndex]() {
    return mFrames.find(frameIndex) != mFrames.end() && mFrames.at(frameIndex)->isResultReady.load();
  });
  LOGD("Dispatcher::getResults End   %d", frameIndex);
  return mFrames.at(frameIndex)->boxes;
}

void Dispatcher::enqueue(Frame* item) {
  LOGD("Dispatcher::enqueue start %d", item->frameIndex);
  std::unique_lock<std::mutex> lock(mItemsMtx);
  mItemsCV.wait(lock, [this] {
    return mItems.size() < mMaxNumItems;
  });
  mItems.push(item);
  lock.unlock();
  mItemsCV.notify_all();
  LOGD("Dispatcher::enqueue end   %d", item->frameIndex);
}

Frame* Dispatcher::takeItem() {
  LOGD("Dispatcher::takeItem start");
  std::unique_lock<std::mutex> lock(mItemsMtx);
  mItemsCV.wait(lock, [this] {
    return !mItems.empty();
  });
  Frame* item = mItems.front();
  mItems.pop();
  lock.unlock();
  mItemsCV.notify_all();
  LOGD("Dispatcher::takeItem end %d", item->frameIndex);
  return item;
}

} // namespace rm
