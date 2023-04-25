#include "mondrian/Frame.hpp"

#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Log.hpp"

namespace md {

const int Frame::FULL_KEY_OFFSET = 1000000;

Frame::Frame(const int vid, const int frameIndex, const cv::Mat& rgbMat,
             Frame* prevFrame, const time_us& enqueueTime)
    : vid(vid), frameIndex(frameIndex), scheduleID(-1), rgbMat(rgbMat),
      width(rgbMat.cols), height(rgbMat.rows), prevFrame(prevFrame), nextFrame(nullptr),
      useInferenceResultForOF(false), extractOFAgain(false), enqueueTime(enqueueTime),
      isBoxesReady(false), isROIsReady(false), PDExtractorID(-1), OFExtractorID(-1),
      isLastFrame(false), inferenceFrameSize(0), inferenceDevice(NO_DEVICE) {}

void Frame::resizeROIs(ROIResizer* roiResizer, bool emulatedBatch, int roiSize) {
  if (emulatedBatch) {
    for (auto& roi: rois) {
      float w = roi->paddedLoc.w;
      float h = roi->paddedLoc.h;
      float scale = std::min(1.0f, float(roiSize - 2 * MergedROI::BORDER) / std::max(h, w));
      roi->scaleTo(scale, ROIResizer::INVALID_LEVEL);
      int bw = MergedROI::borderedLengthOf(w, scale);
      int bh = MergedROI::borderedLengthOf(h, scale);
      if (roiSize < std::max(bw, bh)) {
        LOGE("Frame::resizeROIs: roiSize=%3d | %4.2f*%4.2f=%4.2f => %3d | %4.2f*%4.2f=%4.2f => %3d",
             roiSize, w, scale, w * scale, bw, h, scale, h * scale, bh);
        assert(false);
      }
    }
  } else {
    for (auto& roi: rois) {
      if (roi->type == OF) {
        auto[scale, level] = roiResizer->getTargetScale(roi->id, roi->features,
                                                        roi->maxEdgeLength);
        assert(0.0f < scale && scale <= 1.0f);
        roi->scaleTo(scale, level);
      } else {
        if (roi->nextROI != nullptr) {
          roi->scaleTo(roi->nextROI->targetScale(), roi->nextROI->scaleLevel());
        } else {
          roi->scaleTo(1.0f, ROIResizer::INVALID_LEVEL);
        }
      }
    }
  }
}

void Frame::resetProbeROIs() {
  for (auto& roi: rois) {
    roi->roisForProbing.clear();
    probingROIs.clear();
  }
}

void Frame::filterPDROIs(float threshold, bool eatPD) {
  std::vector<ROI*> OFROIs;
  for (auto& roi: rois) {
    if (roi->type == OF) {
      OFROIs.push_back(roi.get());
    }
  }

  for (auto it = rois.begin(); it != rois.end();) {
    auto& roi = *it;
    if (roi->type == PD) {
      if (eatPD) {
        ROI* maxOverlapROI = nullptr;
        float maxInterSection = 0;
        for (ROI* OFROI: OFROIs) {
          float intersection = roi->paddedLoc.intersection(OFROI->origLoc);
          if (intersection > maxInterSection) {
            maxOverlapROI = OFROI;
            maxInterSection = intersection;
          }
        }
        if (maxInterSection / roi->paddedArea() >= threshold) {
          assert(maxOverlapROI != nullptr);
          maxOverlapROI->eatPD(roi->paddedLoc);
          it = rois.erase(it);
        }
      }

      float totalOFCoverage = 0;
      for (ROI* OFROI: OFROIs) {
        totalOFCoverage += roi->paddedLoc.intersection(OFROI->paddedLoc);
      }
      if (totalOFCoverage / roi->paddedArea() >= threshold) {
        it = rois.erase(it);
        continue;
      }
    }
    it++;
  }

  for (auto& roi: rois) {
    if (roi->type == PD) {
      assert(roi->id == INVALID_ID);
      roi->id = ROI::getNewIds(1).first;
    } else {
      assert(roi->id != INVALID_ID);
    }
  }
}

bool Frame::isReadyToMarry(int packedCanvasIndex) const {
  auto isROIPacked = [&packedCanvasIndex](const std::unique_ptr<MergedROI>& mergedROI) {
    return mergedROI->isPacked() && mergedROI->relativePackedCanvasIndex() <= packedCanvasIndex;
  };
  bool isAllReady = std::all_of(mergedROIs.begin(), mergedROIs.end(), isROIPacked)
                    && std::all_of(probingROIs.begin(), probingROIs.end(), isROIPacked);
  bool isAllUnassigned = std::all_of(boxes.begin(), boxes.end(),
                                     [](auto& box) { return box->id == INVALID_ID; });
  bool isAllAssigned = std::all_of(boxes.begin(), boxes.end(),
                                   [](auto& box) { return box->id != INVALID_ID; });
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
  rois.erase(std::remove_if(rois.begin(), rois.end(),
                            [](const auto& roi) { return roi->type == OF; }),
             rois.end());
  std::for_each(rois.begin(), rois.end(), [](auto& roi) {
    if (roi->type == PD) { roi->id = INVALID_ID; }
  });
  useInferenceResultForOF = false;
  extractOFAgain = false;
  isROIsReady = false;
}

std::string str(const MultiStream& frames) {
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

std::string str(const Stream& frames) {
  MultiStream multiStream;
  for (const auto& frame: frames) {
    multiStream[frame->vid].insert(frame);
  }
  return str(multiStream);
}

} // namespace md
