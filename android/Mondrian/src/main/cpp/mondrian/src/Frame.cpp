#include "mondrian/Frame.hpp"

#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Log.hpp"

namespace md {

const int Frame::FULL_KEY_OFFSET = 1000000;

Frame::Frame(const int vid, const int frameIndex, const cv::Mat& yuvMat,
             const Frame* prevFrame, const time_us& enqueueTime)
    : vid(vid), frameIndex(frameIndex), scheduleID(-1), yuvMat(yuvMat),
      width(yuvMat.cols), height(yuvMat.rows), prevFrame(prevFrame),
      useInferenceResultForOF(false), extractOFAgain(false), enqueueTime(enqueueTime),
      isBoxesReady(false), isROIsReady(false), PDExtractorID(-1), OFExtractorID(-1),
      isLastFrame(false), inferenceFrameSize(0), inferenceDevice(NO_DEVICE) {}

void Frame::prepareRgbMatAndResizedGrayMat(const cv::Size& targetSize) {
  cv::cvtColor(yuvMat, rgbMat, cv::COLOR_YUV2RGB_NV12, 3);
  cv::resize(rgbMat, resizedGrayMat, targetSize, 0, 0, CV_INTER_LINEAR);
  cv::cvtColor(resizedGrayMat, resizedGrayMat, cv::COLOR_RGB2GRAY);
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

void Frame::resizeROIs(ROIResizer* roiResizer, ExecutionType executionType, int roiSize) {
  if (executionType == EMULATED_BATCH) {
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

void Frame::generateMergedROIs(int maxSize, bool merge) {
  mergedROIs.reserve(rois.size());
  for (const auto& roi: rois) {
    std::unique_ptr<MergedROI> mergedROI(new MergedROI({roi.get()}, roi->targetScale(), roi->type));
    roi->mergedROI = mergedROI.get();
    mergedROIs.push_back(std::move(mergedROI));
  }
  if (!merge) return;

  while (true) {
    int i, j;
    bool updated = false;
    std::unique_ptr<MergedROI> merged;
    for (i = 0; i < mergedROIs.size(); i++) {
      for (j = i + 1; j < mergedROIs.size(); j++) {
        const auto& mi = mergedROIs[i].get();
        const auto& mj = mergedROIs[j].get();
        merged = MergedROI::merge(mi, mj);
        int bw = MergedROI::borderedLengthOf(merged->loc().w, merged->targetScale());
        int bh = MergedROI::borderedLengthOf(merged->loc().h, merged->targetScale());
        if (std::max(bw, bh) > maxSize) {
          continue; // would be little more conservative for the general case
        }

        int newArea = merged->resizedArea();
        int origArea = mi->resizedArea() + mj->resizedArea();
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
    assert(j > i);
    mergedROIs.push_back(std::move(merged));
    mergedROIs.erase(mergedROIs.begin() + j);
    mergedROIs.erase(mergedROIs.begin() + i);
  }

  std::sort(mergedROIs.begin(), mergedROIs.end(),
            [](const std::unique_ptr<MergedROI>& m0, const std::unique_ptr<MergedROI>& m1) {
              return m0->loc().maxWH > m1->loc().maxWH;
            });

  for (auto& merged: mergedROIs) {
    for (auto& roi: merged->rois()) {
      roi->mergedROI = merged.get();
    }
  }
}

void Frame::resetProbeROIs() {
  for (auto& roi: rois) {
    roi->roisForProbing.clear();
    probingROIs.clear();
  }
}

bool Frame::isReadyToMarry(int packedCanvasIndex) const {
  auto isROIReady = [&packedCanvasIndex](const std::unique_ptr<MergedROI>& mergedROI) {
    return !mergedROI->isPacked() || mergedROI->relativePackedCanvasIndex() <= packedCanvasIndex;
  };
  bool isAllReady = std::all_of(mergedROIs.begin(), mergedROIs.end(), isROIReady)
                    && std::all_of(probingROIs.begin(), probingROIs.end(), isROIReady);
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
