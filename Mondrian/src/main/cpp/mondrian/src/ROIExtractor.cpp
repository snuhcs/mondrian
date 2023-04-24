#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/MixedFrame.hpp"

namespace md {

const cv::TermCriteria ROIExtractor::CRITERIA = cv::TermCriteria(
    cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);

ROIExtractor::ROIExtractor(const ROIExtractorConfig& config, int maxMergeSize, bool run,
                           ROIResizer* roiResizer, bool emulatedBatch, int roiSize,
                           std::vector<InferenceInfo> inferencePlan, std::set<int> vids)
<<<<<<< Updated upstream
    : config_(config), maxMergeSize_(emulatedBatch ? roiSize : maxMergeSize), ROIResizer_(roiResizer),
      emulatedBatch_(emulatedBatch), ROISize_(roiSize),
      targetSize_(cv::Size(int(config.EXTRACTION_RESIZE_WIDTH),
                           int(config.EXTRACTION_RESIZE_HEIGHT))),
      inferencePlan_(std::move(inferencePlan)),
      fullFrameInferenceCount_(0), fullFrameTarget_(nullptr), fullFrameVid_(-1),
      vids_(std::move(vids)), stop_(false), notFullyPacked_(true) {
=======
    : mConfig(config), mMaxMergeSize(emulatedBatch ? roiSize : maxMergeSize),
      mBorder(config.ROI_BORDER), mROIResizer(roiResizer), mEmulatedBatch(emulatedBatch),
      mROISize(roiSize), mTargetSize(cv::Size(int(config.EXTRACTION_RESIZE_WIDTH),
                                              int(config.EXTRACTION_RESIZE_HEIGHT))),
      mInferencePlan(std::move(inferencePlan)),
      mFullFrameInferenceCount(0), mFullFrameTarget(nullptr), mFullFrameVid(-1),
      mVids(std::move(vids)), mbStop(false), notFullyPacked(true) {
>>>>>>> Stashed changes
  if (run) {
    resetPatchMixerWithPlan(inferencePlan_);
    threads_.reserve(config.NUM_WORKERS);
    for (int extractorId = 0; extractorId < config.NUM_WORKERS; extractorId++) {
      threads_.emplace_back([this, extractorId]() { work(extractorId); });
    }
  }
}

ROIExtractor::~ROIExtractor() {
  stop_ = true;
  queueCV_.notify_all();
  for (auto& thread : threads_) {
    thread.join();
  }
}

void ROIExtractor::enqueue(Frame* frame) {
  std::unique_lock<std::mutex> queueLock(queueMtx_);
  queueCV_.wait(queueLock, [this]() { return PDWaiting_.size() < config_.MAX_QUEUE_SIZE; });
  PDWaiting_.insert(frame);
  LOGD("%-25s                 for video %-5d frame %-4d // %4lu PD | %4lu OF | %4d Processed",
       "ROIExtractor::enqueue", frame->vid, frame->frameIndex,
       PDWaiting_.size(), OFWaiting_.size(),
       std::accumulate(packedFrames_.begin(), packedFrames_.end(), 0,
                       [](int cnt, const auto& it) { return cnt + it.second.size(); }));
  queueLock.unlock();
  queueCV_.notify_all();
}

void ROIExtractor::notify() {
  queueCV_.notify_all();
}

std::tuple<std::vector<MixedFrame>, Frame*, MultiStream, Stream> ROIExtractor::prepareInference(
    std::vector<InferenceInfo>& nextInferencePlan, bool runFull, int scheduleID) {
  // Should be packLock => queueLock order.
  // See postprocessOF() for detail
  std::unique_lock<std::mutex> packLock(packMtx_);
  std::unique_lock<std::mutex> queueLock(queueMtx_);

  if (notFullyPacked_) {
    applyLasts();
  }

  Frame* fullFrameTarget = fullFrameTarget_;

  std::map<int, std::set<ROI*>> groupedROIs;
  for (const auto&[vid, frames]: packedFrames_) {
    for (Frame* frame: frames) {
      assert(frame != fullFrameTarget);
      for (auto& pROI: frame->mergedROIs) {
        groupedROIs[pROI->getPackedMixedFrameIndex()].insert(pROI.get());
      }
      for (auto& probeROI: frame->probingROIs) {
        groupedROIs[probeROI->getPackedMixedFrameIndex()].insert(probeROI.get());
      }
    }
  }

  std::vector<MixedFrame> mixedFrames;
  for (auto&[mixedFrameIndex, rois]: groupedROIs) {
    assert(mixedFrameIndex < inferencePlan_.size());
    const auto& info = inferencePlan_[mixedFrameIndex];
    if (!rois.empty()) {
      mixedFrames.emplace_back(info.device, rois, info.size);
    }
  }

  MultiStream selectedFrames = std::move(packedFrames_);
  packedFrames_.clear();
  notFullyPacked_ = true;
  candidateLastFrames_.clear();

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

  for (Frame* frame: OFProcessing_) {
    frame->extractOFAgain = true;
  }

  if (runFull) {
    int index = fullFrameInferenceCount_ % int(vids_.size());
    fullFrameInferenceCount_++;
    fullFrameVid_ = *std::next(vids_.begin(), index);
  } else {
    fullFrameVid_ = -1;
  }
  fullFrameTarget_ = nullptr;
  inferencePlan_ = nextInferencePlan;
  resetPatchMixerWithPlan(inferencePlan_);

  queueLock.unlock();
  packLock.unlock();
  queueCV_.notify_all();

  return {mixedFrames, fullFrameTarget, selectedFrames, droppedFrames};
}

void ROIExtractor::work(int extractorId) {
  /*
   *    Frame Status           Containing data structure
   * 1. Before PD extraction | PDWaiting_
   * 2. Extracting PD        | -
   * 3. Before OF extraction | OFWaiting_
   * 4. Extracting OF        | OFProcessing_
   * 5. OF extraction ended  | packedFrames_
   */

  auto getPDJob = [this]() {
    if (!PDWaiting_.empty() && OFWaiting_.size() < config_.MAX_QUEUE_SIZE) {
      return *PDWaiting_.begin();
    } else {
      return (Frame*) nullptr;
    }
  };
  auto getOFJob = [this]() {
    if (!OFWaiting_.empty()
        && notFullyPacked_
        && (*OFWaiting_.begin())->readyForOFExtraction()) {
      return *OFWaiting_.begin();
    } else {
      return (Frame*) nullptr;
    }
  };

  while (true) {
    bool isOF = false;
    Frame* frame = nullptr;

    std::unique_lock<std::mutex> queueLock(queueMtx_);
    queueCV_.wait(queueLock, [this, &isOF, &frame, &getPDJob, &getOFJob]() {
      if (stop_) {
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

    if (stop_) {
      queueLock.unlock();
      queueCV_.notify_all();
      return;
    }

    if (isOF) {
      frame->OFExtractorID = extractorId;
      OFWaiting_.erase(OFWaiting_.begin());
      OFProcessing_.insert(frame);
    } else {
      frame->PDExtractorID = extractorId;
      PDWaiting_.erase(PDWaiting_.begin());
    }
    queueLock.unlock();
    queueCV_.notify_all();

    if (isOF) {
      processOF(frame);
    } else {
      processPD(frame);
    }

    if (isOF) {
      postprocessOF(frame);
    } else {
      queueLock.lock();
      OFWaiting_.insert(frame);
      queueLock.unlock();
    }
    queueCV_.notify_all();
  }
}

void ROIExtractor::postprocessOF(Frame* currFrame) {
  currFrame->filterPDROIs(config_.PD_FILTER_THRESHOLD, config_.EAT_PD);
  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeROIs(ROIResizer_, emulatedBatch_, ROISize_);
  currFrame->resizeEndTime = NowMicros();
  currFrame->mergeROIStartTime = NowMicros();
<<<<<<< Updated upstream
  if (config_.MERGE) {
    currFrame->mergeROIs(float(maxMergeSize_));
=======
  if (mConfig.MERGE) {
    currFrame->mergedROIs = MergedROI::mergeROIs(currFrame->rois, mMaxMergeSize, mBorder);
>>>>>>> Stashed changes
  }
  currFrame->mergeROIEndTime = NowMicros();

  currFrame->boxesIfLast = getBoxesIfLast(currFrame);
  currFrame->boxesIfScaled = getBoxesIfScaled(currFrame);

  if (emulatedBatch_) {
    assert(std::all_of(currFrame->boxesIfLast.begin(), currFrame->boxesIfLast.end(),
                       [this](const auto& box) { return std::max(box.first, box.second) <= ROISize_;}));
    assert(std::all_of(currFrame->boxesIfScaled.begin(), currFrame->boxesIfScaled.end(),
                       [this](const auto& box) { return std::max(box.first, box.second) <= ROISize_;}));
  }

  currFrame->mixingStartTime = NowMicros();
  std::unique_lock<std::mutex> packLock(packMtx_);
  tryPack(currFrame);
  packLock.unlock();
  currFrame->mixingEndTime = NowMicros();

  std::lock_guard<std::mutex> queueLock(queueMtx_);
  OFProcessing_.erase(currFrame);
  if (currFrame->extractOFAgain) {
    currFrame->resetOFROIExtraction();
    OFWaiting_.insert(currFrame);
  } else {
    currFrame->isROIsReady = true;
  }
}

void ROIExtractor::tryPack(Frame* frame) {
  // TODO: Cache pack indices
  assert(frame->vid != -1);
  if (!notFullyPacked_) {
    return;
  }

  if (frame->vid == fullFrameVid_) {
    notFullyPacked_ = tryPackFullVid(frame);
  } else {
    notFullyPacked_ = tryPackNonFullVid(frame);
  }

  if (!notFullyPacked_) {
    applyLasts();
    std::lock_guard<std::mutex> queueLock(queueMtx_);
    for (Frame* f: OFProcessing_) {
      f->extractOFAgain = true;
    }
  }
}

static auto appendLastBoxes = [](IntPairs& lastBoxes, Frame* frameToAppend) {
  lastBoxes.insert(lastBoxes.end(),
                   frameToAppend->boxesIfLast.begin(), frameToAppend->boxesIfLast.end());
};

bool ROIExtractor::tryPackFullVid(Frame* frame) {
  assert(candidateLastFrames_.find(frame->vid) == candidateLastFrames_.end());

  // If fullFrameTarget_ is not set
  if (fullFrameTarget_ == nullptr) {
    fullFrameTarget_ = frame;
    return true;
  }

  // Try pack incoming frame as scaled
  auto copiedFreeRectsVec = freeRectsVec_;
  auto[fullPackIndices, fullPackLocations] = PatchMixer::pack(
      copiedFreeRectsVec, fullFrameTarget_->boxesIfScaled, /*backward=*/true,
      emulatedBatch_, ROISize_);

  // If single scaled packing fails (rare case)
  if (fullPackIndices.size() != fullFrameTarget_->boxesIfScaled.size()) {
    assert(fullPackIndices.size() < fullFrameTarget_->boxesIfScaled.size());
    return false;
  }

  // Apply incoming frame and try pack last frame candidates
  PatchMixer::apply(copiedFreeRectsVec, fullFrameTarget_->boxesIfScaled, fullPackIndices,
                    emulatedBatch_, ROISize_);
  IntPairs lastBoxes;
  for (auto&[cVid, info]: candidateLastFrames_) {
    appendLastBoxes(lastBoxes, info.frame);
  }
  auto[lastPackIndices, lastPackLocations] = PatchMixer::pack(
      copiedFreeRectsVec, lastBoxes, /*backward=*/false, emulatedBatch_, ROISize_);

  // If last frame candidates packing failed
  if (lastPackIndices.size() != lastBoxes.size()) {
    assert(lastPackIndices.size() < lastBoxes.size());
    return false;
  }

  // If last frame candidates packing success
  int i = 0;
  for (auto&[_, info]: candidateLastFrames_) {
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
  prepareScaledFrame(fullFrameTarget_, fullPackIndices, fullPackLocations);
  packedFrames_[fullFrameTarget_->vid].insert(fullFrameTarget_);
  freeRectsVec_ = std::move(copiedFreeRectsVec);
  fullFrameTarget_ = frame;
  return true;
}

bool ROIExtractor::tryPackNonFullVid(Frame* frame) {
  const int vid = frame->vid;
  bool vidExists = candidateLastFrames_.find(frame->vid) != candidateLastFrames_.end();
  auto copiedFreeRectsVec = freeRectsVec_;

  // Try pack last candidate frames.
  // If candidateVid == frame->vid, pack candidateVid first as scaled
  std::pair<Indices, Locations> existPackIndicesLocations;
  if (vidExists) {
    existPackIndicesLocations = PatchMixer::pack(
        copiedFreeRectsVec, candidateLastFrames_[vid].frame->boxesIfScaled, /*backward=*/true,
        emulatedBatch_, ROISize_);
    PatchMixer::apply(copiedFreeRectsVec,
                      candidateLastFrames_[vid].frame->boxesIfScaled,
                      existPackIndicesLocations.first, emulatedBatch_, ROISize_);
  } else {
    // Temporarily add. Erase if packing fails
    candidateLastFrames_[vid] = LastPackInfo();
  }

  IntPairs lastBoxes;
  for (auto&[cVid, info]: candidateLastFrames_) {
    if (cVid == vid) {
      appendLastBoxes(lastBoxes, frame);
    } else {
      appendLastBoxes(lastBoxes, info.frame);
    }
  }
  auto[lastPackIndices, lastPackLocations] = PatchMixer::pack(
      copiedFreeRectsVec, lastBoxes, /*backward=*/false,
      emulatedBatch_, ROISize_);

  if (lastPackIndices.size() != lastBoxes.size()) {
    assert(lastPackIndices.size() < lastBoxes.size());
    if (!vidExists) {
      candidateLastFrames_.erase(vid);
    }
    return false;
  }

  // If last frame candidates packing success
  if (vidExists) {
    auto& info = candidateLastFrames_[vid];
    prepareScaledFrame(info.frame,
                       existPackIndicesLocations.first,
                       existPackIndicesLocations.second);
  }
  candidateLastFrames_[vid].frame = frame;
  int i = 0;
  for (auto&[_, info]: candidateLastFrames_) {
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
  packedFrames_[vid].insert(frame);
  freeRectsVec_ = std::move(copiedFreeRectsVec);
  return true;
}

void ROIExtractor::applyLasts() {
  for (auto&[pVid, info]: candidateLastFrames_) {
    assert(info.indices.size() == info.locations.size());
    PatchMixer::apply(freeRectsVec_, info.frame->boxesIfLast, info.indices,
                      emulatedBatch_, ROISize_);
  }
  for (auto&[cVid, info]: candidateLastFrames_) {
    prepareFrameLast(info.frame, info.indices, info.locations);
  }
}

IntPairs ROIExtractor::getBoxesIfLast(const Frame* frame) {
  // TODO: Synchronize simulation with add logics
  IntPairs boxesIfLast;
  for (const auto& pROI: frame->mergedROIs) {
    // TODO: Make below two condition as single value(or function) of condition
    if (!emulatedBatch_ && config_.NO_DOWNSAMPLING_FOR_LAST_FRAME) {
      boxesIfLast.push_back(pROI->getBorderMatWidthHeight(1.0f));
    } else {
      boxesIfLast.push_back(pROI->getBorderMatWidthHeight());
    }
  }
  for (const auto& cROI: frame->rois) {
    if (cROI->getScaleLevel() == ROIResizer::INVALID_LEVEL) {
      continue;
    }
    ROIResizer_->getProbingCandidates(cROI.get());
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
<<<<<<< Updated upstream
  for (const auto& pROI: frame->parentROIs) {
    if (!emulatedBatch_ && config_.NO_DOWNSAMPLING_FOR_LAST_FRAME) {
=======
  for (const auto& pROI: frame->mergedROIs) {
    if (!mEmulatedBatch && mConfig.NO_DOWNSAMPLING_FOR_LAST_FRAME) {
>>>>>>> Stashed changes
      pROI->setTargetScale(1.0f, ROIResizer::INVALID_LEVEL);
    }
    pROI->setPackInfo(locations[i], indices[i].first, emulatedBatch_, ROISize_);
    i++;
  }
  for (const auto& cROI: frame->rois) {
    if (cROI->getScaleLevel() == ROIResizer::INVALID_LEVEL) {
      i += int(cROI->probeScales.size());
      continue;
    }
    for (auto scale: cROI->probeScales) {
      assert(0.0f < scale && scale <= 1.0f);
      std::unique_ptr<ROI> probeROI = std::make_unique<ROI>(
          nullptr, cROI->id, cROI->frame, cROI->paddedLoc, cROI->type, cROI->origin, cROI->label,
          cROI->features.ofFeatures, ROI::INVALID_CONF, 0, config_.ROI_BORDER, true);
      probeROI->setTargetScale(scale, cROI->getScaleLevel());
      probeROI->setPackInfo(locations[i], indices[i].first, emulatedBatch_, ROISize_);
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
  for (const auto& pROI: frame->mergedROIs) {
    BoxesIfIntermediate.push_back(pROI->getBorderMatWidthHeight());
  }
  return BoxesIfIntermediate;
}

void ROIExtractor::prepareScaledFrame(Frame* frame,
                                      const Indices& indices, const Locations& locations) {
  assert(indices.size() == locations.size());
  frame->resetProbeROIs();
  int i = 0;
  for (const auto& pROI: frame->mergedROIs) {
    auto[bw, bh] = pROI->getBorderMatWidthHeight();
    pROI->setPackInfo(locations[i], indices[i].first, emulatedBatch_, ROISize_);
    i++;
  }
  assert(i == locations.size());
}

void ROIExtractor::processPD(Frame* currFrame) {
  currFrame->pixelDiffROIProcessStartTime = NowMicros();
<<<<<<< Updated upstream
  getPixelDiffROIs(currFrame, targetSize_,
                   config_.MAX_PD_ROI_SIZE, config_.MIN_PD_ROI_SIZE,
                   currFrame->childROIs);
=======
  getPixelDiffROIs(currFrame, mTargetSize,
                   mConfig.MAX_PD_ROI_SIZE, mConfig.MIN_PD_ROI_SIZE,
                   currFrame->rois);
>>>>>>> Stashed changes
  currFrame->pixelDiffROIProcessEndTime = NowMicros();
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu PD ROIs",
       "ROIExtractor::processPD",
       currFrame->pixelDiffROIProcessEndTime - currFrame->pixelDiffROIProcessStartTime,
       currFrame->vid, currFrame->frameIndex, currFrame->rois.size());
}

void ROIExtractor::processOF(Frame* currFrame) {
  assert(!currFrame->extractOFAgain);
  Frame* prevFrame = currFrame->prevFrame;
  std::vector<BoundingBox> reliablePrevBoxes;
  if (prevFrame->useInferenceResultForOF) {
    for (const std::unique_ptr<BoundingBox>& box : prevFrame->boxes) {
      if (box->confidence > config_.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD) {
        BoundingBox reliableBox(box->id, Rect(
            std::max(0.0f, box->location.l),
            std::max(0.0f, box->location.t),
            std::min(float(currFrame->width), box->location.r),
            std::min(float(currFrame->height), box->location.b)),
                                box->confidence, box->label, O_PACKED_BBOX);
        reliableBox.srcROI = box->srcROI;
        reliablePrevBoxes.push_back(reliableBox);
      }
    }
  } else {
    for (auto& cROI : currFrame->prevFrame->rois) {
      if (cROI->type == PD) {
        continue;
      }
      BoundingBox reliableBox(cROI->id, cROI->origLoc, 1, cROI->label, cROI->origin);
      reliableBox.srcROI = cROI.get();
      reliablePrevBoxes.push_back(reliableBox);
    }
  }
  currFrame->opticalFlowROIProcessStartTime = NowMicros();
  getOpticalFlowROIs(prevFrame, currFrame, reliablePrevBoxes, targetSize_, currFrame->childROIs);
  currFrame->opticalFlowROIProcessEndTime = NowMicros();
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu OF ROIs", "ROIExtractor::processOF",
       currFrame->opticalFlowROIProcessEndTime - currFrame->opticalFlowROIProcessStartTime,
       currFrame->vid, currFrame->frameIndex,
       std::count_if(currFrame->rois.begin(), currFrame->rois.end(),
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
<<<<<<< Updated upstream
      float newL = std::max(0.0f, loc.l + x);
      float newT = std::max(0.0f, loc.t + y);
      float newR = std::min(float(width), loc.r + x);
      float newB = std::min(float(height), loc.b + y);
      if (newR - newL >= 1.0f && newB - newT >= 1.0f) {
=======
      float newLeft = std::max(0.0f, loc.l + x);
      float newTop = std::max(0.0f, loc.t + y);
      float newRight = std::min(float(width), loc.r + x);
      float newBottom = std::min(float(height), loc.b + y);
      if (newRight - newLeft >= 1.0f && newBottom - newTop >= 1.0f) {
>>>>>>> Stashed changes
        outChildROIs.push_back(std::make_unique<ROI>(
            box.srcROI, box.id, currFrame, Rect(newL, newT, newR, newB),
            OF, box.origin, box.label, of, box.confidence,
            config_.ROI_PADDING, config_.ROI_BORDER, false));
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
    float x = std::min(bbx.l, bbx.r) * xRatio;
    float y = std::min(bbx.t, bbx.b) * yRatio;
    float w = std::abs(bbx.r - bbx.l) * xRatio;
    float h = std::abs(bbx.b - bbx.t) * yRatio;
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
<<<<<<< Updated upstream
          ((float) bbx.l + (float) bbx.w / 2) * xRatio,
          ((float) bbx.t + (float) bbx.h / 2) * yRatio));
=======
          ((float) bbx.l + (float) bbx.width() / 2) * xRatio,
          ((float) bbx.t + (float) bbx.height() / 2) * yRatio));
>>>>>>> Stashed changes
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
  for (int i = 0; i < config_.PD_INTERVAL; ++i) {
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
    if (std::min(box.w, box.h) >= 1.0f) {
      outChildROIs.push_back(std::make_unique<ROI>(
          nullptr, UNASSIGNED_ID, currFrame, box,
          PD, O_PD, -1, OFFeatures({}, {}, {}), ROI::INVALID_CONF,
          config_.ROI_PADDING, config_.ROI_BORDER, false));
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
  freeRectsVec_.clear();
  IntPairs WHs;
  for (const auto& info: inferencePlan) {
    WHs.emplace_back(info.size, info.size);
    freeRectsVec_.push_back({IntRect(0, 0, info.size, info.size)});
  }
}

} // namespace md
