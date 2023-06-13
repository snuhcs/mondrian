#include "mondrian/ROIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "mondrian/Log.hpp"
#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIPrioritizer.hpp"

namespace md {

const cv::TermCriteria ROIExtractor::CRITERIA = cv::TermCriteria(
    cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);

ROIExtractor::ROIExtractor(const ROIExtractorConfig& config, int maxMergeSize,
                           ROIResizer* roiResizer, ExecutionType executionType, int roiSize,
                           std::vector<InferenceInfo> inferencePlan, int numVideos)
    : config_(config), maxMergeSize_(maxMergeSize),
      ROIResizer_(roiResizer), executionType_(executionType), ROISize_(roiSize),
      targetSize_(cv::Size(int(config.EXTRACTION_RESIZE_WIDTH),
                           int(config.EXTRACTION_RESIZE_HEIGHT))),
      inferencePlan_(std::move(inferencePlan)),
      fullFrameInferenceCount_(0), fullFrameTarget_(nullptr), fullFrameVid_(-1),
      numVideos_(numVideos), stop_(false), pull_(false), fullyPacked_(false) {
  assert(executionType_ == MONDRIAN || ROISize_ == maxMergeSize_);
  resetPatchMixerWithPlan(inferencePlan_);
  threads_.reserve(config.NUM_WORKERS);
  for (int extractorId = 0; extractorId < config.NUM_WORKERS; extractorId++) {
    threads_.emplace_back([this, extractorId]() { work(extractorId); });
  }
}

ROIExtractor::~ROIExtractor() {
  stop_ = true;
  queueCV_.notify_all();
  for (auto& thread: threads_) {
    thread.join();
  }
}

void ROIExtractor::enqueue(Frame* frame) {
  std::unique_lock<std::mutex> queueLock(queueMtx_);
  if (!config_.STREAM_MODE) {
    queueCV_.wait(queueLock, [this]() { return PDWaiting_.size() < config_.MAX_QUEUE_SIZE; });
  }
  LOGD("XXX == %lu OF Waiting %lu OF Processing %d Processed | %d",
       OFWaiting_.size(), OFProcessing_.size(),
       std::accumulate(packedFrames_.begin(), packedFrames_.end(), 0,
                       [](int sum, const auto& pair) { return sum + pair.second.size(); }),
       OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
  PDWaiting_.insert(frame);
  queueLock.unlock();
  queueCV_.notify_all();
}

void ROIExtractor::notify() {
  queueCV_.notify_all();
}

PackingResult ROIExtractor::prepareInference(std::vector<InferenceInfo>& nextInferencePlan,
                                             bool runFull, int scheduleID) {
  // Should be packLock => queueLock sort.
  // See postprocessOF() for detail
  std::unique_lock<std::mutex> packLock(packMtx_);
  std::unique_lock<std::mutex> queueLock(queueMtx_);

  pull_ = true;

  if (config_.STREAM_MODE) {
    time_us start = NowMicros();
    queueCV_.wait(queueLock, [this]() { return OFProcessing_.empty(); });
    LOGD("XXX == prepareInference %lu OF Waiting %lu OF Processing %d Processed | %d",
         OFWaiting_.size(), OFProcessing_.size(),
         std::accumulate(packedFrames_.begin(), packedFrames_.end(), 0,
                         [](int sum, const auto& pair) { return sum + pair.second.size(); }),
         OFWaiting_.empty() ? -1 : (*OFWaiting_.begin())->frameIndex);
    packGatheredMultiStream(packedFrames_);
    time_us end = NowMicros();
  } else { // Back to back mode
    if (!fullyPacked_) {
      applyLasts();
    }
  }

  Frame* fullFrameTarget = fullFrameTarget_;

  std::map<int, std::set<MergedROI*>> groupedMergedROIs;
  for (const auto&[vid, frames]: packedFrames_) {
    for (Frame* frame: frames) {
      assert(frame != fullFrameTarget);
      for (auto& mergedROI: frame->mergedROIs) {
        if (mergedROI->isPacked()) {
          groupedMergedROIs[mergedROI->relativePackedCanvasIndex()].insert(mergedROI.get());
        }
      }
      for (auto& probeROI: frame->probingROIs) {
        if (probeROI->isPacked()) {
          groupedMergedROIs[probeROI->relativePackedCanvasIndex()].insert(probeROI.get());
        }
      }
    }
  }

  std::vector<PackedCanvas> packedCanvases;
  for (auto&[relativePackedCanvasIndex, mergedROIs]: groupedMergedROIs) {
    assert(relativePackedCanvasIndex < inferencePlan_.size());
    const auto& info = inferencePlan_[relativePackedCanvasIndex];
    if (!mergedROIs.empty()) {
      packedCanvases.emplace_back(mergedROIs, info.size, info.device);
    }
  }

  MultiStream selectedFrames = std::move(packedFrames_);
  packedFrames_.clear();
  fullyPacked_ = false;
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
    fullFrameVid_ = fullFrameInferenceCount_ % numVideos_;
    fullFrameInferenceCount_++;
  } else {
    fullFrameVid_ = -1;
  }
  fullFrameTarget_ = nullptr;
  inferencePlan_ = nextInferencePlan;
  resetPatchMixerWithPlan(inferencePlan_);

  queueLock.unlock();
  packLock.unlock();
  queueCV_.notify_all();

  pull_ = false;

  return {packedCanvases, fullFrameTarget, selectedFrames, droppedFrames};
}

void ROIExtractor::packGatheredMultiStream(const MultiStream& multiStream) {
  time_us startTime = NowMicros();
  // Full frame
  if (multiStream.find(fullFrameVid_) != multiStream.end()) {
    fullFrameTarget_ = *multiStream.at(fullFrameVid_).rbegin();
    packedFrames_[fullFrameVid_].erase(fullFrameTarget_);
    LOGD("XXX == Last Full Frame %d", fullFrameTarget_->frameIndex);
  }

  // Last frames
  for (const auto&[vid, frames]: multiStream) {
    if (vid == fullFrameVid_) {
      continue;
    }
    Frame* lastFrame = *frames.rbegin();
    auto[indices, locations] = ROIPacker::pack(freeRectsVec_, lastFrame->boxesIfLast,
                                               /*backward=*/false, executionType_, ROISize_);
    bool fullyPacked = indices.size() == lastFrame->boxesIfLast.size();
    int maxPackedCanvasIndex = -1;
    for (auto& [packedCanvasIndex, freeRectIndex]: indices) {
      maxPackedCanvasIndex = std::max(maxPackedCanvasIndex, packedCanvasIndex);
    }
    LOGD("XXX == Last Pack Frame %d: %lu / %lu Packed, Last Packed Frame=%d",
         lastFrame->frameIndex, indices.size(), lastFrame->boxesIfLast.size(), maxPackedCanvasIndex);
    if (fullyPacked) {
      ROIPacker::apply(freeRectsVec_, lastFrame->boxesIfLast, indices, executionType_, ROISize_);
    } else {
      indices.resize(lastFrame->boxesIfLast.size());
      locations.resize(lastFrame->boxesIfLast.size());
      ROIPacker::apply(freeRectsVec_, lastFrame->boxesIfLast, indices, executionType_, ROISize_);
    }
    prepareFrameLast(lastFrame, indices, locations);
  }
  time_us packLastTime = NowMicros();

  // Gather MergedROIs not in last frames
  std::vector<MergedROI*> mergedROIs;
  for (const auto&[vid, frames]: multiStream) {
    for (Frame* frame: frames) {
      if (frame == *frames.rbegin()) {
        continue;
      }
      for (auto& mergedROI: frame->mergedROIs) {
        mergedROIs.push_back(mergedROI.get());
      }
    }
  }
  time_us gatherTime = NowMicros();

  // Sort MergedROIs
  auto sortedMergedROIs = ROIPrioritizer::sort(mergedROIs);
  time_us sortTime = NowMicros();

  // Pack MergedROIs
  for (MergedROI* mergedROI: sortedMergedROIs) {
    auto[bw, bh] = mergedROI->borderedMatWH();
    auto[indices, locations] = ROIPacker::pack(freeRectsVec_, {{bw, bh}}, /*backward=*/false,
                                               executionType_, ROISize_);
    if (!indices.empty()) {
      ROIPacker::apply(freeRectsVec_, {{bw, bh}}, indices, executionType_, ROISize_);
      mergedROI->setPackInfo(locations[0], indices[0].first, executionType_, ROISize_);
    }
  }
  time_us packOthersTime = NowMicros();

  LOGD("XXX == # ROIs: %lu, # Frames: %d | "
       "total: %lld, packLastTime: %lld, gatherTime: %lld, sortTime: %lld, packOthersTime: %lld",
       sortedMergedROIs.size(),
       std::accumulate(packedFrames_.begin(), packedFrames_.end(), 0,
                       [](int sum, const auto& pair) {
         return sum + pair.second.size();
       }) + (multiStream.find(fullFrameVid_) != multiStream.end() ? 1 : 0),
       packOthersTime - startTime,
       packLastTime - startTime,
       gatherTime - packLastTime,
       sortTime - gatherTime,
       packOthersTime - sortTime);
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
    bool ofFrameExists = !OFWaiting_.empty();
    bool notFullyPacked = !fullyPacked_;
    bool readyForOFExtraction = ofFrameExists && (*OFWaiting_.begin())->readyForOFExtraction();
    if (!pull_ && ofFrameExists && notFullyPacked && readyForOFExtraction) {
      return *OFWaiting_.begin();
    } else {
//      LOGD("XXX %s %s %s",
//           ofFrameExists
//           ? "ofFrameExists"
//           : "no of frames ",
//           notFullyPacked
//           ? "notFullyPacked"
//           : "fullyPacked   ",
//           readyForOFExtraction
//           ? "readyForOFExtraction    "
//           : "not readyForOFExtraction");
//      if (ofFrameExists && !readyForOFExtraction) {
//        Frame* frame = *OFWaiting_.begin();
//        LOGD("XXX Frame %d, %s %s",
//             frame->frameIndex,
//             frame->useInferenceResultForOF
//             ? "useInferenceResultForOF"
//             : "useOFResult            ",
//             frame->readyForOFExtraction()
//             ? "readyForOFExtraction    "
//             : "not readyForOFExtraction");
//      }
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
    time_us start = NowMicros();

    if (stop_) {
      queueLock.unlock();
      queueCV_.notify_all();
      return;
    }

    if (isOF) {
      frame->OFExtractorID = extractorId;
      OFWaiting_.erase(frame);
      OFProcessing_.insert(frame);
    } else {
      frame->PDExtractorID = extractorId;
      PDWaiting_.erase(frame);
      PDProcessing_.insert(frame);
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
      PDProcessing_.erase(frame);
      OFWaiting_.insert(frame);
      queueLock.unlock();
    }
    time_us end = NowMicros();
    if (isOF) {
      LOGD("XXX total: %lld | resize: %lld | merge: %lld",
           end - start,
           frame->resizeEndTime - frame->resizeStartTime,
           frame->mergeROIEndTime - frame->mergeROIStartTime);
    }
    queueCV_.notify_all();
  }
}

void ROIExtractor::postprocessOF(Frame* currFrame) {
  currFrame->filterPDROIs(config_.PD_FILTER_THRESHOLD, config_.EAT_PD);
  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeROIs(ROIResizer_, executionType_, ROISize_);
  currFrame->resizeEndTime = NowMicros();
  currFrame->mergeROIStartTime = NowMicros();
  currFrame->mergedROIs = MergedROI::mergeROIs(currFrame->rois, maxMergeSize_, config_.MERGE);
  currFrame->mergeROIEndTime = NowMicros();

  currFrame->boxesIfLast = getBoxesIfLast(currFrame);
  currFrame->boxesIfScaled = getBoxesIfScaled(currFrame);

  if (executionType_ == EMULATED_BATCH) {
    assert(std::all_of(currFrame->boxesIfLast.begin(), currFrame->boxesIfLast.end(),
                       [this](const auto& box) { return std::max(box.first, box.second) <= ROISize_;}));
    assert(std::all_of(currFrame->boxesIfScaled.begin(), currFrame->boxesIfScaled.end(),
                       [this](const auto& box) { return std::max(box.first, box.second) <= ROISize_;}));
  }

  if (config_.STREAM_MODE) {
    packedFrames_[currFrame->vid].insert(currFrame);
    OFProcessing_.erase(currFrame);
    currFrame->isROIsReady = true;
  } else {
    currFrame->packingStartTime = NowMicros();
    std::unique_lock<std::mutex> packLock(packMtx_);
    tryPack(currFrame);
    packLock.unlock();
    currFrame->packingEndTime = NowMicros();

    std::lock_guard<std::mutex> queueLock(queueMtx_);
    OFProcessing_.erase(currFrame);
    if (currFrame->extractOFAgain) {
      currFrame->resetOFROIExtraction();
      OFWaiting_.insert(currFrame);
    } else {
      currFrame->isROIsReady = true;
    }
  }
}

void ROIExtractor::tryPack(Frame* frame) {
  // TODO: Cache pack indices
  assert(frame->vid != -1);
  if (fullyPacked_) {
    return;
  }

  if (frame->vid == fullFrameVid_) {
    bool packingSuccess = tryPackFullVid(frame);
    fullyPacked_ = !packingSuccess;
  } else {
    bool packingSuccess = tryPackNonFullVid(frame);
    fullyPacked_ = !packingSuccess;
  }

  if (fullyPacked_) {
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
  auto[fullPackIndices, fullPackLocations] = ROIPacker::pack(
      copiedFreeRectsVec, fullFrameTarget_->boxesIfScaled, /*backward=*/true,
      executionType_, ROISize_);

  // If single scaled packing fails (rare case)
  if (fullPackIndices.size() != fullFrameTarget_->boxesIfScaled.size()) {
    assert(fullPackIndices.size() < fullFrameTarget_->boxesIfScaled.size());
    return false;
  }

  // Apply incoming frame and try pack last frame candidates
  ROIPacker::apply(copiedFreeRectsVec, fullFrameTarget_->boxesIfScaled, fullPackIndices,
                   executionType_, ROISize_);
  IntPairs lastBoxes;
  for (auto&[cVid, info]: candidateLastFrames_) {
    appendLastBoxes(lastBoxes, info.frame);
  }
  auto[lastPackIndices, lastPackLocations] = ROIPacker::pack(
      copiedFreeRectsVec, lastBoxes, /*backward=*/false, executionType_, ROISize_);

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
  prepareFrameScaled(fullFrameTarget_, fullPackIndices, fullPackLocations);
  if (!config_.STREAM_MODE) {
    packedFrames_[fullFrameTarget_->vid].insert(fullFrameTarget_);
  }
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
  std::pair<IntPairs, IntPairs> existPackIndicesLocations;
  if (vidExists) {
    existPackIndicesLocations = ROIPacker::pack(
        copiedFreeRectsVec, candidateLastFrames_[vid].frame->boxesIfScaled, /*backward=*/true,
        executionType_, ROISize_);
    ROIPacker::apply(copiedFreeRectsVec,
                     candidateLastFrames_[vid].frame->boxesIfScaled,
                     existPackIndicesLocations.first, executionType_, ROISize_);
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
  auto[lastPackIndices, lastPackLocations] = ROIPacker::pack(
      copiedFreeRectsVec, lastBoxes, /*backward=*/false,
      executionType_, ROISize_);

  if (lastPackIndices.size() != lastBoxes.size()) {
    if (!vidExists) {
      candidateLastFrames_.erase(vid);
    }
    return false;
  }

  // If last frame candidates packing success
  if (vidExists) {
    auto& info = candidateLastFrames_[vid];
    prepareFrameScaled(info.frame,
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
  if (!config_.STREAM_MODE) {
    packedFrames_[vid].insert(frame);
  }
  freeRectsVec_ = std::move(copiedFreeRectsVec);
  return true;
}

void ROIExtractor::applyLasts() {
  for (auto&[pVid, info]: candidateLastFrames_) {
    assert(info.indices.size() == info.locations.size());
    ROIPacker::apply(freeRectsVec_, info.frame->boxesIfLast, info.indices,
                     executionType_, ROISize_);
  }
  for (auto&[cVid, info]: candidateLastFrames_) {
    prepareFrameLast(info.frame, info.indices, info.locations);
  }
}

IntPairs ROIExtractor::getBoxesIfLast(const Frame* frame) {
  // TODO: Synchronize simulation with add logics
  IntPairs boxesIfLast;
  for (const auto& mergedROI: frame->mergedROIs) {
    // TODO: Make below two condition as single value(or function) of condition
    bool noScaleForLast = executionType_ != MONDRIAN || config_.NO_DOWNSAMPLING_FOR_LAST_FRAME;
    float scale = noScaleForLast ? 1.0f : mergedROI->targetScale();
    auto[bw, bh] = mergedROI->borderedMatWH(scale);
    boxesIfLast.emplace_back(bw, bh);
  }
  for (const auto& roi: frame->rois) {
    if (roi->scaleLevel() == ROIResizer::INVALID_LEVEL) {
      roi->probeScales.clear();
      continue;
    }
    ROIResizer_->getProbingCandidates(roi.get());
    for (auto scale: roi->probeScales) {
      int bw = MergedROI::borderedLengthOf(roi->paddedLoc.w, scale);
      int bh = MergedROI::borderedLengthOf(roi->paddedLoc.h, scale);
      boxesIfLast.emplace_back(bw, bh);
    }
  }
  return boxesIfLast;
}

void ROIExtractor::prepareFrameLast(Frame* frame, const IntPairs& indices,
                                    const IntPairs& locations) {
  assert(indices.size() == locations.size());
  frame->isLastFrame = true;
  frame->resetProbeROIs();
  int i = 0;
  for (const auto& mergedROI: frame->mergedROIs) {
    if (executionType_ != MONDRIAN || config_.NO_DOWNSAMPLING_FOR_LAST_FRAME) {
      mergedROI->setTargetScale(1.0f);
    }
    mergedROI->setPackInfo(locations[i], indices[i].first, executionType_, ROISize_);
    i++;
  }
  for (const auto& roi: frame->rois) {
    if (roi->scaleLevel() == ROIResizer::INVALID_LEVEL) {
      assert(roi->probeScales.empty());
      continue;
    }
    for (auto probeScale: roi->probeScales) {
      std::unique_ptr<MergedROI> probeROI(new MergedROI({roi.get()}, probeScale, true));
      assert(0.0f < probeScale && probeScale <= 1.0f);
      probeROI->setPackInfo(locations[i], indices[i].first, executionType_, ROISize_);
      roi->roisForProbing.push_back(probeROI.get());
      frame->probingROIs.push_back(std::move(probeROI));
      i++;
    }
  }
  assert(i == locations.size());
}

IntPairs ROIExtractor::getBoxesIfScaled(const Frame* frame) {
  // TODO: Synchronize simulation with add logics
  IntPairs BoxesIfIntermediate;
  for (const auto& mergedROI: frame->mergedROIs) {
    BoxesIfIntermediate.push_back(mergedROI->borderedMatWH());
  }
  return BoxesIfIntermediate;
}

void ROIExtractor::prepareFrameScaled(Frame* frame,
                                      const IntPairs& indices, const IntPairs& locations) {
  assert(indices.size() == locations.size());
  frame->resetProbeROIs();
  int i = 0;
  for (const auto& mergedROI: frame->mergedROIs) {
    auto[bw, bh] = mergedROI->borderedMatWH();
    mergedROI->setPackInfo(locations[i], indices[i].first, executionType_, ROISize_);
    i++;
  }
  assert(i == locations.size());
}

void ROIExtractor::processPD(Frame* currFrame) {
  currFrame->pixelDiffROIProcessStartTime = NowMicros();
  getPixelDiffROIs(currFrame, targetSize_,
                   config_.MAX_PD_ROI_SIZE, config_.MIN_PD_ROI_SIZE,
                   currFrame->rois);
  currFrame->pixelDiffROIProcessEndTime = NowMicros();
  LOGD("PD ROI Extraction took %5lld us for video %d frame %d // %lu PD ROIs",
       currFrame->pixelDiffROIProcessEndTime - currFrame->pixelDiffROIProcessStartTime,
       currFrame->vid, currFrame->frameIndex, currFrame->rois.size());
}

void ROIExtractor::processOF(Frame* currFrame) {
  const Frame* prevFrame = currFrame->prevFrame;
  std::vector<BoundingBox> reliablePrevBoxes;
  if (prevFrame->useInferenceResultForOF) {
    for (const std::unique_ptr<BoundingBox>& box : prevFrame->boxes) {
      if (box->confidence > config_.OF_CONF_THRESHOLD) {
        BoundingBox reliableBox(box->id, Rect(
            std::max(0.0f, box->loc.l),
            std::max(0.0f, box->loc.t),
            std::min(float(currFrame->width), box->loc.r),
            std::min(float(currFrame->height), box->loc.b)),
                                box->confidence, box->label, O_PACKED_CANVAS);
        reliableBox.srcROI = box->srcROI;
        reliablePrevBoxes.push_back(reliableBox);
      }
    }
  } else {
    for (auto& roi : currFrame->prevFrame->rois) {
      BoundingBox reliableBox(roi->id, roi->origLoc, 1, roi->label, roi->origin);
      reliableBox.srcROI = roi.get();
      reliablePrevBoxes.push_back(reliableBox);
    }
  }
  currFrame->opticalFlowROIProcessStartTime = NowMicros();
  getOpticalFlowROIs(prevFrame, currFrame, reliablePrevBoxes, targetSize_, currFrame->rois);
  currFrame->opticalFlowROIProcessEndTime = NowMicros();
  LOGD("OF ROI Extraction took %5lld us for video %d frame %d // %lu OF ROIs",
       currFrame->opticalFlowROIProcessEndTime - currFrame->opticalFlowROIProcessStartTime,
       currFrame->vid, currFrame->frameIndex,
       std::count_if(currFrame->rois.begin(), currFrame->rois.end(),
                     [](auto& roi) { return roi->type == OF; }));
}

void ROIExtractor::getOpticalFlowROIs(const Frame* prevFrame, Frame* currFrame,
                                      const std::vector<BoundingBox>& boundingBoxes,
                                      const cv::Size& targetSize,
                                      std::vector<std::unique_ptr<ROI>>& outChildROIs) {
  auto width = float(currFrame->rgbMat.cols);
  auto height = float(currFrame->rgbMat.rows);

  std::vector<Rect> boundingRects;
  boundingRects.reserve(boundingBoxes.size());
  for (const auto& bbx : boundingBoxes) {
    boundingRects.emplace_back(bbx.loc);
  }

  if (!boundingBoxes.empty()) {
    const std::vector<OFFeatures>& ofFeatures = opticalFlowTracking(
        prevFrame, currFrame, boundingRects, targetSize);
    assert(ofFeatures.size() == boundingBoxes.size());
    for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
      const BoundingBox& box = boundingBoxes[boxIndex];
      const Rect& loc = box.loc;
      const OFFeatures& of = ofFeatures[boxIndex];
      float x = of.shiftAvg.first;
      float y = of.shiftAvg.second;
      float newL = std::max(0.0f, loc.l + x);
      float newT = std::max(0.0f, loc.t + y);
      float newR = std::min(float(width), loc.r + x);
      float newB = std::min(float(height), loc.b + y);
      if (newR - newL >= 1.0f && newB - newT >= 1.0f) {
        outChildROIs.emplace_back(new ROI(
            box.srcROI, box.id, currFrame, {newL, newT, newR, newB},
            OF, box.origin, box.label, of, box.confidence));
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
      inputPoints.emplace_back(
          ((float) bbx.l + (float) bbx.w / 2) * xRatio,
          ((float) bbx.t + (float) bbx.h / 2) * yRatio);
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
      boxShifts.emplace_back(x * (float) currFrame->width / (float) targetSize.width,
                             y * (float) currFrame->height / (float) targetSize.height);
      boxErrs.push_back(errs[j]);
      boxStatusVec.push_back(status[j]);
    }
    ofFeatures.emplace_back(boxShifts, boxErrs, boxStatusVec);
  }
  return ofFeatures;
}

void ROIExtractor::getPixelDiffROIs(Frame* currFrame, const cv::Size& targetSize,
                                    const float maxPDROISize, const float minPDROISize,
                                    std::vector<std::unique_ptr<ROI>>& outChildROIs) const {

  // Find {PD_INTERVAL}th previous frame. If not available, use farthest frame.
  const Frame* prevFrame = currFrame;
  for (int i = 0; i < config_.PD_INTERVAL; i++) {
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
      boxes.emplace_back(
              box.x * float(currFrame->rgbMat.cols) / float(targetSize.width),
              box.y * float(currFrame->rgbMat.rows) / float(targetSize.height),
              (box.x + box.width) * float(currFrame->rgbMat.cols) / float(targetSize.width),
              (box.y + box.height) * float(currFrame->rgbMat.rows) / float(targetSize.height));
    }
  }

  for (const Rect& box: boxes) {
    if (std::min(box.w, box.h) >= 1.0f) {
      outChildROIs.emplace_back(new ROI(
          nullptr,
          INVALID_ID,
          currFrame,
          box,
          PD,
          O_PD,
          -1,
          OFFeatures({}, {}, {}),
          ROI::INVALID_CONF));
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
