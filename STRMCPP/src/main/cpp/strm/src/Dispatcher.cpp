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
                       PatchMixer* patchMixer,
                       Logger* logger)
    : mLogger(logger),
      mKey(key),
      mTag(key.substr(key.size() - 8)),
      mConfig(config),
      mRoIExtractor(new RoIExtractor(roIExtractorConfig, resizeProfile, roIPrioritizer)),
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
}

Dispatcher::~Dispatcher() {
  isClosed.store(true);
  mThread.join();
}

int Dispatcher::enqueue(const cv::Mat& mat) {
  LOGD("Dispatcher%s::enqueue(Mat(%d, %d, %d))", mTag.c_str(), mat.cols, mat.rows, mat.channels());
  const time_us enqueueTime = NowMicros();
  std::unique_lock<std::mutex> lock(mFramesMtx);
  mFramesCv.wait(lock, [this] {
    return mEnqueuedFrameIndex - mProcessedFrameIndex < mMaxNumItems;
  });
  int frameIndex = mEnqueuedFrameIndex++;
  mFrames.insert(std::make_pair(
      frameIndex, std::make_shared<Frame>(mKey, frameIndex, mat, enqueueTime)));
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
  currFrame->dispatcherProcessStartTime = NowMicros();
  if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
    // # Full Frame Inference
    mCountMixedFrameInference = 0;
    mUseInferenceResults = true;
    currFrame->fullFrameEnqueueTime = NowMicros();
    int handle = mInferenceEngine->enqueue(currFrame->mat, true);
    std::vector<RoI> emptyRoIs;
    std::vector<BoundingBox> results = assignIdsToBoxes(mInferenceEngine->getResults(handle),
                                                        mPrevFrame == nullptr ? emptyRoIs : mPrevFrame->origRoIs,
                                                        0.8);
    /*
    std::vector<BoundingBox> results = mInferenceEngine->getResults(handle);
                                                        */
    currFrame->fullFrameGetResultsTime = NowMicros();
    currFrame->boxes.insert(currFrame->boxes.end(), results.begin(), results.end());
    currFrame->isResultReady.store(true);
    notifyResults();
  } else {
    // # Mixed Frame Inference
    auto roiPair = mRoIExtractor->process(mPrevFrame.get(), currFrame.get(),
                                          getPrevBoxes(mUseInferenceResults));
    currFrame->origRoIs = roiPair.first;
    currFrame->rois = roiPair.second;

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

std::vector<BoundingBox>
Dispatcher::assignIdsToBoxes(const std::vector<BoundingBox> &boxes, std::vector<RoI>& rois,
                             float overlapThreshold) {
  std::vector<BoundingBox> unassignedBoxes;
  std::vector<BoundingBox> assignedBoxes;

  // Match Boxes to RoI. Boxes can be unmatched. (if overlap ratio lower than threshold)
  for (const BoundingBox& box : boxes) {
    float maxOverlap = -1;
    RoI* maxRoI = nullptr;
    for (RoI& roi : rois) {
      int intersection = roi.location.intersection(box.location);
      assert(box.location.area() != 0);
      float overlapRatio = (float) intersection / (float) box.location.area();
      if (maxOverlap < overlapRatio) {
        maxOverlap = overlapRatio;
        maxRoI = &roi;
      }
    }
    if (maxRoI != nullptr && maxOverlap >= overlapThreshold) {
      maxRoI->boxes.emplace_back(0, box.location, box.confidence, box.labelName);
    } else {
      unassignedBoxes.emplace_back(0, box.location, box.confidence, box.labelName);
    }
  }

  // Let RoIs find their most well corresponding Box
  for (RoI& roi : rois) {
    int maxIntersection = -1;
    int maxIndex = -1;
    for (int i = 0; i<roi.boxes.size(); ++i) {
      BoundingBox& box = roi.boxes[i];
      int intersection = roi.location.intersection(box.location);
      if (maxIntersection < intersection) {
        maxIntersection = intersection;
        maxIndex = i;
      }
    }
    if (maxIndex != -1) {
      BoundingBox& box = roi.boxes[maxIndex];
      assignedBoxes.emplace_back(roi.id, box.location, box.confidence, box.labelName);
      for (int i = 0; i<roi.boxes.size(); ++i) {
        if (i == maxIndex) continue;
        unassignedBoxes.emplace_back(0, box.location, box.confidence, box.labelName);
      }
    }
  }

  // If unassigned Boxes exist (1. those who does not match with any RoI 2. those who lost competition between other Boxes in single RoI),
  // classify them as newly appeared objects and assign new Id
  if (!unassignedBoxes.empty()) {
    std::pair<idType, idType> idRange = RoI::getNewIds(unassignedBoxes.size());
    idType id = idRange.first;
    for (const BoundingBox& box : unassignedBoxes) {
      assert(id < idRange.second);
      assignedBoxes.emplace_back(id++, box.location, box.confidence, box.labelName);
    }
  }

  return assignedBoxes;
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
      prevResults.emplace_back(box.id, Rect(
          box.location.left - mConfig.ROI_PADDING,
          box.location.top - mConfig.ROI_PADDING,
          box.location.right + mConfig.ROI_PADDING,
          box.location.bottom + mConfig.ROI_PADDING), box.confidence, box.labelName, box.targetSize);
    }
  } else {
    for (const RoI& roi : mPrevFrame->opticalFlowRoIs) {
      prevResults.emplace_back(roi.id, roi.location, 1.0, roi.labelName);
    }
    for (const RoI& roi : mPrevFrame->pixelDiffRoIs) {
      prevResults.emplace_back(roi.id, roi.location, 1.0, roi.labelName);
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
