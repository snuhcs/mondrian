#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/MixedFrame.hpp"
#include "mondrian/Test.hpp"

namespace md {

const cv::TermCriteria ROIExtractor::CRITERIA = cv::TermCriteria(
    cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);

ROIExtractor::ROIExtractor(const ROIExtractorConfig& config, int maxMergeSize, bool run,
                           ROIResizer* roiResizer, bool emulatedBatch, int roiSize,
                           std::vector<InferenceInfo> inferencePlan, std::set<int> vids)
    : mConfig(config), mMaxMergeSize(emulatedBatch ? roiSize : maxMergeSize), mROIResizer(roiResizer),
      mEmulatedBatch(emulatedBatch), mROISize(roiSize),
      mTargetSize(cv::Size(int(config.EXTRACTION_RESIZE_WIDTH),
                           int(config.EXTRACTION_RESIZE_HEIGHT))),
      mInferencePlan(std::move(inferencePlan)),
      mFullFrameInferenceCount(0), mFullFrameTarget(nullptr), mFullFrameVid(-1),
      mVids(std::move(vids)), mbStop(false), notFullyPacked(true) {
  if (run) {
    resetPatchMixerWithPlan(mInferencePlan);
    mThreads.reserve(config.NUM_WORKERS);
    for (int extractorId = 0; extractorId < config.NUM_WORKERS; extractorId++) {
      mThreads.emplace_back([this, extractorId]() { work(extractorId); });
    }
  }
}

ROIExtractor::~ROIExtractor() {
  mbStop = true;
  queueCv.notify_all();
  for (auto& thread : mThreads) {
    thread.join();
  }
}

void ROIExtractor::enqueue(Frame* frame) {
  std::unique_lock<std::mutex> queueLock(queueMtx);
  queueCv.wait(queueLock, [this]() { return mPDWaiting.size() < mConfig.MAX_QUEUE_SIZE; });
  mPDWaiting.insert(frame);
  LOGD("%-25s                 for video %-5d frame %-4d // %4lu PD | %4lu OF | %4d Processed",
       "ROIExtractor::enqueue", frame->vid, frame->frameIndex,
       mPDWaiting.size(), mOFWaiting.size(),
       std::accumulate(mPackedFrames.begin(), mPackedFrames.end(), 0,
                       [](int cnt, const auto& it) { return cnt + it.second.size(); }));
  queueLock.unlock();
  queueCv.notify_all();
}

void ROIExtractor::notify() {
  queueCv.notify_all();
}

std::tuple<std::vector<MixedFrame>, Frame*, MultiStream, Stream> ROIExtractor::prepareInference(
    std::vector<InferenceInfo>& nextInferencePlan, bool runFull, int scheduleID) {
  // Should be packLock => queueLock order.
  // See postprocessOF() for detail
  std::unique_lock<std::mutex> packLock(packMtx);
  std::unique_lock<std::mutex> queueLock(queueMtx);

  if (notFullyPacked) {
    applyLasts();
  }

  Frame* fullFrameTarget = mFullFrameTarget;

  std::map<int, std::set<ROI*>> groupedROIs;
  for (const auto&[vid, frames]: mPackedFrames) {
    for (Frame* frame: frames) {
      assert(frame != fullFrameTarget);
      for (auto& pROI: frame->parentROIs) {
        groupedROIs[pROI->getPackedMixedFrameIndex()].insert(pROI.get());
      }
      for (auto& probeROI: frame->probingROIs) {
        groupedROIs[probeROI->getPackedMixedFrameIndex()].insert(probeROI.get());
      }
    }
  }

  std::vector<MixedFrame> mixedFrames;
  for (auto&[mixedFrameIndex, rois]: groupedROIs) {
    assert(mixedFrameIndex < mInferencePlan.size());
    const auto& info = mInferencePlan[mixedFrameIndex];
    if (!rois.empty()) {
      mixedFrames.emplace_back(info.device, rois, info.size);
    }
  }

  MultiStream selectedFrames = std::move(mPackedFrames);
  mPackedFrames.clear();
  notFullyPacked = true;
  mCandidateLastFrames.clear();

  Stream droppedFrames;

  time_us scheduledTime = NowMicros();
  for (auto&[vid, frames]: selectedFrames) {
    for (auto& frame: frames) {
      frame->scheduledTime = scheduledTime;
      frame->scheduleID = scheduleID;
      frame->useInferenceResultForOF = true;
    }
  }
  if (fullFrameTarget != nullptr) {
    fullFrameTarget->scheduledTime = scheduledTime;
    fullFrameTarget->scheduleID = scheduleID;
    fullFrameTarget->useInferenceResultForOF = true;
  }

  for (Frame* frame: mOFProcessing) {
    frame->extractOFAgain = true;
  }

  if (runFull) {
    int index = mFullFrameInferenceCount % int(mVids.size());
    mFullFrameInferenceCount++;
    mFullFrameVid = *std::next(mVids.begin(), index);
  } else {
    mFullFrameVid = -1;
  }
  mFullFrameTarget = nullptr;
  mInferencePlan = nextInferencePlan;
  resetPatchMixerWithPlan(mInferencePlan);

  queueLock.unlock();
  packLock.unlock();
  queueCv.notify_all();

  return {mixedFrames, fullFrameTarget, selectedFrames, droppedFrames};
}

void ROIExtractor::work(int extractorId) {
  /*
   *    Frame Status           Containing data structure
   * 1. Before PD extraction | mPDWaiting
   * 2. Extracting PD        | -
   * 3. Before OF extraction | mOFWaiting
   * 4. Extracting OF        | mOFProcessing
   * 5. OF extraction ended  | mPackedFrames
   */

  auto getPDJob = [this]() {
    if (!mPDWaiting.empty() && mOFWaiting.size() < mConfig.MAX_QUEUE_SIZE) {
      return *mPDWaiting.begin();
    } else {
      return (Frame*) nullptr;
    }
  };
  auto getOFJob = [this]() {
    if (!mOFWaiting.empty()
        && notFullyPacked
        && (*mOFWaiting.begin())->readyForOFExtraction()) {
      return *mOFWaiting.begin();
    } else {
      return (Frame*) nullptr;
    }
  };

  while (true) {
    bool isOF = false;
    Frame* frame = nullptr;

    std::unique_lock<std::mutex> queueLock(queueMtx);
    queueCv.wait(queueLock, [this, &isOF, &frame, &getPDJob, &getOFJob]() {
      if (mbStop) {
        return true;
      }
      frame = getOFJob();
      if (frame != nullptr) {
        isOF = true;
        return true;
      }
      frame = getPDJob();
      if (frame != nullptr) {
        isOF = false;
        return true;
      }
      return false;
    });

    if (mbStop) {
      queueLock.unlock();
      queueCv.notify_all();
      return;
    }

    if (isOF) {
      frame->OFExtractorID = extractorId;
      mOFWaiting.erase(mOFWaiting.begin());
      mOFProcessing.insert(frame);
    } else {
      frame->PDExtractorID = extractorId;
      mPDWaiting.erase(mPDWaiting.begin());
    }
    queueLock.unlock();
    queueCv.notify_all();

    if (isOF) {
      processOF(frame);
    } else {
      processPD(frame);
    }

    if (isOF) {
      postprocessOF(frame);
    } else {
      queueLock.lock();
      mOFWaiting.insert(frame);
      queueLock.unlock();
    }
    queueCv.notify_all();
  }
}

void ROIExtractor::postprocessOF(Frame* currFrame) {
  currFrame->filterPDROIs(mConfig.PD_FILTER_THRESHOLD, mConfig.EAT_PD);
  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeROIs(mROIResizer, mEmulatedBatch, mROISize);
  currFrame->resizeEndTime = NowMicros();
  currFrame->mergeROIStartTime = NowMicros();
  if (mConfig.MERGE) {
    currFrame->mergeROIs(float(mMaxMergeSize));
  }
  currFrame->mergeROIEndTime = NowMicros();

  currFrame->boxesIfLast = getBoxesIfLast(currFrame);
  currFrame->boxesIfScaled = getBoxesIfScaled(currFrame);

  if (mEmulatedBatch) {
    assert(std::all_of(currFrame->boxesIfLast.begin(), currFrame->boxesIfLast.end(),
                       [this](const auto& box) { return std::max(box.first, box.second) <= mROISize;}));
    assert(std::all_of(currFrame->boxesIfScaled.begin(), currFrame->boxesIfScaled.end(),
                       [this](const auto& box) { return std::max(box.first, box.second) <= mROISize;}));
  }

  currFrame->mixingStartTime = NowMicros();
  std::unique_lock<std::mutex> packLock(packMtx);
  tryPack(currFrame);
  packLock.unlock();
  currFrame->mixingEndTime = NowMicros();

  std::lock_guard<std::mutex> queueLock(queueMtx);
  mOFProcessing.erase(currFrame);
  if (currFrame->extractOFAgain) {
    currFrame->resetOFROIExtraction();
    mOFWaiting.insert(currFrame);
  } else {
    currFrame->isROIsReady = true;
  }
}

void ROIExtractor::tryPack(Frame* frame) {
  // TODO: Cache pack indices
  assert(frame->vid != -1);
  if (!notFullyPacked) {
    return;
  }

  if (frame->vid == mFullFrameVid) {
    notFullyPacked = tryPackFullVid(frame);
  } else {
    notFullyPacked = tryPackNonFullVid(frame);
  }

  if (!notFullyPacked) {
    applyLasts();
    std::lock_guard<std::mutex> queueLock(queueMtx);
    for (Frame* f: mOFProcessing) {
      f->extractOFAgain = true;
    }
  }
}

static auto appendLastBoxes = [](IntPairs& lastBoxes, Frame* frameToAppend) {
  lastBoxes.insert(lastBoxes.end(),
                   frameToAppend->boxesIfLast.begin(), frameToAppend->boxesIfLast.end());
};

bool ROIExtractor::tryPackFullVid(Frame* frame) {
  assert(mCandidateLastFrames.find(frame->vid) == mCandidateLastFrames.end());

  // If mFullFrameTarget is not set
  if (mFullFrameTarget == nullptr) {
    mFullFrameTarget = frame;
    return true;
  }

  // Try pack incoming frame as scaled
  auto copiedFreeRectsVec = mFreeRectsVec;
  auto[fullPackIndices, fullPackLocations] = PatchMixer::pack(
      copiedFreeRectsVec, mFullFrameTarget->boxesIfScaled, /*backward=*/true,
      mEmulatedBatch, mROISize);

  // If single scaled packing fails (rare case)
  if (fullPackIndices.size() != mFullFrameTarget->boxesIfScaled.size()) {
    assert(fullPackIndices.size() < mFullFrameTarget->boxesIfScaled.size());
    return false;
  }

  // Apply incoming frame and try pack last frame candidates
  PatchMixer::apply(copiedFreeRectsVec, mFullFrameTarget->boxesIfScaled, fullPackIndices,
                    mEmulatedBatch, mROISize);
  IntPairs lastBoxes;
  for (auto&[cVid, info]: mCandidateLastFrames) {
    appendLastBoxes(lastBoxes, info.frame);
  }
  auto[lastPackIndices, lastPackLocations] = PatchMixer::pack(
      copiedFreeRectsVec, lastBoxes, /*backward=*/false, mEmulatedBatch, mROISize);

  // If last frame candidates packing failed
  if (lastPackIndices.size() != lastBoxes.size()) {
    assert(lastPackIndices.size() < lastBoxes.size());
    return false;
  }

  // If last frame candidates packing success
  int i = 0;
  for (auto&[_, info]: mCandidateLastFrames) {
    int start = i;
    int end = i + int(info.frame->boxesIfLast.size());
    info.indices.clear();
    info.indices.insert(info.indices.end(),
                        lastPackIndices.begin() + start,
                        lastPackIndices.begin() + end);
    info.locations.clear();
    info.locations.insert(info.locations.end(),
                          lastPackLocations.begin() + start,
                          lastPackLocations.begin() + end);
    i = end;
  }
  assert(i == lastPackIndices.size());
  prepareScaledFrame(mFullFrameTarget, fullPackIndices, fullPackLocations);
  mPackedFrames[mFullFrameTarget->vid].insert(mFullFrameTarget);
  mFreeRectsVec = std::move(copiedFreeRectsVec);
  mFullFrameTarget = frame;
  return true;
}

bool ROIExtractor::tryPackNonFullVid(Frame* frame) {
  const int vid = frame->vid;
  bool vidExists = mCandidateLastFrames.find(frame->vid) != mCandidateLastFrames.end();
  auto copiedFreeRectsVec = mFreeRectsVec;

  // Try pack last candidate frames.
  // If candidateVid == frame->vid, pack candidateVid first as scaled
  std::pair<Indices, Locations> existPackIndicesLocations;
  if (vidExists) {
    existPackIndicesLocations = PatchMixer::pack(
        copiedFreeRectsVec, mCandidateLastFrames[vid].frame->boxesIfScaled, /*backward=*/true,
        mEmulatedBatch, mROISize);
    PatchMixer::apply(copiedFreeRectsVec,
                      mCandidateLastFrames[vid].frame->boxesIfScaled,
                      existPackIndicesLocations.first, mEmulatedBatch, mROISize);
  } else {
    // Temporarily add. Erase if packing fails
    mCandidateLastFrames[vid] = LastPackInfo();
  }

  IntPairs lastBoxes;
  for (auto&[cVid, info]: mCandidateLastFrames) {
    if (cVid == vid) {
      appendLastBoxes(lastBoxes, frame);
    } else {
      appendLastBoxes(lastBoxes, info.frame);
    }
  }
  auto[lastPackIndices, lastPackLocations] = PatchMixer::pack(
          copiedFreeRectsVec, lastBoxes, /*backward=*/false,
          mEmulatedBatch, mROISize);

  if (lastPackIndices.size() != lastBoxes.size()) {
    assert(lastPackIndices.size() < lastBoxes.size());
    if (!vidExists) {
      mCandidateLastFrames.erase(vid);
    }
    return false;
  }

  // If last frame candidates packing success
  if (vidExists) {
    auto& info = mCandidateLastFrames[vid];
    prepareScaledFrame(info.frame,
                       existPackIndicesLocations.first,
                       existPackIndicesLocations.second);
  }
  mCandidateLastFrames[vid].frame = frame;
  int i = 0;
  for (auto&[_, info]: mCandidateLastFrames) {
    int start = i;
    int end = i + int(info.frame->boxesIfLast.size());
    info.indices.clear();
    info.indices.insert(info.indices.end(),
                        lastPackIndices.begin() + start,
                        lastPackIndices.begin() + end);
    info.locations.clear();
    info.locations.insert(info.locations.end(),
                          lastPackLocations.begin() + start,
                          lastPackLocations.begin() + end);
    i = end;
  }
  assert(i == lastPackIndices.size());
  mPackedFrames[vid].insert(frame);
  mFreeRectsVec = std::move(copiedFreeRectsVec);
  return true;
}

void ROIExtractor::applyLasts() {
  for (auto&[pVid, info]: mCandidateLastFrames) {
    assert(info.indices.size() == info.locations.size());
    PatchMixer::apply(mFreeRectsVec, info.frame->boxesIfLast, info.indices,
                      mEmulatedBatch, mROISize);
  }
  for (auto&[cVid, info]: mCandidateLastFrames) {
    prepareFrameLast(info.frame, info.indices, info.locations);
  }
}

IntPairs ROIExtractor::getBoxesIfLast(const Frame* frame) {
  // TODO: Synchronize simulation with add logics
  IntPairs boxesIfLast;
  for (const auto& pROI: frame->parentROIs) {
    // TODO: Make below two condition as single value(or function) of condition
    if (!mEmulatedBatch && mConfig.NO_DOWNSAMPLING_FOR_LAST_FRAME) {
      boxesIfLast.push_back(pROI->getBorderMatWidthHeight(1.0f));
    } else {
      boxesIfLast.push_back(pROI->getBorderMatWidthHeight());
    }
  }
  for (const auto& cROI: frame->childROIs) {
    if (cROI->getScaleLevel() == ROIResizer::INVALID_LEVEL) {
      continue;
    }
    mROIResizer->getProbingCandidates(cROI.get());
    for (auto scale: cROI->probeScales) {
      boxesIfLast.push_back(cROI->getBorderMatWidthHeight(scale));
    }
  }
  return boxesIfLast;
}

void ROIExtractor::prepareFrameLast(Frame* frame,
                                    const Indices& indices, const Locations& locations) {
  assert(indices.size() == locations.size());
  frame->isLastFrame = true;
  frame->resetProbeROIs();
  int i = 0;
  for (const auto& pROI: frame->parentROIs) {
    if (!mEmulatedBatch && mConfig.NO_DOWNSAMPLING_FOR_LAST_FRAME) {
      pROI->setTargetScale(1.0f, ROIResizer::INVALID_LEVEL);
    }
    pROI->setPackInfo(locations[i], indices[i].first, mEmulatedBatch, mROISize);
    i++;
  }
  for (const auto& cROI: frame->childROIs) {
    if (cROI->getScaleLevel() == ROIResizer::INVALID_LEVEL) {
      i += int(cROI->probeScales.size());
      continue;
    }
    for (auto scale: cROI->probeScales) {
      assert(0.0f < scale && scale <= 1.0f);
      std::unique_ptr<ROI> probeROI = std::make_unique<ROI>(
              nullptr, cROI->id, cROI->frame, cROI->paddedLoc, cROI->type, cROI->origin, cROI->label,
              cROI->features.ofFeatures, ROI::INVALID_CONF, 0, mConfig.ROI_BORDER, true);
      probeROI->setTargetScale(scale, cROI->getScaleLevel());
      probeROI->setPackInfo(locations[i], indices[i].first, mEmulatedBatch, mROISize);
      cROI->roisForProbing.push_back(probeROI.get());
      frame->probingROIs.push_back(std::move(probeROI));
      i++;
    }
  }
  assert(i == locations.size());
}

IntPairs ROIExtractor::getBoxesIfScaled(const Frame* frame) {
  // TODO: Synchronize simulation with add logics
  IntPairs BoxesIfIntermediate;
  for (const auto& pROI: frame->parentROIs) {
    BoxesIfIntermediate.push_back(pROI->getBorderMatWidthHeight());
  }
  return BoxesIfIntermediate;
}

void ROIExtractor::prepareScaledFrame(Frame* frame,
                                      const Indices& indices, const Locations& locations) {
  assert(indices.size() == locations.size());
  frame->resetProbeROIs();
  int i = 0;
  for (const auto& pROI: frame->parentROIs) {
    auto[bw, bh] = pROI->getBorderMatWidthHeight();
    pROI->setPackInfo(locations[i], indices[i].first, mEmulatedBatch, mROISize);
    i++;
  }
  assert(i == locations.size());
}

void ROIExtractor::processPD(Frame* currFrame) {
  currFrame->pixelDiffROIProcessStartTime = NowMicros();
  getPixelDiffROIs(currFrame, mTargetSize,
                   mConfig.MAX_PD_ROI_SIZE, mConfig.MIN_PD_ROI_SIZE,
                   currFrame->childROIs);
  currFrame->pixelDiffROIProcessEndTime = NowMicros();
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu PD ROIs",
       "ROIExtractor::processPD",
       currFrame->pixelDiffROIProcessEndTime - currFrame->pixelDiffROIProcessStartTime,
       currFrame->vid, currFrame->frameIndex, currFrame->childROIs.size());
}

