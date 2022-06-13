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

void PatchReconstructor::updateMixedFrameInferenceResults(MixedFrame& mixedFrame, float overlapThreshold) {

  bool runOriginalCode = false;

  for (const BoundingBox& box : mixedFrame.boxes) {
    float maxOverlap = -1;
    Rect maxBoxPos;
    RoI *maxRoI = nullptr;
    Frame* maxFrame = nullptr;
    for (const std::shared_ptr<Frame> &frame : mixedFrame.packedFrames) {
      for (RoI &roi : frame->rois) {
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
            maxRoI = &roi;
            maxFrame = frame.get();
          }
        }
      }
    }
    if (runOriginalCode) {
      if (maxFrame != nullptr && maxOverlap >= overlapThreshold) {
        maxFrame->boxes.emplace_back(0, maxBoxPos, box.confidence, box.labelName, maxRoI->targetSize);
      }
    } else {
      if (maxRoI != nullptr && maxOverlap >= overlapThreshold) {
        maxRoI->boxes.emplace_back(0, maxBoxPos, box.confidence, box.labelName, maxRoI->targetSize);
      }
    }
  }

  // >>> for debugging
  std::vector<int> boxNum;
  if (runOriginalCode) {
    for (const std::shared_ptr<Frame> &frame : mixedFrame.packedFrames) {
      boxNum.push_back(frame->boxes.size());
    }
  }
  // <<<

  if (!runOriginalCode) {
    std::vector<BoundingBox> unassignedBoxes;
    for (const std::shared_ptr<Frame> &frame : mixedFrame.packedFrames) {
      for (RoI &roi : frame->rois) {
        if (roi.isPacked()) {
          int maxIntersection = -1;
          int maxIndex = -1;
          for (int i = 0; i < roi.boxes.size(); ++i) {
            BoundingBox &box = roi.boxes[i];
            int intersection = roi.location.intersection(box.location);
            if (maxIntersection < intersection) {
              maxIntersection = intersection;
              maxIndex = i;
            }
          }
          if (maxIndex != -1) {
            BoundingBox &box = roi.boxes[maxIndex];
            frame->boxes.emplace_back(roi.id, box.location, box.confidence, box.labelName,
                                      box.targetSize);
            for (int i = 0; i < roi.boxes.size(); ++i) {
              if (i == maxIndex) continue;
              unassignedBoxes.emplace_back(0, box.location, box.confidence, box.labelName,
                                           box.targetSize);
            }
          }
        }
      }

      // If new Boxes exist (those who lost competition between other Boxes in single RoI),
      // classify them as newly appeared objects and assign new Id
      if (!unassignedBoxes.empty()) {
        std::pair<idType, idType> idRange = RoI::getNewIds(unassignedBoxes.size());
        idType id = idRange.first;
        for (const BoundingBox& box : unassignedBoxes) {
          assert(id < idRange.second);
          frame->boxes.emplace_back(id++, box.location, box.confidence, box.labelName, box.targetSize);
        }
      }
      unassignedBoxes.clear();
    }

    // >>> for debugging
    for (const std::shared_ptr<Frame> &frame : mixedFrame.packedFrames) {
      boxNum.push_back(frame->boxes.size());
    }
    // <<<
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
            roi.id,
            Rect(box.location.left + roi.location.left,
                 box.location.top + roi.location.top,
                 box.location.right + roi.location.left,
                 box.location.bottom + roi.location.top),
            box.confidence, box.labelName, box.targetSize);
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

