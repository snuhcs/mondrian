#include "strm/DataType.hpp"

#include <numeric>

#include "strm/Log.hpp"
#include "strm/Logger.hpp"
#include "strm/RoIResizer.hpp"

namespace rm {

const idType UNASSIGNED_ID = -1;
const idType MERGED_ROI_ID = -2;

const std::pair<float, float> RoI::NOT_PACKED{-1, -1};

void Frame::resizeRoIs(RoIResizer* roiResizer) {
  for (auto& cRoI: childRoIs) {
    if (cRoI->type == RoI::Type::OF) {
      auto[scale, level] = roiResizer->getTargetScale(cRoI->id, cRoI->features);
      cRoI->setTargetScale(scale, level);
    }
  }
  for (auto& cRoI: childRoIs) {
    if (cRoI->type == RoI::Type::PD && cRoI->nextRoI != nullptr) {
      cRoI->setTargetScale(roiResizer->maxScale(), roiResizer->maxLevel());
    }
  }
}

void Frame::resetParentRoIs() {
  parentRoIs.clear();
  assert(parentRoIs.empty());
  for (auto& cRoI: childRoIs) {
    assert(cRoI->childRoIs.empty());
    assert(cRoI->roisForProbing.empty());
  }
  for (auto& cRoI: childRoIs) {
    std::unique_ptr<RoI> pRoI = std::make_unique<RoI>(*cRoI);
    cRoI->parentRoI = pRoI.get();
    pRoI->childRoIs.push_back(cRoI.get());
    parentRoIs.push_back(std::move(pRoI));
  }
}

void Frame::mergeRoIs(float mergeThreshold, float maxSize) {
  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < parentRoIs.size(); i++) {
      for (j = i + 1; j < parentRoIs.size(); j++) {
        const std::unique_ptr<RoI>& roi0 = parentRoIs[i];
        const std::unique_ptr<RoI>& roi1 = parentRoIs[j];
        float intersection = roi0->paddedLoc.intersection(roi1->paddedLoc);
        if (intersection / roi0->getPaddedArea() < mergeThreshold &&
            intersection / roi1->getPaddedArea() < mergeThreshold) {
          continue;
        }
        float newLeft = std::min(roi0->paddedLoc.left, roi1->paddedLoc.left);
        float newTop = std::min(roi0->paddedLoc.top, roi1->paddedLoc.top);
        float newRight = std::max(roi0->paddedLoc.right, roi1->paddedLoc.right);
        float newBottom = std::max(roi0->paddedLoc.bottom, roi1->paddedLoc.bottom);
        if (newRight - newLeft > maxSize || newBottom - newTop > maxSize) {
          continue;
        }
        float newArea = (newRight - newLeft) * (newBottom - newLeft);
        if (roi0->getTargetScale() > roi1->getTargetScale()) {
          // If roi0 resizes conservatively than roi1
          newArea = newArea * roi0->getTargetScale() * roi0->getTargetScale();
        } else {
          // If roi1 resizes conservatively than roi0
          newArea = newArea * roi1->getTargetScale() * roi1->getTargetScale();
        }
        float originalArea = roi0->getResizedArea() + roi1->getResizedArea();
        if (newArea >= originalArea) {
          continue;
        }
        updated = true;
        break;
      }
      if (updated) {
        break;
      }
    }
    if (!updated) {
      break;
    }
    parentRoIs.push_back(std::move(RoI::mergeRoIs(parentRoIs[i].get(), parentRoIs[j].get())));
    // Match child parent
    RoI* mergedRoI = parentRoIs.back().get();
    mergedRoI->childRoIs.insert(mergedRoI->childRoIs.end(),
                                parentRoIs[i]->childRoIs.begin(), parentRoIs[i]->childRoIs.end());
    mergedRoI->childRoIs.insert(mergedRoI->childRoIs.end(),
                                parentRoIs[j]->childRoIs.begin(), parentRoIs[j]->childRoIs.end());
    for (RoI* cRoI: mergedRoI->childRoIs) {
      cRoI->parentRoI = mergedRoI;
    }
    assert(j > i);
    parentRoIs.erase(parentRoIs.begin() + j);
    parentRoIs.erase(parentRoIs.begin() + i);
  }
}

void Frame::addProbeRoIs(RoIResizer* mRoIResizer) {
  assert(probingRoIs.empty());
  for (auto& cRoI: childRoIs) {
    assert(cRoI->frame == this);
    assert(cRoI->roisForProbing.empty());
    std::vector<float> probingCandidates = mRoIResizer->getProbingCandidates(
        cRoI->getTargetScale(), cRoI->getScaleLevel(), mRoIResizer->getNumProbeSteps());
    for (auto scale: probingCandidates) {
      std::unique_ptr<RoI> probeRoI = std::make_unique<RoI>(
          nullptr, cRoI->id, cRoI->frame, cRoI->paddedLoc, cRoI->type, cRoI->origin, cRoI->label,
          cRoI->features.ofFeatures, RoI::INVALID_CONF, 0, true);
      probeRoI->setTargetScale(scale, cRoI->getScaleLevel());
      cRoI->roisForProbing.push_back(probeRoI.get());
      probingRoIs.push_back(std::move(probeRoI));
    }
  }
}