void ROIExtractor::processOF(Frame* currFrame) {
  assert(!currFrame->extractOFAgain);
  Frame* prevFrame = currFrame->prevFrame;
  std::vector<BoundingBox> reliablePrevBoxes;
  if (prevFrame->useInferenceResultForOF) {
    testAssignedUniqueBoxID(prevFrame->boxes);
    for (const std::unique_ptr<BoundingBox>& box : prevFrame->boxes) {
      if (box->confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        BoundingBox reliableBox(box->id, Rect(
            std::max(0.0f, box->location.left),
            std::max(0.0f, box->location.top),
            std::min(float(currFrame->width), box->location.right),
            std::min(float(currFrame->height), box->location.bottom)),
                                box->confidence, box->label, origin_BB);
        reliableBox.srcROI = box->srcROI;
        reliablePrevBoxes.push_back(reliableBox);
      }
    }
  } else {
    for (auto& cROI : currFrame->prevFrame->childROIs) {
      if (cROI->type == PD) {
        continue;
      }
      BoundingBox reliableBox(cROI->id, cROI->origLoc, 1, cROI->label, cROI->origin);
      reliableBox.srcROI = cROI.get();
      reliablePrevBoxes.push_back(reliableBox);
    }
  }
  currFrame->opticalFlowROIProcessStartTime = NowMicros();
  getOpticalFlowROIs(prevFrame, currFrame, reliablePrevBoxes, mTargetSize, currFrame->childROIs);
  currFrame->opticalFlowROIProcessEndTime = NowMicros();
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu OF ROIs", "ROIExtractor::processOF",
       currFrame->opticalFlowROIProcessEndTime - currFrame->opticalFlowROIProcessStartTime,
       currFrame->vid, currFrame->frameIndex,
       std::count_if(currFrame->childROIs.begin(), currFrame->childROIs.end(),
                     [](auto& cROI) { return cROI->type == OF; }));
}

