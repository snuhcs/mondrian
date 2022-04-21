#include "strm/PatchMixer.hpp"

namespace rm {

PatchMixer::PatchMixer(PatchMixerConfig config, InferenceEngine* inferenceEngine,
                       PatchReconstructor* patchReconstructor)
    : mConfig(config), mInferenceEngine(inferenceEngine),
      mPatchReconstructor(patchReconstructor),
      mFreeRects({Rect(0, 0, config.MIXED_FRAME_SIZE, config.MIXED_FRAME_SIZE)}) {
  LOGD("PatchMixer() %d %d", config.MIXED_FRAME_SIZE, config.MAX_PACKED_FRAMES);
};

void PatchMixer::reset() {
  mPackedFrames.clear();
  mFreeRects.clear();
  mFreeRects.emplace_back(0, 0, mConfig.MIXED_FRAME_SIZE, mConfig.MIXED_FRAME_SIZE);
}

PatchMixer::Status PatchMixer::tryPackAndEnqueueMixedFrame(Frame* currFrame) {
  assert(currFrame != nullptr);
  LOGD("PatchMixer::tryPackAndEnqueueMixedFrame(%s, %d)", currFrame->key.c_str(),
       currFrame->frameIndex);
  std::lock_guard<std::mutex> patchMixerLock(mPatchMixerMtx);
  auto finishedKeysIt = mFinishedKeys.find(currFrame->key);
  if (finishedKeysIt != mFinishedKeys.end()) {
    mFinishedKeys.erase(finishedKeysIt);
    LOGD("PatchMixer::tryPackAndEnqueueMixedFrame(%s, %d) end %d", currFrame->key.c_str(),
         currFrame->frameIndex, FINISHED_AND_PROCESS_LAST_FRAME_AGAIN);
    return FINISHED_AND_PROCESS_LAST_FRAME_AGAIN;
  }
  bool isAllPacked = true;
  for (RoI& roi : currFrame->rois) {
    std::pair<int, int> wh = roi.getResizedWidthHeight();
    bool isPacked = false;
    for (auto it = mFreeRects.begin(); it != mFreeRects.end(); it++) {
      const Rect freeRect = *it;
      if (canFit(wh, freeRect)) {
        mFreeRects.erase(it);
        roi.packedLocation = std::make_pair(freeRect.left, freeRect.top);
        std::pair<Rect, Rect> newFreeRectPair = splitFreeRect(wh, freeRect);
        mFreeRects.push_back(newFreeRectPair.first);
        mFreeRects.push_back(newFreeRectPair.second);
        isPacked = true;
        break;
      }
    }
    isAllPacked &= isPacked;
  }

  mPackedFrames.push_back(currFrame);
  int minPackedFrameIndex = INT_MAX;
  for (const Frame* frame : mPackedFrames) {
    if (minPackedFrameIndex > frame->frameIndex) {
      minPackedFrameIndex = frame->frameIndex;
    }
  }
  int numPackedFrames = currFrame->frameIndex - minPackedFrameIndex + 1;
  if (!isAllPacked || numPackedFrames >= mConfig.MAX_PACKED_FRAMES) {
    Status status = FINISHED;
    if (countPackedFrame(currFrame->key) >= 2) {
      mPackedFrames.erase(std::find(mPackedFrames.begin(), mPackedFrames.end(), currFrame));
      status = FINISHED_AND_PROCESS_LAST_FRAME_AGAIN;
    }

    if (mConfig.PACKING) {
      MixedFrame mixedFrame(mixedFrameIndex++, mPackedFrames, mConfig.MIXED_FRAME_SIZE, true);
      mixedFrame.handle = mInferenceEngine->enqueue(mixedFrame.packedMat, false);
      mPatchReconstructor->enqueue(mixedFrame);
    } else {
      MixedFrame mixedFrame(mixedFrameIndex++, mPackedFrames, mConfig.MIXED_FRAME_SIZE, false);
      for (Frame* frame : mixedFrame.packedFrames) {
        for (RoI& roi : frame->rois) {
          if (roi.isPacked()) {
            roi.handle = mInferenceEngine->enqueue(roi.getMat(), false);
          }
        }
      }
      mPatchReconstructor->enqueue(mixedFrame);
    }
    mFinishedKeys.clear();
    for (Frame* frame : mPackedFrames) {
      mFinishedKeys.insert(frame->key);
    }
    mFinishedKeys.erase(currFrame->key);
    reset();
    LOGD("PatchMixer::tryPackAndEnqueueMixedFrame(%s, %d) end %d", currFrame->key.c_str(),
         currFrame->frameIndex, status);
    return status;
  } else {
    LOGD("PatchMixer::tryPackAndEnqueueMixedFrame(%s, %d) end %d", currFrame->key.c_str(),
         currFrame->frameIndex, CONTINUE_PACKING);
    return CONTINUE_PACKING;
  }
}

int PatchMixer::countPackedFrame(const std::string& key) {
  int count = 0;
  for (Frame* frame : mPackedFrames) {
    if (frame->key == key) {
      count++;
    }
  }
  return count;
}

bool PatchMixer::canFit(std::pair<int, int> wh, Rect rect) {
  return wh.first <= rect.width() && wh.second <= rect.height();
}

std::pair<Rect, Rect> PatchMixer::splitFreeRect(std::pair<int, int> wh, Rect rect) {
  int w = wh.first;
  int h = wh.second;
  if (rect.width() > rect.height()) {
    return std::make_pair(Rect(rect.left + w, rect.top, rect.right, rect.bottom),
                          Rect(rect.left, rect.top + h, rect.left + w, rect.bottom));
  } else {
    return std::make_pair(Rect(rect.left, rect.top + h, rect.right, rect.bottom),
                          Rect(rect.left + w, rect.top, rect.right, rect.top + h));
  }
}

} // namespace rm