void Frame::resetProbeRoIs() {
  for (auto& cRoI: childRoIs) {
    cRoI->roisForProbing.clear();
    probingRoIs.clear();
  }
}

void Frame::filterPDRoIs(float threshold) {
  std::vector<RoI*> OFRoIs;
  for (auto& cRoI: childRoIs) {
    if (cRoI->type == RoI::Type::OF) {
      OFRoIs.push_back(cRoI.get());
    }
  }

  for (auto it = childRoIs.begin(); it != childRoIs.end();) {
    auto& cRoI = *it;
    if (cRoI->type == RoI::Type::PD) {
      float totalOFCoverage = 0;
      for (RoI* OFRoI: OFRoIs) {
        float intersection = cRoI->paddedLoc.intersection(OFRoI->paddedLoc);
        totalOFCoverage += intersection;
      }
      if (totalOFCoverage / cRoI->getPaddedArea() >= threshold) {
        it = childRoIs.erase(it);
        continue;
      }
    }
    it++;
  }

  for (auto& cRoI: childRoIs) {
    if (cRoI->type == RoI::PD) {
      assert(cRoI->id == UNASSIGNED_ID);
      cRoI->id = RoI::getNewIds(1).first;
    } else {
      assert(cRoI->id != UNASSIGNED_ID);
    }
  }
}

bool Frame::isReadyToMarry(int mixedFrameIndex) const {
  bool atLeastOneIndexIsSame = false;
  for (const auto& pRoI: parentRoIs) {
    if (!pRoI->isPacked()) {
      continue;
    }
    if (pRoI->packedMixedFrameIndex > mixedFrameIndex) {
      return false;
    }
    atLeastOneIndexIsSame |= (pRoI->packedMixedFrameIndex == mixedFrameIndex);
  }
  return atLeastOneIndexIsSame;
}

bool Frame::readyForOFExtraction() const {
  if (prevFrame->useInferenceResultForOF) {
    return prevFrame->isBoxesReady;
  } else {
    return prevFrame->isRoIsReady;
  }
}

std::set<Frame*> filterLastFrames(const MultiStream& frames) {
  std::set<Frame*> lastFrames;
  for (auto it: frames) {
    if (!it.second.empty()) {
      lastFrames.insert(*it.second.rbegin());
    }
  }
  return lastFrames;
}

std::string toString(const MultiStream& frames) {
  std::stringstream ss;
  for (auto it = frames.begin(); it != frames.end(); it++) {
    ss << "video " << it->first << ": ";
    if (it->second.empty()) {
      ss << "EMPTY";
    } else {
      Frame* firstFrame = *(it->second.begin());
      Frame* lastFrame = *(it->second.rbegin());
      ss << firstFrame->frameIndex << " ~ " << lastFrame->frameIndex;
    }
    if (it != std::prev(frames.end())) {
      ss << ", ";
    }
  }
  return ss.str();
}

std::string toString(const std::vector<InferenceInfo>& inferencePlan) {
  std::stringstream ss;
  for (int i = int(inferencePlan.size()) - 1; i >= 0; i--) {
    const InferenceInfo& info = inferencePlan[i];
    // TODO: support other processors
    ss << "(" << (info.device == GPU ? "GPU" : "DSP") << ", "
       << info.size << ", "
       << info.accumulatedLatency << ")";
    if (i != 0) {
      ss << ", ";
    }
  }
  return ss.str();
}

FrameBuffer::FrameBuffer(int vid, int capacity, int startIndex)
    : vid(vid), capacity(capacity), count(startIndex) {
  frames.resize(capacity);
}

Frame* FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this, frameIndex]() { return frames[frameIndex % capacity].get() == nullptr; });
  Frame* prevFrame = frameIndex == 0 ? nullptr : frames[(frameIndex - 1) % capacity].get();
  frames[frameIndex % capacity] = std::make_unique<Frame>(
      vid, frameIndex, mat, prevFrame, NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[frameIndex % capacity].get();
  }
  Frame* currFrame = frames[frameIndex % capacity].get();
  lock.unlock();
  LOGD("%-25s                 for video %-5d frame %-4d",
       "FrameBuffer::enqueue", vid, frameIndex);
  return currFrame;
}

