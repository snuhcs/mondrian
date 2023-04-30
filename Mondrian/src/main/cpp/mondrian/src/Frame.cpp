#include "mondrian/Frame.hpp"

#include "mondrian/ROI.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Test.hpp"
#include "mondrian/Log.hpp"

namespace md {

const int Frame::FULL_KEY_OFFSET = 1000000;

Frame::Frame(const int vid, const int frameIndex, const cv::Mat rgbMat,
             Frame* prevFrame, const time_us& enqueueTime)
    : vid(vid), frameIndex(frameIndex), rgbMat(rgbMat),
      width(rgbMat.cols), height(rgbMat.rows), prevFrame(prevFrame), nextFrame(nullptr),
      useInferenceResultForOF(false), extractOFAgain(false), enqueueTime(enqueueTime),
      isBoxesReady(false), isROIsReady(false), PDExtractorID(-1), OFExtractorID(-1),
      isLastFrame(false), inferenceFrameSize(0), inferenceDevice(NO_DEVICE) {}

void Frame::resizeROIs(ROIResizer* roiResizer, bool emulatedBatch, int roiSize) {
  if (emulatedBatch) {
    for (auto& cROI: childROIs) {
      float w = cROI->paddedLoc.width();
      float h = cROI->paddedLoc.height();
      float scale = std::min(1.0f, float(roiSize - 2 * cROI->roiBorder) / std::max(h, w));
      cROI->setTargetScale(scale, ROIResizer::INVALID_LEVEL);
      auto[bw, bh] = cROI->getBorderMatWidthHeight();
      if (roiSize < std::max(bw, bh)) {
        LOGE("Frame::resizeROIs: roiSize=%3d | %4.2f*%4.2f=%4.2f => %3d | %4.2f*%4.2f=%4.2f => %3d",
             roiSize, w, scale, w * scale, bw, h, scale, h * scale, bh);
        assert(false);
      }
    }
  } else {
    for (auto& cROI: childROIs) {
      if (cROI->type == OF) {
        auto[scale, level] = roiResizer->getTargetScale(cROI->id, cROI->features,
                                                        cROI->maxEdgeLength);
        assert(0.0f < scale && scale <= 1.0f);
        cROI->setTargetScale(scale, level);
      } else {
        if (cROI->nextROI != nullptr) {
          cROI->setTargetScale(cROI->nextROI->getTargetScale(), cROI->nextROI->getScaleLevel());
        } else {
          cROI->setTargetScale(1.0f, ROIResizer::INVALID_LEVEL);
        }
      }
    }
  }
}

void Frame::resetParentROIs() {
  parentROIs.clear();
  assert(parentROIs.empty());
  for (auto& cROI: childROIs) {
    assert(cROI->childROIs.empty());
    assert(cROI->roisForProbing.empty());
  }
  for (auto& cROI: childROIs) {
    std::unique_ptr<ROI> pROI = std::make_unique<ROI>(*cROI);
    cROI->parentROI = pROI.get();
    pROI->childROIs.push_back(cROI.get());
    parentROIs.push_back(std::move(pROI));
  }
}

void Frame::mergeROIs(float maxSize) {
  resetParentROIs();
  while (true) {
    bool updated = false;
    int i, j;
    for (i = 0; i < parentROIs.size(); i++) {
      for (j = i + 1; j < parentROIs.size(); j++) {
        const auto& roi0 = parentROIs[i];
        const auto& roi1 = parentROIs[j];
        Rect newRect = Rect::merge(roi0->paddedLoc, roi1->paddedLoc);
        float newScale = std::max(roi0->getTargetScale(), roi1->getTargetScale());

        int nw = ROI::getResizedMatEdgeLength(newRect.width(), newScale);
        int nh = ROI::getResizedMatEdgeLength(newRect.height(), newScale);
        if (std::max(nw + 2*roi0->roiBorder, nh + 2*roi0->roiBorder) > maxSize) {
          // would be little more conservative for the general case
          continue;
        }

        int newArea = ROI::getResizedArea(newRect.width(), newRect.height(), newScale);
        int origArea = roi0->getResizedArea() + roi1->getResizedArea();
        if (newArea >= origArea) {
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
    parentROIs.push_back(std::move(ROI::mergeROIs(parentROIs[i].get(), parentROIs[j].get())));
    // Match child parent
    ROI* mergedROI = parentROIs.back().get();
    mergedROI->childROIs.insert(mergedROI->childROIs.end(),
                                parentROIs[i]->childROIs.begin(), parentROIs[i]->childROIs.end());
    mergedROI->childROIs.insert(mergedROI->childROIs.end(),
                                parentROIs[j]->childROIs.begin(), parentROIs[j]->childROIs.end());
    for (ROI* cROI: mergedROI->childROIs) {
      cROI->parentROI = mergedROI;
    }
    assert(j > i);
    parentROIs.erase(parentROIs.begin() + j);
    parentROIs.erase(parentROIs.begin() + i);
  }
  std::sort(parentROIs.begin(), parentROIs.end(),
            [](const std::unique_ptr<ROI>& l, const std::unique_ptr<ROI>& r) {
              return l->maxEdgeLength > r->maxEdgeLength;
            });
  testAssignedUniqueROIID(childROIs);
  testParentChildrenIDsAndChildIDsSame(childROIs, parentROIs);
  testChildROIsFrameRelation(childROIs);
  testParentROIsFrameRelation(parentROIs);
}

void Frame::resetProbeROIs() {
  for (auto& cROI: childROIs) {
    cROI->roisForProbing.clear();
    probingROIs.clear();
  }
}

void Frame::filterPDROIs(float threshold, bool eatPD) {
  std::vector<ROI*> OFROIs;
  for (auto& cROI: childROIs) {
    if (cROI->type == OF) {
      OFROIs.push_back(cROI.get());
    }
  }

  for (auto it = childROIs.begin(); it != childROIs.end();) {
    auto& cROI = *it;
    if (cROI->type == PD) {
      if (eatPD) {
        ROI* maxOverlapROI = nullptr;
        float maxInterSection = 0;
        for (ROI* OFROI: OFROIs) {
          float intersection = cROI->paddedLoc.intersection(OFROI->origLoc);
          if (intersection > maxInterSection) {
            maxOverlapROI = OFROI;
            maxInterSection = intersection;
          }
        }
        if (maxInterSection / cROI->getPaddedArea() >= threshold) {
          assert(maxOverlapROI != nullptr);
          maxOverlapROI->eatPD(cROI->paddedLoc);
          it = childROIs.erase(it);
        }
      }

      float totalOFCoverage = 0;
      for (ROI* OFROI: OFROIs) {
        totalOFCoverage += cROI->paddedLoc.intersection(OFROI->paddedLoc);
      }
      if (totalOFCoverage / cROI->getPaddedArea() >= threshold) {
        it = childROIs.erase(it);
        continue;
      }
    }
    it++;
  }

  for (auto& cROI: childROIs) {
    if (cROI->type == PD) {
      assert(cROI->id == UNASSIGNED_ID);
      cROI->id = ROI::getNewIds(1).first;
    } else {
      assert(cROI->id != UNASSIGNED_ID);
    }
  }
  testAssignedUniqueROIID(childROIs);
}

bool Frame::isReadyToMarry(int mixedFrameIndex) const {
  auto isROIPacked = [&mixedFrameIndex](const std::unique_ptr<ROI>& roi) {
    return roi->isPacked() && roi->getPackedMixedFrameIndex() <= mixedFrameIndex;
  };
  bool isAllReady = std::all_of(parentROIs.begin(), parentROIs.end(), isROIPacked)
                    && std::all_of(probingROIs.begin(), probingROIs.end(), isROIPacked);
  bool isAllUnassigned = std::all_of(boxes.begin(), boxes.end(),
                                     [](auto& box) { return box->id == UNASSIGNED_ID; });
  bool isAllAssigned = std::all_of(boxes.begin(), boxes.end(),
                                   [](auto& box) { return box->id != UNASSIGNED_ID; });
  assert(isAllUnassigned || isAllAssigned);
  return isAllReady && isAllUnassigned;
}

bool Frame::readyForOFExtraction() const {
  if (prevFrame->useInferenceResultForOF) {
    return prevFrame->isBoxesReady;
  } else {
    return prevFrame->isROIsReady;
  }
}

void Frame::resetOFROIExtraction() {
  childROIs.erase(std::remove_if(childROIs.begin(), childROIs.end(),
                                 [](const auto& cROI) { return cROI->type == OF; }),
                  childROIs.end());
  std::for_each(childROIs.begin(), childROIs.end(), [](auto& cROI) {
    if (cROI->type == PD) { cROI->id = UNASSIGNED_ID; }
  });
  useInferenceResultForOF = false;
  extractOFAgain = false;
  isROIsReady = false;
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

} // namespace md
