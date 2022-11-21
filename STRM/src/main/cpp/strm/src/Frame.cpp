#include "strm/Frame.hpp"

#include "strm/RoI.hpp"
#include "strm/RoIResizer.hpp"
#include "strm/Test.hpp"
#include "strm/Log.hpp"

namespace rm {

const int Frame::FULL_KEY_OFFSET = 1000000;

Frame::Frame(const int vid, const int frameIndex, const cv::Mat mat,
             Frame* prevFrame, const time_us& enqueueTime)
    : vid(vid), frameIndex(frameIndex), mat(mat),
      width(mat.cols), height(mat.rows), prevFrame(prevFrame), nextFrame(nullptr),
      useInferenceResultForOF(false), extractOFAgain(false), enqueueTime(enqueueTime),
      isBoxesReady(false), isRoIsReady(false), PDExtractorID(-1), OFExtractorID(-1),
      inferenceFrameSize(0), inferenceDevice(NO_DEVICE) {}

void Frame::resizeRoIs(RoIResizer* roiResizer, bool emulatedBatch, int roiSize) {
  if (emulatedBatch) {
    for (auto& cRoI: childRoIs) {
      int w = RoI::toInt(cRoI->paddedLoc.width());
      int h = RoI::toInt(cRoI->paddedLoc.height());
      if (std::max(w, h) <= roiSize) {
        cRoI->setTargetScale(1.0f, RoIResizer::INVALID_LEVEL);
      } else if (w >= h) {
        cRoI->setTargetScale(float(roiSize) / float(w), RoIResizer::INVALID_LEVEL);
      } else { // w < h
        cRoI->setTargetScale(float(roiSize) / float(h), RoIResizer::INVALID_LEVEL);
      }
    }
  } else {
    for (auto& cRoI: childRoIs) {
      if (cRoI->type == OF) {
        auto[scale, level] = roiResizer->getTargetScale(cRoI->id, cRoI->features);
        assert(0.0f < scale && scale <= 1.0f);
        cRoI->setTargetScale(scale, level);
      } else {
        cRoI->setTargetScale(roiResizer->maxScale(), roiResizer->maxLevel());
      }
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

void Frame::mergeRoIs(float maxSize) {
  resetParentRoIs();
  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < parentRoIs.size(); i++) {
      for (j = i + 1; j < parentRoIs.size(); j++) {
        const auto& roi0 = parentRoIs[i];
        const auto& roi1 = parentRoIs[j];
        Rect newRect = Rect::merge(roi0->paddedLoc, roi1->paddedLoc);
        if (std::max(newRect.width(), newRect.height()) > maxSize) {
          continue;
        }
        float newScale = std::max(roi0->getTargetScale(), roi1->getTargetScale());
        float origArea = roi0->getResizedArea() + roi1->getResizedArea();
        if (newRect.area() * newScale * newScale >= origArea) {
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
  testAssignedUniqueRoIID(childRoIs);
  testParentChildrenIDsAndChildIDsSame(childRoIs, parentRoIs);
  testChildRoIsFrameRelation(childRoIs);
  testParentRoIsFrameRelation(parentRoIs);
}

void Frame::resetProbeRoIs() {
  for (auto& cRoI: childRoIs) {
    cRoI->roisForProbing.clear();
    probingRoIs.clear();
  }
}

void Frame::filterPDRoIs(float threshold, bool eatPD) {
  std::vector<RoI*> OFRoIs;
  for (auto& cRoI: childRoIs) {
    if (cRoI->type == OF) {
      OFRoIs.push_back(cRoI.get());
    }
  }

  for (auto it = childRoIs.begin(); it != childRoIs.end();) {
    auto& cRoI = *it;
    if (cRoI->type == PD) {
      if (eatPD) {
        RoI* maxOverlapRoI = nullptr;
        float maxInterSection = 0;
        for (RoI* OFRoI: OFRoIs) {
          float intersection = cRoI->paddedLoc.intersection(OFRoI->origLoc);
          if (intersection > maxInterSection) {
            maxOverlapRoI = OFRoI;
            maxInterSection = intersection;
          }
        }
        if (maxInterSection / cRoI->getPaddedArea() >= threshold) {
          assert(maxOverlapRoI != nullptr);
          maxOverlapRoI->eatPD(cRoI->paddedLoc);
        }
      }

      float totalOFCoverage = 0;
      for (RoI* OFRoI: OFRoIs) {
        totalOFCoverage += cRoI->paddedLoc.intersection(OFRoI->paddedLoc);
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
  testAssignedUniqueRoIID(childRoIs);
}

bool Frame::isReadyToMarry(int mixedFrameIndex) const {
  bool atLeastOneIndexIsSame = false;
  for (const auto& pRoI: parentRoIs) {
    if (!pRoI->isPacked()) {
      continue;
    }
    if (pRoI->getPackedMixedFrameIndex() > mixedFrameIndex) {
      return false;
    }
    atLeastOneIndexIsSame |= (pRoI->getPackedMixedFrameIndex() == mixedFrameIndex);
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

void Frame::resetOFRoIExtraction() {
  childRoIs.erase(std::remove_if(childRoIs.begin(), childRoIs.end(),
                                 [](const auto& cRoI) { return cRoI->type == OF; }),
                  childRoIs.end());
  std::for_each(childRoIs.begin(), childRoIs.end(), [](auto& cRoI) {
    if (cRoI->type == PD) { cRoI->id = UNASSIGNED_ID; }
  });
  useInferenceResultForOF = false;
  extractOFAgain = false;
  isRoIsReady = false;
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

std::string toString(const Stream& frames) {
  MultiStream multiStream;
  for (const auto& frame: frames) {
    multiStream[frame->vid].insert(frame);
  }
  return toString(multiStream);
}

} // namespace rm