void FrameBuffer::freeImage(const std::vector<int>& frameIndices) {
  std::unique_lock<std::mutex> lock(mtx);
  // Hide them from any other frame's eyesight
  for (int frameIndex: frameIndices) {
    auto frame = frames[frameIndex % capacity].get();

    assert(frame != nullptr);
    assert(frame->nextFrame != nullptr);
    assert(frame->nextFrame->prevFrame == frame);
    if (frame->prevFrame != nullptr) {
      assert(frame->prevFrame->nextFrame == frame);
    }
    frames[frameIndex % capacity]->nextFrame->prevFrame = nullptr;
  }
  // Reset smart pointers
  for (int frameIndex: frameIndices) {
    assert(frames[frameIndex % capacity].get() != nullptr);
    frames[frameIndex % capacity].reset();
  }
  lock.unlock();
  cv.notify_all();
  LOGD("%-25s                 for video %-5d frame %-4d ~ %-4d",
       "FrameBuffer::freeImage", vid, frameIndices.front(), frameIndices.back());
}

const int RoI::INVALID_CONF = -1;

RoI::RoI(RoI* prevRoI,
         const idType id,
         Frame* frame,
         const Rect origLoc,
         const Type type,
         const Origin origin,
         const int label,
         const OFFeatures ofFeatures,
         const float confidence,
         float roiPadding,
         bool isProbingRoI)
    : prevRoI(prevRoI), id(id), frame(frame), origLoc(origLoc), paddedLoc(
    std::max(0.0f, origLoc.left - roiPadding),
    std::max(0.0f, origLoc.top - roiPadding),
    std::min(float(frame->mat.cols), origLoc.right + roiPadding),
    std::min(float(frame->mat.rows), origLoc.bottom + roiPadding)),
      type(type), origin(origin), label(label), features{
        paddedLoc.width(),
        paddedLoc.height(),
        label,
        type,
        origin,
        (float) origLoc.width() / (float) origLoc.height(),
        ofFeatures,
        confidence
    }, maxEdgeLength(std::max(paddedLoc.width(), paddedLoc.height())),
      targetScale(1.0f), scaleLevel(RoIResizer::INVALID_LEVEL), packedLocation(NOT_PACKED),
      isMatchTried(false), nextRoI(nullptr), parentRoI(nullptr), box(nullptr), probingBox(nullptr),
      packedMixedFrameIndex(INT_MAX), packedAbsMixedFrameIndex(-1),
      isProbingRoI(isProbingRoI), priority(-1) {
  if (prevRoI != nullptr) {
    prevRoI->nextRoI = this;
  }
}

std::unique_ptr<RoI> RoI::mergeRoIs(const RoI* pRoI0, const RoI* pRoI1) {
  assert(pRoI0->frame == pRoI1->frame);
  float newLeft = std::min(pRoI0->paddedLoc.left, pRoI1->paddedLoc.left);
  float newTop = std::min(pRoI0->paddedLoc.top, pRoI1->paddedLoc.top);
  float newRight = std::max(pRoI0->paddedLoc.right, pRoI1->paddedLoc.right);
  float newBottom = std::max(pRoI0->paddedLoc.bottom, pRoI1->paddedLoc.bottom);
  RoI::Type roiType = pRoI0->type != RoI::Type::PD || pRoI1->type != RoI::Type::PD
                      ? RoI::Type::OF
                      : RoI::Type::PD;
  int roiLabel;
  if (pRoI0->label == pRoI1->label) {
    roiLabel = pRoI0->label;
  } else if (pRoI0->label != -1 && pRoI1->label == -1) {
    roiLabel = pRoI0->label;
  } else if (pRoI0->label == -1 && pRoI1->label != -1) {
    roiLabel = pRoI1->label;
  } else {
    roiLabel = -1;
  }
  std::unique_ptr<RoI> mergedRoI = std::make_unique<RoI>(
      nullptr, MERGED_ROI_ID, pRoI0->frame, Rect(newLeft, newTop, newRight, newBottom),
      roiType, origin_Null, roiLabel, OFFeatures({}, {}, {}), RoI::INVALID_CONF, 0, false);
  mergedRoI->setTargetScale(pRoI0->targetScale > pRoI1->targetScale ?
                            pRoI0->targetScale : pRoI1->targetScale, RoIResizer::INVALID_LEVEL);
  return std::move(mergedRoI);
}

void RoI::setTargetScale(float newTargetScale, int newScaleLevel) {
  assert(0 <= newTargetScale);
  assert(newTargetScale <= 1);
  float minEdgeLength = std::min(paddedLoc.width(), paddedLoc.height());
  // compare with 1/minEdgeLength to prevent shorter edge being even shorter than 1 after downscaling
  targetScale = std::max(1 / minEdgeLength, newTargetScale);
  scaleLevel = newScaleLevel;
}

int MixedFrame::numMixedFrames = 0;

} // namespace rm
