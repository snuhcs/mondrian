#include "strm/PatchMixer.hpp"

namespace rm {

PatchMixer::PatchMixer(PatchMixerConfig config, InferenceEngine* inferenceEngine,
                       PatchReconstructor* patchReconstructor)
    : mConfig(config), mInferenceEngine(inferenceEngine),
      mPatchReconstructor(patchReconstructor),
      mFreeRects({Rect(0, 0, config.MIXED_FRAME_SIZES.at(0), config.MIXED_FRAME_SIZES.at(0))}) {
  LOGD("PatchMixer() %d %d", config.MIXED_FRAME_SIZES.at(0), config.MAX_PACKED_FRAMES);
}

void PatchMixer::reset() {
  mPackedFrames.clear();
  mFreeRects.clear();
  mFreeRects.emplace_back(0, 0, mConfig.MIXED_FRAME_SIZES.at(0), mConfig.MIXED_FRAME_SIZES.at(0));
}

PatchMixer::Status PatchMixer::tryPackAndEnqueueMixedFrame(const std::shared_ptr<Frame>& currFrame) {
  assert(currFrame != nullptr);
  LOGD("PatchMixer::tryPackAndEnqueueMixedFrame(%s, %d)", currFrame->key.c_str(),
       currFrame->frameIndex);
  std::lock_guard<std::mutex> patchMixerLock(mPatchMixerMtx);

  auto finishedKeysIt = mFinishedKeys.find(currFrame->key);
  if (finishedKeysIt != mFinishedKeys.end()) {
    mFinishedKeys.erase(finishedKeysIt);
    LOGD("PatchMixer::tryPackAndEnqueueMixedFrame(%s, %d) end %d", currFrame->key.c_str(),
         currFrame->frameIndex, DONE_BUT_NEED_REPROCESS);
    return DONE_BUT_NEED_REPROCESS;
  }

  currFrame->mixingStartTime = NowMicros();
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
  currFrame->mixingEndTime = NowMicros();

  /*
  // >>> Will later replace numPackedFrames
  std::chrono::system_clock::time_point oldestBirthTime = currFrame->birthTime;
  for (const Frame* frame : mPackedFrames) {
    if (oldestBirthTime > frame->birthTime) {
      oldestBirthTime = frame->birthTime;
    }
  }
  long long inferenceTimeMs = mInferenceEngine->getInferenceTimeMs();
  long long oldestRoIAge = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - oldestBirthTime).count();
  bool tooOld = oldestRoIAge > (mConfig.LATENCY_SLO_MS - inferenceTimeMs);
  // <<<
  */

  // >>> Will later replaced by above section of code
  int minPackedFrameIndex = currFrame->frameIndex;
  for (const std::shared_ptr<Frame>& frame : mPackedFrames) {
    if (minPackedFrameIndex > frame->frameIndex) {
      minPackedFrameIndex = frame->frameIndex;
    }
  }
  int numPackedFrames = currFrame->frameIndex - minPackedFrameIndex + 1;
  bool tooOld = (numPackedFrames >= mConfig.MAX_PACKED_FRAMES);
  // <<<

  bool doEnqueue = !isAllPacked || tooOld;
  Status status = (!doEnqueue) ? ONGOING :
                  (countPackedFrame(currFrame->key) == 0) ? DONE_BUT_DROPPED_FEW_ROIS
                                                          : DONE_BUT_NEED_REPROCESS;
  bool packCurrFrame = isAllPacked || (status == DONE_BUT_DROPPED_FEW_ROIS);

  if (packCurrFrame) {
    mPackedFrames.push_back(currFrame);
  }

  if (doEnqueue) {
    mFinishedKeys.clear();
    for (const std::shared_ptr<Frame>& frame : mPackedFrames) {
      mFinishedKeys.insert(frame->key);
    }
    mFinishedKeys.erase(currFrame->key);
    enqueueMixedFrame(MixedFrame(mixedFrameIndex++, mPackedFrames, mConfig.MIXED_FRAME_SIZES.at(0),
                                 mConfig.PACKING));
    reset();
  }

  LOGD("PatchMixer::tryPackAndEnqueueMixedFrame(%s, %d) end %d", currFrame->key.c_str(),
       currFrame->frameIndex, status);
  return status;
}

int PatchMixer::countPackedFrame(const std::string& key) {
  int count = 0;
  for (const std::shared_ptr<Frame>& frame : mPackedFrames) {
    if (frame->key == key) {
      count++;
    }
  }
  return count;
}

bool PatchMixer::canFit(std::pair<int, int> wh, const Rect& rect) {
  return wh.first <= rect.width() && wh.second <= rect.height();
}

std::pair<Rect, Rect> PatchMixer::splitFreeRect(std::pair<int, int> wh, const Rect& rect) {
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

void PatchMixer::enqueueMixedFrame(MixedFrame mixedFrame) {
  const time_us mixedFrameEnqueueTime = NowMicros();
  for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
    frame->mixedFrameEnqueueTime = mixedFrameEnqueueTime;
  }
  if (mConfig.PACKING) {
    mixedFrame.handle = mInferenceEngine->enqueue(mixedFrame.packedMat, false);
  } else {
    for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
      for (RoI& roi : frame->rois) {
        if (roi.isPacked()) {
          roi.handle = mInferenceEngine->enqueue(roi.getMat(), false);
        }
      }
    }
  }
  mPatchReconstructor->enqueue(mixedFrame);
}

} // namespace rm
