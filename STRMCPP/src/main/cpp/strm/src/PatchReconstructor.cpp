#include "strm/PatchReconstructor.hpp"

#include "strm/Log.hpp"
#include "strm/Utils.hpp"

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
}

PatchReconstructor::~PatchReconstructor() {
  isClosed.store(true);
  mThread.join();
}

void PatchReconstructor::process(MixedFrame& mixedFrame) {
  LOGD("PatchReconstructor::process(%d) %lu frames packed", mixedFrame.mixedFrameIndex,
       mixedFrame.packedFrames.size());
  bool isMixedInference = !mixedFrame.packedMat.empty();
  if (isMixedInference) {
    mixedFrame.boxes = mInferenceEngine->getResults(mixedFrame.handle);
  } else {
    for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
      for (RoI& roi : frame->rois) {
        if (roi.isPacked()) {
          assert(roi.handle != -1);
          roi.boxes = mInferenceEngine->getResults(roi.handle);
        }
      }
    }
  }
  time_us reconstructStartTime = NowMicros();
  if (isMixedInference) {
    updateMixedFrameInferenceResults(mixedFrame, mConfig.OVERLAP_THRESHOLD);
  } else {
    updateRoIInferenceResults(mixedFrame);
  }
  for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
    frame->boxes = nms(frame->boxes, NUM_LABELS, mConfig.FRAME_BOXES_IOU_THRESHOLD);
  }
  time_us reconstructEndTime = NowMicros();
  for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
    frame->reconstructStartTime = reconstructStartTime;
    frame->reconstructEndTime = reconstructEndTime;
  }
}

void PatchReconstructor::updateMixedFrameInferenceResults(MixedFrame& mixedFrame,
                                                          float overlapThreshold) {
  for (const BoundingBox& box : mixedFrame.boxes) {
    float maxOverlap = -1;
    Rect maxBoxPos;
    Frame* maxFrame = nullptr;
    for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
      for (RoI& roi : frame->rois) {
        if (roi.isPacked()) {
          Rect movedAndResizedBoxPos(
              std::max(0,
                       (box.location.left - roi.packedLocation.first)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.left),
              std::max(0,
                       (box.location.top - roi.packedLocation.second)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.top),
              std::min(roi.frame->mat.cols,
                       (box.location.right - roi.packedLocation.first)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.left),
              std::min(roi.frame->mat.rows,
                       (box.location.bottom - roi.packedLocation.second)
                       * roi.maxEdgeLength / roi.targetSize + roi.location.top));
          int intersection = roi.location.intersection(movedAndResizedBoxPos);
          float overlapRatio = (float) intersection / (float) movedAndResizedBoxPos.area();
          if (maxOverlap < overlapRatio) {
            maxOverlap = overlapRatio;
            maxBoxPos = movedAndResizedBoxPos;
            maxFrame = frame.get();
          }
        }
      }
    }
    if (maxFrame != nullptr && maxOverlap >= overlapThreshold) {
      maxFrame->boxes.emplace_back(maxBoxPos, box.confidence, box.labelName);
    }
  }
  for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
    frame->isResultReady.store(true);
  }
}

void PatchReconstructor::updateRoIInferenceResults(MixedFrame& mixedFrame) {
  for (const std::shared_ptr<Frame>& frame : mixedFrame.packedFrames) {
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

void PatchReconstructor::enqueue(const MixedFrame& item) {
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