void ROIExtractor::getOpticalFlowROIs(const Frame* prevFrame, Frame* currFrame,
                                      const std::vector<BoundingBox>& boundingBoxes,
                                      const cv::Size& targetSize,
                                      std::vector<std::unique_ptr<ROI>>& outChildROIs) const {
  auto width = float(currFrame->rgbMat.cols);
  auto height = float(currFrame->rgbMat.rows);

  std::vector<Rect> boundingRects;
  boundingRects.reserve(boundingBoxes.size());
  for (const auto& bbx : boundingBoxes) {
    boundingRects.push_back(Rect(bbx.location));
  }

  if (!boundingBoxes.empty()) {
    const std::vector<OFFeatures>& ofFeatures = opticalFlowTracking(
        prevFrame, currFrame, boundingRects, targetSize);
    assert(ofFeatures.size() == boundingBoxes.size());
    for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
      const BoundingBox& box = boundingBoxes[boxIndex];
      const Rect& loc = box.location;
      const OFFeatures& of = ofFeatures[boxIndex];
      float x = of.shiftAvg.first;
      float y = of.shiftAvg.second;
      float newLeft = std::max(0.0f, loc.left + x);
      float newTop = std::max(0.0f, loc.top + y);
      float newRight = std::min(float(width), loc.right + x);
      float newBottom = std::min(float(height), loc.bottom + y);
      if (newRight - newLeft >= 1.0f && newBottom - newTop >= 1.0f) {
        outChildROIs.push_back(std::make_unique<ROI>(
            box.srcROI, box.id, currFrame, Rect(newLeft, newTop, newRight, newBottom),
            OF, box.origin, box.label, of, box.confidence,
            mConfig.ROI_PADDING, mConfig.ROI_BORDER, false));
      }
    }
  }
}

