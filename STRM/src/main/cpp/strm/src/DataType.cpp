#include "strm/DataType.hpp"

#include "strm/Log.hpp"
#include "strm/Logger.hpp"
#include "strm/RoIResizer.hpp"

namespace rm {

const idType UNASSIGNED_ID = -1;
const idType MERGED_ROI_ID = -2;

const std::pair<float, float> RoI::NOT_PACKED{-1, -1};

void Frame::resizeRoIs(RoIResizer* roiResizer) {
  for (auto& cRoI : childRoIs) {
    if (cRoI->type == RoI::Type::OF) {
      cRoI->setTargetSize(roiResizer->getTargetSize(cRoI->id, cRoI->features));
    }
  }
  for (auto& cRoI : childRoIs) {
    if (cRoI->type == RoI::Type::PD && cRoI->nextRoI != nullptr) {
      cRoI->setTargetSize(cRoI->nextRoI->getTargetSize());
    }
  }
}

void Frame::resetParentRoIs() {
  parentRoIs.clear();
  assert(parentRoIs.empty());
  for (auto& cRoI : childRoIs) {
    assert(cRoI->childRoIs.empty());
    assert(cRoI->roisForProbing.empty());
  }
  for (auto& cRoI : childRoIs) {
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
        if (roi0->getTargetSize() * roi1->maxEdgeLength >
            roi1->getTargetSize() * roi0->maxEdgeLength) {
          // If roi0 resizes conservatively than roi1
          newArea = newArea * roi0->getTargetSize() * roi0->getTargetSize()
                    / roi0->maxEdgeLength / roi0->maxEdgeLength;
        } else {
          // If roi1 resizes conservatively than roi0
          newArea = newArea * roi1->getTargetSize() * roi1->getTargetSize()
                    / roi1->maxEdgeLength / roi1->maxEdgeLength;
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
    for (RoI* cRoI : mergedRoI->childRoIs) {
      cRoI->parentRoI = mergedRoI;
    }
    assert(j > i);
    parentRoIs.erase(parentRoIs.begin() + j);
    parentRoIs.erase(parentRoIs.begin() + i);
  }
}

void Frame::addProbeRoIs(int numProbeSteps, float probeStepSize) {
  assert(probingRoIs.empty());
  for (auto& cRoI : childRoIs) {
    assert(cRoI->frame == this);
    assert(cRoI->roisForProbing.empty());
    float probe = float(-numProbeSteps) * probeStepSize;
    for (int i = 0; i < 2 * numProbeSteps + 1; i++) {
      std::unique_ptr<RoI> probeRoI = std::make_unique<RoI>(
          nullptr, cRoI->id, cRoI->frame, cRoI->paddedLoc, cRoI->type, cRoI->origin, cRoI->label,
          cRoI->features.ofFeatures, 0, true);
      probeRoI->setTargetSize(cRoI->getTargetSize() + probe);
      cRoI->roisForProbing.push_back(probeRoI.get());
      probingRoIs.push_back(std::move(probeRoI));
      probe += probeStepSize;
    }
  }
}

void Frame::resetProbeRoIs() {
  for (auto& cRoI : childRoIs) {
    cRoI->roisForProbing.clear();
    probingRoIs.clear();
  }
}

void Frame::filterPDRoIs(float threshold) {
  std::vector<RoI*> OFRoIs;
  for (auto& cRoI : childRoIs) {
    if (cRoI->type == RoI::Type::OF) {
      OFRoIs.push_back(cRoI.get());
    }
  }

  for (auto it = childRoIs.begin(); it != childRoIs.end();) {
    auto& cRoI = *it;
    if (cRoI->type == RoI::Type::PD) {
      float totalOFCoverage = 0;
      for (RoI* OFRoI : OFRoIs) {
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
  for (const auto& pRoI : parentRoIs) {
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

std::string Frame::toShortKey(const std::string& key) {
  return key.substr(key.size() - 1, 1);
}

std::set<Frame*> filterLastFrames(const MultiStream& frames) {
  std::set<Frame*> lastFrames;
  for (auto it : frames) {
    if (!it.second.empty()) {
      lastFrames.insert(*it.second.rbegin());
    }
  }
  return lastFrames;
}

std::string toString(const MultiStream& frames) {
  std::stringstream ss;
  for (auto it = frames.begin(); it != frames.end(); it++) {
    std::string shortKey = Frame::toShortKey(it->first);
    ss << "video " << shortKey << ": ";
    if (it->first.empty()) {
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

FrameBuffer::FrameBuffer(const std::string& key, int capacity)
    : key(key), shortKey(Frame::toShortKey(key)), capacity(capacity), count(0) {
  frames.resize(capacity);
}

Frame* FrameBuffer::enqueue(const cv::Mat& mat) {
  std::unique_lock<std::mutex> lock(mtx);
  int frameIndex = count++;
  cv.wait(lock, [this, frameIndex]() { return frames[frameIndex % capacity].get() == nullptr; });
  Frame* prevFrame = frameIndex == 0 ? nullptr : frames[(frameIndex - 1) % capacity].get();
  frames[frameIndex % capacity] = std::make_unique<Frame>(key, frameIndex, mat, prevFrame,
                                                          NowMicros());
  if (prevFrame != nullptr) {
    prevFrame->nextFrame = frames[frameIndex % capacity].get();
  }
  Frame* currFrame = frames[frameIndex % capacity].get();
  lock.unlock();
  LOGD("%-25s                 for video %-5s frame %-4d",
       "FrameBuffer::enqueue", shortKey.c_str(), frameIndex);
  return currFrame;
}

void
FrameBuffer::freeImage(const std::vector<int>& frameIndices, Logger* logger, Logger* roiLogger) {
  std::unique_lock<std::mutex> lock(mtx);
  for (int frameIndex : frameIndices) {
    Frame* frame = frames[frameIndex % capacity].get();
    if (logger != nullptr) {
      logger->log(frame);
    }
    if (roiLogger != nullptr) {
      for (auto& cRoI : frame->childRoIs) {
        roiLogger->logRoI(cRoI.get());
      }
    }
    frames[frameIndex % capacity].reset();
  }
  lock.unlock();
  cv.notify_all();
  LOGD("%-25s                 for video %-5s frame %-4d ~ %-4d",
       "FrameBuffer::freeImage", shortKey.c_str(), frameIndices.front(), frameIndices.back());
}

int MixedFrame::numMixedFrames = 0;

} // namespace rm
