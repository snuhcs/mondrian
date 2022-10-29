#include "strm/Frame.hpp"

#include "strm/RoI.hpp"
#include "strm/RoIResizer.hpp"

namespace rm {

Frame::Frame(const int vid, const int frameIndex, const cv::Mat mat,
             Frame* prevFrame, const time_us& enqueueTime)
    : vid(vid), frameIndex(frameIndex), mat(mat),
      width(mat.cols), height(mat.rows), prevFrame(prevFrame), useInferenceResultForOF(false),
      extractOFAgain(false), enqueueTime(enqueueTime), isFullFrameTarget(false),
      isBoxesReady(false), isRoIsReady(false), PDExtractorID(-1), OFExtractorID(-1),
      inferenceFrameSize(0) {}

void Frame::resizeRoIs(RoIResizer* roiResizer) {
  for (auto& cRoI: childRoIs) {
    if (cRoI->type == OF) {
      auto[scale, level] = roiResizer->getTargetScale(cRoI->id, cRoI->features);
      cRoI->setTargetScale(scale, level);
    }
  }
  for (auto& cRoI: childRoIs) {
    if (cRoI->type == PD && cRoI->nextRoI != nullptr) {
      cRoI->setTargetScale(cRoI->nextRoI->getTargetScale(), cRoI->nextRoI->getScaleLevel());
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
    if (cRoI->type == OF) {
      OFRoIs.push_back(cRoI.get());
    }
  }

  for (auto it = childRoIs.begin(); it != childRoIs.end();) {
    auto& cRoI = *it;
    if (cRoI->type == PD) {
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
    if (cRoI->type == PD) {
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

} // namespace rm
