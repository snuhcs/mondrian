#include "strm/PatchReconstructor.hpp"

#include "strm/Log.hpp"

namespace rm {

PatchReconstructor::PatchReconstructor(PatchReconstructorConfig config,
                                       InferenceEngine* inferenceEngine,
                                       PatchReconstructorCallback* callback)
    : mConfig(config),
      mInferenceEngine(inferenceEngine),
      mCallback(callback),
      mMaxNumItems(config.MAX_QUEUE_SIZE),
      isClosed(false) {
  LOGD("PatchReconstructor()");
  mThread = std::thread([this]() {
    while (!isClosed.load()) {
      MixedFrame item = takeItem();
      process(item);
      mCallback->notifyMixedInferenceResults(item);
    }
  });
};

void PatchReconstructor::process(MixedFrame& mixedFrame) {
  LOGD("PatchReconstructor::process(%d) %d frames packed", mixedFrame.mixedFrameIndex,
       mixedFrame.packedFrames.size());
  if (!mixedFrame.packedMat.empty()) {
    mixedFrame.boxes = mInferenceEngine->getResults(mixedFrame.handle);
    updateMixedFrameInferenceResults(mixedFrame, mConfig.MATCH_PADDING, mConfig.USE_IOU_THRESHOLD);
  } else {
    for (Frame* frame : mixedFrame.packedFrames) {
      for (RoI& roi : frame->rois) {
        if (roi.isPacked()) {
          assert(roi.handle != -1);
          roi.boxes = mInferenceEngine->getResults(roi.handle);
        }
      }
    }
    updateRoIInferenceResults(mixedFrame);
  }
}

void PatchReconstructor::updateMixedFrameInferenceResults(MixedFrame& mixedFrame, int matchPadding,
                                                          float useIoUThreshold) {
  for (const BoundingBox& box : mixedFrame.boxes) {
    float maxOverlap = -1;
    Rect maxBoxPos;
    Frame* maxFrame = nullptr;
    for (Frame*& frame : mixedFrame.packedFrames) {
      for (RoI& roi : frame->rois) {
        if (roi.isPacked()) {
          Rect paddedRoIPos(
              std::max(0, roi.location.left - matchPadding),
              std::max(0, roi.location.top - matchPadding),
              std::min(roi.frame->mat.cols, roi.location.right + matchPadding),
              std::min(roi.frame->mat.rows, roi.location.bottom + matchPadding));
          Rect movedAndResizedBoxPos(
              std::max(0,
                       (int) ((float) (box.location.left - roi.packedLocation.first) / roi.scale) +
                       roi.location.left),
              std::max(0,
                       (int) ((float) (box.location.top - roi.packedLocation.second) / roi.scale) +
                       roi.location.top),
              std::min(roi.frame->mat.cols,
                       (int) ((float) (box.location.right - roi.packedLocation.first) / roi.scale) +
                       roi.location.left),
              std::min(roi.frame->mat.rows,
                       (int) ((float) (box.location.bottom - roi.packedLocation.second) /
                              roi.scale) + roi.location.top));
          int intersection = paddedRoIPos.intersection(movedAndResizedBoxPos);
          float overlapRatio = std::max((float) intersection / (float) paddedRoIPos.area(),
                                        (float) intersection /
                                        (float) movedAndResizedBoxPos.area());
          if (maxOverlap < overlapRatio) {
            maxOverlap = overlapRatio;
            maxBoxPos = movedAndResizedBoxPos;
            maxFrame = frame;
          }
        }
      }
    }
    if (maxFrame != nullptr && maxOverlap > useIoUThreshold) {
      maxFrame->boxes.emplace_back(maxBoxPos, box.confidence, box.labelName);
    }
  }
  for (Frame*& frame : mixedFrame.packedFrames) {
    frame->isResultReady.store(true);
  }
}

void PatchReconstructor::updateRoIInferenceResults(MixedFrame& mixedFrame) {
  for (Frame*& frame : mixedFrame.packedFrames) {
    for (RoI& roi : frame->rois) {
      for (const BoundingBox& box : roi.boxes) {
        frame->boxes.emplace_back(
            Rect(box.location.left + roi.location.left,
                 box.location.top + roi.location.top,
                 box.location.right + roi.location.left,
                 box.location.bottom + roi.location.top),
            box.confidence, box.labelName);
      }
    }
    frame->isResultReady.store(true);
  }
}

void PatchReconstructor::enqueue(const MixedFrame item) {
  LOGD("PatchReconstructor::enqueue(%d)", item.mixedFrameIndex);
  std::unique_lock<std::mutex> lock(mItemsMtx);
  mItemsCV.wait(lock, [this] {
    return mItems.size() < mMaxNumItems;
  });
  mItems.push(item);
  lock.unlock();
  mItemsCV.notify_all();
  LOGD("PatchReconstructor::enqueue(%d) end", item.mixedFrameIndex);
}

MixedFrame PatchReconstructor::takeItem() {
  LOGD("PatchReconstructor::takeItem()");
  std::unique_lock<std::mutex> lock(mItemsMtx);
  mItemsCV.wait(lock, [this] {
    return !mItems.empty();
  });
  MixedFrame item = mItems.front();
  mItems.pop();
  lock.unlock();
  mItemsCV.notify_all();
  LOGD("PatchReconstructor::takeItem() end : %d", item.mixedFrameIndex);
  return item;
}

} // namespace rm