std::vector<OFFeatures> ROIExtractor::opticalFlowTracking(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->resizedGrayMat.empty());
  assert(!currFrame->resizedGrayMat.empty());
  assert(prevFrame->resizedGrayMat.channels() == currFrame->resizedGrayMat.channels());

  const cv::Mat& prevImage = prevFrame->resizedGrayMat;
  const cv::Mat& currImage = currFrame->resizedGrayMat;

  std::vector<int> startEndIndices = {0};
  std::vector<cv::Point2f> inputPoints;
  for (const Rect& bbx: boundingBoxes) {
    float xRatio = (float) targetSize.width / (float) prevFrame->width;
    float yRatio = (float) targetSize.height / (float) prevFrame->height;
    float x = std::min(bbx.left, bbx.right) * xRatio;
    float y = std::min(bbx.top, bbx.bottom) * yRatio;
    float w = std::abs(bbx.right - bbx.left) * xRatio;
    float h = std::abs(bbx.bottom - bbx.top) * yRatio;
    x = std::min(std::max(0.0f, x), float(prevImage.cols));
    y = std::min(std::max(0.0f, y), float(prevImage.rows));
    w = std::min(std::max(0.0f, w), float(prevImage.cols) - x);
    h = std::min(std::max(0.0f, h), float(prevImage.rows) - y);

    std::vector<cv::Point2f> points;
    cv::Rect roiBbx = cv::Rect(int(x), int(y), int(w), int(h));
    cv::goodFeaturesToTrack(prevImage(roiBbx), points, 50, 0.01, 5, cv::Mat(), 3, false, 0.03);
    for (cv::Point2f& p: points) {
      p.x += x;
      p.y += y;
    }
    if (points.empty()) {
      startEndIndices.push_back(startEndIndices.back() + 1);
      inputPoints.push_back(cv::Point2f(
          ((float) bbx.left + (float) bbx.width() / 2) * xRatio,
          ((float) bbx.top + (float) bbx.height() / 2) * yRatio));
    } else {
      startEndIndices.push_back(startEndIndices.back() + int(points.size()));
      inputPoints.insert(inputPoints.end(), points.begin(), points.end());
    }
  }
  assert(startEndIndices.back() == inputPoints.size());

  std::vector<uchar> status;
  std::vector<float> errs;
  std::vector<cv::Point2f> outputPoints;
  cv::calcOpticalFlowPyrLK(prevImage, currImage, inputPoints, outputPoints, status, errs,
                           cv::Size(15, 15), 2, CRITERIA);
  assert(inputPoints.size() == outputPoints.size());
  assert(inputPoints.size() == status.size());
  assert(inputPoints.size() == errs.size());

  std::vector<OFFeatures> ofFeatures;
  for (int i = 0; i < startEndIndices.size() - 1; i++) {
    int startIndex = startEndIndices[i];
    int endIndex = startEndIndices[i + 1];
    std::vector<std::pair<float, float>> boxShifts;
    std::vector<float> boxErrs;
    std::vector<uchar> boxStatusVec;
    for (int j = startIndex; j < endIndex; j++) {
      float x = outputPoints[j].x - inputPoints[j].x;
      float y = outputPoints[j].y - inputPoints[j].y;
      boxShifts.push_back({x * (float) currFrame->width / (float) targetSize.width,
                           y * (float) currFrame->height / (float) targetSize.height});
      boxErrs.push_back(errs[j]);
      boxStatusVec.push_back(status[j]);
    }
    ofFeatures.push_back(OFFeatures(boxShifts, boxErrs, boxStatusVec));
  }
  return ofFeatures;
}

