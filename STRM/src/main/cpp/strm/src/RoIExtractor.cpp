#include "strm/RoIExtractor.hpp"

#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "opencv2/video/tracking.hpp"

#include "strm/Log.hpp"
#include "strm/MixedFrame.hpp"
#include "strm/Test.hpp"

namespace rm {

const cv::TermCriteria RoIExtractor::CRITERIA = cv::TermCriteria(
    cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.03);

RoIExtractor::RoIExtractor(const RoIExtractorConfig& config, int maxMergeSize, bool run,
                           RoIResizer* roiResizer, std::vector<InferenceInfo> inferencePlan,
                           std::set<int> vids)
    : mConfig(config), mMaxMergeSize(maxMergeSize), mRoIResizer(roiResizer),
      mTargetSize(cv::Size(int(config.EXTRACTION_RESIZE_WIDTH),
                           int(config.EXTRACTION_RESIZE_HEIGHT))),
      mInferencePlan(std::move(inferencePlan)), mFullFrameInferenceCount(0),
      mVids(std::move(vids)), mbStop(false), isFullyPacked(false), mFullFrameVid(-1) {
  if (run) {
    resetBinPackerWithPlan(mInferencePlan);
    mThreads.reserve(config.NUM_WORKERS);
    for (int extractorId = 0; extractorId < config.NUM_WORKERS; extractorId++) {
      mThreads.emplace_back([this, extractorId]() { work(extractorId); });
    }
  }
}

RoIExtractor::~RoIExtractor() {
  mbStop = true;
  queueCv.notify_all();
  for (auto& thread : mThreads) {
    thread.join();
  }
}

void RoIExtractor::enqueue(Frame* frame) {
  std::unique_lock<std::mutex> queueLock(queueMtx);
  queueCv.wait(queueLock, [this]() { return mPDWaiting.size() < mConfig.MAX_QUEUE_SIZE; });
  mPDWaiting.insert(frame);
  LOGD("%-25s                 for video %-5d frame %-4d // %4lu PD | %4lu OF | %4d Processed",
       "RoIExtractor::enqueue", frame->vid, frame->frameIndex,
       mPDWaiting.size(), mOFWaiting.size(),
       std::accumulate(mPackedFrames.begin(), mPackedFrames.end(), 0,
                       [](int cnt, const auto& it) { return cnt + it.second.size(); }));
  queueLock.unlock();
  queueCv.notify_all();
}

void RoIExtractor::notify() {
  queueCv.notify_all();
}

std::tuple<std::vector<MixedFrame>, Frame*, MultiStream, Stream> RoIExtractor::prepareInference(
    std::vector<InferenceInfo>& nextInferencePlan, bool runFull) {
//  LOGD("XXX RoIExtractor::prepareInference before packLock");
  std::unique_lock<std::mutex> packLock(packMtx);
//  LOGD("XXX RoIExtractor::prepareInference before queueLock");
  std::unique_lock<std::mutex> queueLock(queueMtx);
//  LOGD("XXX RoIExtractor::prepareInference after queueLock");

  if (!isFullyPacked) {
    restorePrevs(/*isLast=*/false);
    packAndApplyPrevs(/*isLast=*/true);
    for (int vid: mPrevPackVids) {
      const auto& info = mPrevPackInfos[vid];
      prepareFrameLast(info.frame, info.lastPackedLocations, info.last.indices);
    }
  }
//  LOGD("XXX A");

  Frame* fullFrameTarget = nullptr;
  if (mFullFrameVid != -1) {
    if (!mPackedFrames[mFullFrameVid].empty()) {
      fullFrameTarget = *mPackedFrames[mFullFrameVid].rbegin();
    }
  }
//  LOGD("XXX B");

  std::map<int, std::set<RoI*>> groupedRoIs;
  for (const auto&[vid, frames]: mPackedFrames) {
    for (Frame* frame: frames) {
      if (frame == fullFrameTarget) continue;
      for (auto& cRoI: frame->childRoIs) {
        groupedRoIs[cRoI->packedMixedFrameIndex].insert(cRoI.get());
      }
      for (auto& probeRoI: frame->probingRoIs) {
        groupedRoIs[probeRoI->packedMixedFrameIndex].insert(probeRoI.get());
      }
    }
  }
//  LOGD("XXX C");
  std::vector<MixedFrame> mixedFrames;
  for (auto&[mixedFrameIndex, rois]: groupedRoIs) {
    const auto& info = mInferencePlan[mixedFrameIndex];
    mixedFrames.emplace_back(info.device, rois, info.size);
  }
//  LOGD("XXX D");

  MultiStream selectedFrames = std::move(mPackedFrames);
  mPackedFrames.clear();
  isFullyPacked = false;
  mPrevPackVids.clear();
  mPrevPackInfos.clear();
//  LOGD("XXX E");

  Stream droppedFrames;

  for (Frame* frame: mOFProcessing) {
    frame->extractOFAgain = true;
  }
//  LOGD("XXX F");

  if (runFull) {
    int index = mFullFrameInferenceCount % int(mVids.size());
    mFullFrameInferenceCount++;
    mFullFrameVid = *std::next(mVids.begin(), index);
  } else {
    mFullFrameVid = -1;
  }
  mInferencePlan = nextInferencePlan;
  resetBinPackerWithPlan(mInferencePlan);

  queueLock.unlock();
  packLock.unlock();
  queueCv.notify_all();

  return {mixedFrames, fullFrameTarget, selectedFrames, droppedFrames};
}

void RoIExtractor::work(int extractorId) {
  /*
   *    Frame Status           Containing data structure
   * 1. Before PD extraction | mPDWaiting
   * 2. Extracting PD        | -
   * 3. Before OF extraction | mOFWaiting
   * 4. Extracting OF        | mOFProcessing
   * 5. OF extraction ended  | mBinPacker
   *
   * After mBinPacker full, isFullyPacked will be true.
   */

  auto getPDJob = [this]() {
    if (!mPDWaiting.empty() && mOFWaiting.size() < mConfig.MAX_QUEUE_SIZE) {
      return *mPDWaiting.begin();
    } else {
      return (Frame*) nullptr;
    }
  };
  auto getOFJob = [this]() {
    if (mOFWaiting.empty()) {
      LOGD("XXX getOFJob: %d %d", mOFWaiting.empty(), isFullyPacked);
    } else {
      Frame* firstFrame = *mOFWaiting.begin();
      LOGD("XXX getOFJob: %d %d %d %d %d %d", mOFWaiting.empty(), isFullyPacked, firstFrame->vid, firstFrame->frameIndex, firstFrame->useInferenceResultForOF, firstFrame->readyForOFExtraction());
    }
    if (!mOFWaiting.empty()
        && !isFullyPacked
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

void RoIExtractor::postprocessOF(Frame* currFrame) {
  currFrame->filterPDRoIs(mConfig.PD_FILTER_THRESHOLD);
  std::stringstream ss0;
  for (auto& cRoI: currFrame->childRoIs) {
    ss0 << "(" << cRoI->paddedLoc.width() << ", " << cRoI->paddedLoc.height() << ")" << ", ";
  }
  LOGD("XXX filterPDRoIs: %s", ss0.str().c_str());
  currFrame->resizeStartTime = NowMicros();
  currFrame->resizeRoIs(mRoIResizer);
  currFrame->resizeEndTime = NowMicros();
  std::stringstream ss1;
  for (auto& cRoI: currFrame->childRoIs) {
    ss1 << "(" << cRoI->paddedLoc.width() << ", " << cRoI->paddedLoc.height() << ")" << ", ";
  }
  LOGD("XXX resizeRoIs: %s", ss1.str().c_str());
  currFrame->mergeRoIStartTime = NowMicros();
  if (mConfig.MERGE) {
    currFrame->mergeRoIs(mConfig.MERGE_THRESHOLD, float(mMaxMergeSize));
  }
  currFrame->mergeRoIEndTime = NowMicros();

  IntPairs boxesIfLast = getBoxesIfLastFrame(currFrame);
  IntPairs boxesIfIntermediate = getBoxesIfIntermediateFrame(currFrame);

  std::unique_lock<std::mutex> packLock(packMtx);
  tryPack(currFrame, boxesIfLast, boxesIfIntermediate);
  packLock.unlock();

  std::lock_guard<std::mutex> queueLock(queueMtx);
  mOFProcessing.erase(currFrame);
  if (currFrame->extractOFAgain) {
    LOGD("XXX currFrame->extractOFAgain");
    currFrame->resetOFRoIExtraction();
    mOFWaiting.insert(currFrame);
  } else {
    LOGD("XXX currFrame->isRoIsReady");
    currFrame->isRoIsReady = true;
  }
}

void RoIExtractor::tryPack(Frame* currFrame, const IntPairs& boxesIfLast,
                           const IntPairs& boxesIfIntermediate) {
  const int vid = currFrame->vid;
  LOGD("XXX RoIExtractor::tryPack %d", isFullyPacked);
  if (!isFullyPacked) {
    /*
     * mPrevPackVids contains the candidate last frames.
     * If incoming frame's packing as a last frame fails,
     * mPrevPackVids will be used as last frames.
     */
    bool vidExists = std::find(mPrevPackVids.begin(), mPrevPackVids.end(), vid) != mPrevPackVids.end();
    if (currFrame->vid == mFullFrameVid) {
      LOGD("XXX currFrame->vid == mFullFrameVid");
      assert(!vidExists);
      restorePrevs(/*isLast=*/false);
      auto packIndices = mBinPacker->pack(boxesIfIntermediate, false);
      if (packIndices.size() != boxesIfIntermediate.size()) { // If single scaled packing fails (rare case)
        assert(packIndices.size() < boxesIfIntermediate.size());
        packAndApplyPrevs(/*isLast=*/true);
        isFullyPacked = true;
      } else { // Single scaled packing success
        auto packedLocations = mBinPacker->apply({boxesIfIntermediate, packIndices});
        std::vector<int> packedVids;
        for (int pVid: mPrevPackVids) {
          auto& info = mPrevPackInfos[pVid];
          info.last.indices = mBinPacker->pack(info.last.boxes, true);
          if (info.last.indices.size() != info.last.boxes.size()) {
            isFullyPacked = true;
            break;
          } else {
            info.lastPackedLocations = mBinPacker->apply(info.last);
            packedVids.push_back(pVid);
          }
        }
        if (isFullyPacked) { // Failed to pack last frames
          for (auto it = packedVids.rbegin(); it != packedVids.rend(); it++) {
            auto& info = mPrevPackInfos[*it];
            mBinPacker->restore(info.last);
          }
          mBinPacker->restore({boxesIfIntermediate, packIndices});
          packAndApplyPrevs(/*isLast=*/true);
          for (int pVid: mPrevPackVids) {
            const auto& info = mPrevPackInfos[pVid];
            prepareFrameLast(info.frame, info.lastPackedLocations, info.last.indices);
          }
        } else { // Success to pack last frames
          packAndApplyPrevs(/*isLast=*/false);
        }
      }
    } else {
      LOGD("XXX currFrame->vid != mFullFrameVid");
      restorePrevs(/*isLast=*/false);
      if (vidExists) {
        packAndApplyPrev(/*isLast=*/false, /*targetVid=*/vid);
      }
      packAndApplyPrevs(/*isLast=*/true, /*skipVid=*/vid);
      auto packIndicesLast = mBinPacker->pack(boxesIfLast, true);
      LOGD("XXX mBinPacker->pack(boxesIfLast, true); %lu %lu ", packIndicesLast.size(), boxesIfLast.size());
      if (packIndicesLast.size() == boxesIfLast.size()) { // Packing success
        restorePrevs(/*isLast=*/true, /*skipVid=*/vid);
        if (vidExists) {
          auto& prevVidInfo = mPrevPackInfos[vid];
          prepareFrameIntermediate(prevVidInfo.frame,
                                   prevVidInfo.interPackedLocations,
                                   prevVidInfo.inter.indices);
          mPrevPackVids.erase(mPrevPackVids.begin());
        }
        mPrevPackVids.push_back(vid);
        mPackedFrames[currFrame->vid].insert(currFrame);
        auto& info = mPrevPackInfos[vid];
        info.frame = currFrame;
        info.last.boxes = boxesIfLast;
        info.inter.boxes = boxesIfIntermediate;
        packAndApplyPrevs(/*isLast=*/false);
      } else { // Packing failed
        assert(packIndicesLast.size() < boxesIfLast.size());
        if (vidExists) {
          restorePrevs(/*isLast=*/true, /*skipVid=*/vid);
          restorePrev(/*isLast=*/false, /*targetVid=*/vid);
          packAndApplyPrevs(/*isLast=*/true);
        }
        for (int pVid: mPrevPackVids) {
          const auto& info = mPrevPackInfos[pVid];
          prepareFrameLast(info.frame, info.lastPackedLocations, info.last.indices);
        }
        isFullyPacked = true;
      }
    }
  }
  if (isFullyPacked) {
    std::lock_guard<std::mutex> queueLock(queueMtx);
    for (Frame* frame: mOFProcessing) {
      frame->extractOFAgain = true;
    }
  }
}

void RoIExtractor::packAndApplyPrevs(bool isLast, int skipVid) {
  for (int pVid : mPrevPackVids) {
    if (pVid != skipVid) {
      packAndApplyPrev(isLast, pVid);
    }
  }
}

void RoIExtractor::packAndApplyPrev(bool isLast, int targetVid) {
  auto& info = mPrevPackInfos[targetVid];
  if (isLast) {
    info.last.indices = mBinPacker->pack(info.last.boxes, true);
    assert(info.last.indices.size() == info.last.boxes.size());
    info.lastPackedLocations = mBinPacker->apply(info.last);
  } else {
    info.inter.indices = mBinPacker->pack(info.inter.boxes, false);
    assert(info.inter.indices.size() == info.inter.boxes.size());
    info.interPackedLocations = mBinPacker->apply(info.inter);
  }
}

void RoIExtractor::restorePrevs(bool isLast, int skipVid) {
  for (auto it = mPrevPackVids.rbegin(); it != mPrevPackVids.rend(); it++) {
    if (*it != skipVid) {
      restorePrev(isLast, *it);
    }
  }
}

void RoIExtractor::restorePrev(bool isLast, int targetVid) {
  auto& info = mPrevPackInfos[targetVid];
  if (isLast) {
    mBinPacker->restore(info.last);
  } else {
    mBinPacker->restore(info.inter);
  }
}

IntPairs RoIExtractor::getBoxesIfLastFrame(const Frame* frame) {
  // TODO: Synchronize simulation with add logics
  IntPairs boxesIfLast;
  for (const auto& pRoI : frame->parentRoIs) {
    int w = int(pRoI->paddedLoc.width());
    int h = int(pRoI->paddedLoc.height());
    boxesIfLast.emplace_back(w, h);
    LOGD("XXX getBoxesIfLastFrame parentRoIs: %d %d", w, h);
  }
  for (const auto& cRoI : frame->childRoIs) {
    std::vector<float> probingCandidates = mRoIResizer->getProbingCandidates(
        cRoI->getTargetScale(), cRoI->getScaleLevel(), mRoIResizer->getNumProbeSteps());
    for (auto scale: probingCandidates) {
      int w = int(cRoI->paddedLoc.width() * scale);
      int h = int(cRoI->paddedLoc.height() * scale);
      boxesIfLast.emplace_back(w, h);
      LOGD("XXX getBoxesIfLastFrame probeRoIs: %d %d", w, h);
    }
  }
  return boxesIfLast;
}

void RoIExtractor::prepareFrameLast(Frame* frame,
                                    const IntPairs& packedLocations,
                                    const IntPairs& packedIndices) {
  assert(packedLocations.size() == packedIndices.size());
  frame->resetProbeRoIs();
  int i = 0;
  for (const auto& pRoI: frame->parentRoIs) {
    pRoI->packedLocation = packedLocations[i];
    pRoI->packedMixedFrameIndex = packedIndices[i].first;
    i++;
  }
  for (const auto& cRoI: frame->childRoIs) {
    std::vector<float> probingCandidates = mRoIResizer->getProbingCandidates(
        cRoI->getTargetScale(), cRoI->getScaleLevel(), mRoIResizer->getNumProbeSteps());
    for (auto scale: probingCandidates) {
      std::unique_ptr<RoI> probeRoI = std::make_unique<RoI>(
          nullptr, cRoI->id, cRoI->frame, cRoI->paddedLoc, cRoI->type, cRoI->origin, cRoI->label,
          cRoI->features.ofFeatures, RoI::INVALID_CONF, 0, true);
      assert(0.0f < scale && scale <= 1.0f);
      probeRoI->setTargetScale(scale, cRoI->getScaleLevel());
      probeRoI->packedLocation = packedLocations[i];
      probeRoI->packedMixedFrameIndex = packedIndices[i].first;
      i++;
      cRoI->roisForProbing.push_back(probeRoI.get());
      frame->probingRoIs.push_back(std::move(probeRoI));
    }
  }
  assert(i == packedLocations.size());
}

IntPairs RoIExtractor::getBoxesIfIntermediateFrame(const Frame* frame) {
  // TODO: Synchronize simulation with add logics
  IntPairs BoxesIfIntermediate;
  for (const auto& pRoI : frame->parentRoIs) {
    auto[rw, rh] = pRoI->getResizedWidthHeight();
    BoxesIfIntermediate.emplace_back(int(rw), int(rh));
  }
  return BoxesIfIntermediate;
}

void RoIExtractor::prepareFrameIntermediate(Frame* frame,
                                            const IntPairs& packedLocations,
                                            const IntPairs& packedIndices) {
  assert(packedLocations.size() == packedIndices.size());
  frame->resetProbeRoIs();
  int i = 0;
  for (const auto& pRoI : frame->parentRoIs) {
    pRoI->packedLocation = packedLocations[i];
    pRoI->packedMixedFrameIndex = packedIndices[i].first;
    i++;
  }
  assert(i == packedLocations.size());
}

void RoIExtractor::processPD(Frame* currFrame) {
  currFrame->pixelDiffRoIProcessStartTime = NowMicros();
  getPixelDiffRoIs(currFrame, mTargetSize, mConfig.MIN_ROI_AREA, currFrame->childRoIs);
  currFrame->pixelDiffRoIProcessEndTime = NowMicros();
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu PD RoIs",
       "RoIExtractor::processPD",
       currFrame->pixelDiffRoIProcessEndTime - currFrame->pixelDiffRoIProcessStartTime,
       currFrame->vid, currFrame->frameIndex, currFrame->childRoIs.size());
}

void RoIExtractor::processOF(Frame* currFrame) {
  assert(!currFrame->extractOFAgain);
  Frame* prevFrame = currFrame->prevFrame;
  LOGD("XXX processOF prevFrame %d %d: %lu", prevFrame->vid, prevFrame->frameIndex, prevFrame->boxes.size());
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
        reliableBox.srcRoI = box->srcRoI;
        reliablePrevBoxes.push_back(reliableBox);
      }
    }
  } else {
    for (auto& cRoI : currFrame->prevFrame->childRoIs) {
      BoundingBox reliableBox(cRoI->id, cRoI->origLoc, 1, cRoI->label, cRoI->origin);
      reliableBox.srcRoI = cRoI.get();
      reliablePrevBoxes.push_back(reliableBox);
    }
  }
  std::stringstream ss;
  for (auto& box : reliablePrevBoxes) {
    auto& loc = box.location;
    ss << "(" << loc.left << ", " << loc.top << ", " << loc.right << ", " << loc.bottom << "), ";
  }
  LOGD("XXX reliablePrevBoxes %d: %s", prevFrame->useInferenceResultForOF, ss.str().c_str());
  currFrame->opticalFlowRoIProcessStartTime = NowMicros();
  getOpticalFlowRoIs(prevFrame, currFrame, reliablePrevBoxes, mTargetSize, currFrame->childRoIs);
  currFrame->opticalFlowRoIProcessEndTime = NowMicros();
  LOGD("%-25s took %-7lld us for video %-5d frame %-4d // %4lu OF RoIs", "RoIExtractor::processOF",
       currFrame->opticalFlowRoIProcessEndTime - currFrame->opticalFlowRoIProcessStartTime,
       currFrame->vid, currFrame->frameIndex,
       std::count_if(currFrame->childRoIs.begin(), currFrame->childRoIs.end(),
                     [](auto& cRoI) { return cRoI->type == OF; }));
}

void RoIExtractor::getOpticalFlowRoIs(const Frame* prevFrame, Frame* currFrame,
                                      const std::vector<BoundingBox>& boundingBoxes,
                                      const cv::Size& targetSize,
                                      std::vector<std::unique_ptr<RoI>>& outChildRoIs) const {
  auto width = float(currFrame->mat.cols);
  auto height = float(currFrame->mat.rows);

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
      if (newLeft < newRight && newTop < newBottom) {
        LOGD("XXX OF RoI: (%f, %f, %f, %f) %f", newLeft, newTop, newRight, newBottom, mConfig.ROI_PADDING);
        outChildRoIs.push_back(std::make_unique<RoI>(
            box.srcRoI, box.id, currFrame, Rect(newLeft, newTop, newRight, newBottom),
            OF, box.origin, box.label, of, box.confidence, mConfig.ROI_PADDING, false));
      }
    }
  }
}

std::vector<OFFeatures> RoIExtractor::opticalFlowTracking(
    const Frame* prevFrame, const Frame* currFrame,
    const std::vector<Rect>& boundingBoxes, const cv::Size& targetSize) {
  assert(!prevFrame->preProcessedMat.empty());
  assert(!currFrame->preProcessedMat.empty());
  assert(prevFrame->preProcessedMat.channels() == currFrame->preProcessedMat.channels());

  const cv::Mat& prevImage = prevFrame->preProcessedMat;
  const cv::Mat& currImage = currFrame->preProcessedMat;

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

void RoIExtractor::getPixelDiffRoIs(Frame* currFrame, const cv::Size& targetSize,
                                    const float minRoIArea,
                                    std::vector<std::unique_ptr<RoI>>& outChildRoIs) const {

  // Find {PD_INTERVAL}th previous frame. If not available, use farthest frame.
  Frame* prevFrame = currFrame;
  for (int i = 0; i < mConfig.PD_INTERVAL; ++i) {
    assert(prevFrame != nullptr);
    if (prevFrame->prevFrame == nullptr) {
      break;
    }
    prevFrame = prevFrame->prevFrame;
  }

  assert(!prevFrame->preProcessedMat.empty());
  assert(!currFrame->preProcessedMat.empty());
  assert(prevFrame->preProcessedMat.channels() == currFrame->preProcessedMat.channels());
  cv::Mat mat = calculateDiffAndThreshold(prevFrame->preProcessedMat, currFrame->preProcessedMat);
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
    if (box.area() >= minRoIArea) {
      boxes.push_back(Rect(
          box.x * float(currFrame->mat.cols) / float(targetSize.width),
          box.y * float(currFrame->mat.rows) / float(targetSize.height),
          (box.x + box.width) * float(currFrame->mat.cols) / float(targetSize.width),
          (box.y + box.height) * float(currFrame->mat.rows) / float(targetSize.height)));
    }
  }

  for (const Rect& box : boxes) {
    LOGD("XXX PD RoI: (%f, %f, %f, %f) %f", box.left, box.top, box.right, box.bottom, mConfig.ROI_PADDING);
    outChildRoIs.push_back(std::make_unique<RoI>(
        nullptr,
        UNASSIGNED_ID,
        currFrame,
        box,
        PD,
        origin_PD,
        -1,
        OFFeatures({}, {}, {}),
        RoI::INVALID_CONF,
        mConfig.ROI_PADDING,
        false));
  }
}

cv::Mat RoIExtractor::calculateDiffAndThreshold(
    const cv::Mat& prevMat, const cv::Mat& currMat) {
  cv::Mat diff;
  cv::absdiff(prevMat, currMat, diff);
  cv::dilate(diff, diff,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(0, 0),
             2);
  cv::threshold(diff, diff, 35, 255, cv::THRESH_BINARY);
  return diff;
}

void RoIExtractor::cannyEdgeDetection(cv::Mat mat) {
  cv::Canny(mat, mat, 120, 255, 3, false);
  cv::dilate(mat, mat,
             cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)),
             cv::Point(0, 0),
             1);
}

void RoIExtractor::resetBinPackerWithPlan(const std::vector<InferenceInfo>& inferencePlan) {
  IntPairs WHs;
  for (const auto& info : inferencePlan) {
    WHs.emplace_back(info.size, info.size);
  }
  mBinPacker = std::make_unique<BinPacker>(WHs);
}

} // namespace rm