void ROIExtractor::getPixelDiffROIs(Frame* currFrame, const cv::Size& targetSize,
                                    const float maxPDROISize, const float minPDROISize,
                                    std::vector<std::unique_ptr<ROI>>& outChildROIs) const {

  // Find {PD_INTERVAL}th previous frame. If not available, use farthest frame.
  Frame* prevFrame = currFrame;
  for (int i = 0; i < mConfig.PD_INTERVAL; ++i) {
    assert(prevFrame != nullptr);
    if (prevFrame->prevFrame == nullptr) {
      break;
    }
    prevFrame = prevFrame->prevFrame;
  }

  assert(!prevFrame->resizedGrayMat.empty());
  assert(!currFrame->resizedGrayMat.empty());
  assert(prevFrame->resizedGrayMat.channels() == currFrame->resizedGrayMat.channels());
  cv::Mat mat = calculateDiffAndThreshold(prevFrame->resizedGrayMat, currFrame->resizedGrayMat);
  cannyEdgeDetection(mat);

  std::vector<std::vector<cv::Point>> contours;
  cv::Mat hierarchy;

  cv::findContours(mat, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // replaces get boxes from contours.
  std::vector<Rect> boxes;
  for (const std::vector<cv::Point>& contour : contours) {
    double approxDistance = cv::arcLength(contour, true) * 0.02;
    std::vector<cv::Point> approxCurve;
    cv::approxPolyDP(contour, approxCurve, approxDistance, true);
    cv::Rect2f box = cv::boundingRect(approxCurve);
    if (minPDROISize <= std::min(box.width, box.height)
        && std::max(box.width, box.height) <= maxPDROISize) {
      boxes.push_back(Rect(
              box.x * float(currFrame->rgbMat.cols) / float(targetSize.width),
              box.y * float(currFrame->rgbMat.rows) / float(targetSize.height),
              (box.x + box.width) * float(currFrame->rgbMat.cols) / float(targetSize.width),
              (box.y + box.height) * float(currFrame->rgbMat.rows) / float(targetSize.height)));
    }
  }

  for (const Rect& box: boxes) {
    if (std::min(box.width(), box.height()) >= 1.0f) {
      outChildROIs.push_back(std::make_unique<ROI>(
              nullptr, UNASSIGNED_ID, currFrame, box,
              PD, origin_PD, -1, OFFeatures({}, {}, {}), ROI::INVALID_CONF,
              mConfig.ROI_PADDING, mConfig.ROI_BORDER, false));
    }
  }
}

cv::Mat ROIExtractor::calculateDiffAndThreshold(
    const cv::Mat& prevMat, const cv::Mat& currMat) {
  cv::Mat diff;
  cv::absdiff(prevMat, currMat, diff);
  cv::dilate(diff, diff,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(-1, -1),
             2);
  cv::threshold(diff, diff, 35, 255, cv::THRESH_BINARY);
  return diff;
}

void ROIExtractor::cannyEdgeDetection(cv::Mat mat) {
  cv::Canny(mat, mat, 120, 255, 3, false);
  cv::dilate(mat, mat,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(-1, -1),
             2);
}

void ROIExtractor::resetPatchMixerWithPlan(const std::vector<InferenceInfo>& inferencePlan) {
  mFreeRectsVec.clear();
  IntPairs WHs;
  for (const auto& info: inferencePlan) {
    WHs.emplace_back(info.size, info.size);
    mFreeRectsVec.push_back({IntRect(0, 0, info.size, info.size)});
  }
}

} // namespace md
