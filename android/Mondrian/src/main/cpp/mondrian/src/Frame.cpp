#include "mondrian/Frame.hpp"

#include <numeric>

#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Log.hpp"

namespace md {

const int Frame::FULL_KEY_OFFSET = 1000000;

Frame::Frame(const int vid, const int frameIndex, const cv::Mat& yuvMat,
             const Frame* prevFrame, const time_us& enqueueTime)
    : vid(vid), frameIndex(frameIndex), scheduleID(-1), yuvMat(yuvMat),
      width_(0), height_(0), prevFrame(prevFrame),
      useInferenceResultForOF(false), reprocessOF(false), enqueueTime(enqueueTime),
      isBoxesReady(false), isROIsReady(false), PDExtractorID(-1), OFExtractorID(-1),
      isLastFrame(false), inferenceFrameSize(0), inferenceDevice(NO_DEVICE) {}

void Frame::prepareRgbMatAndResizedGrayMat(const cv::Size& targetSize) {
  cv::cvtColor(yuvMat, rgbMat, cv::COLOR_YUV2RGB_NV12, 3);
  width_ = rgbMat.cols;
  height_ = rgbMat.rows;
  cv::resize(rgbMat, resizedGrayMat, targetSize, 0, 0, CV_INTER_LINEAR);
  cv::cvtColor(resizedGrayMat, resizedGrayMat, cv::COLOR_RGB2GRAY);
}

void Frame::eatPDROIs(float overlap_thres) {
  std::vector<ROI*> ofROIs;
  for (auto& roi : rois) {
    if (roi->type == OF) {
      ofROIs.push_back(roi.get());
    }
  }

  for (auto it = rois.begin(); it != rois.end();) {
    if ((*it)->type == OF) {
      it++;
      continue;
    }
    auto& pdROI = *it;
    ROI* maxOverlapROI = nullptr;
    float maxIntersection = -1;
    for (ROI* ofROI : ofROIs) {
      float intersection = pdROI->paddedLoc.intersection(ofROI->origLoc);
      if (intersection > maxIntersection) {
        maxOverlapROI = ofROI;
        maxIntersection = intersection;
      }
    }
    if (maxIntersection / pdROI->paddedArea() >= overlap_thres) {
      assert(maxOverlapROI != nullptr);
      maxOverlapROI->eatPD(pdROI->paddedLoc);
      it = rois.erase(it);
    } else {
      it++;
    }
  }
}

void Frame::filterPDROIs(float overlap_thres) {
  std::vector<ROI*> OFROIs;
  for (auto& roi : rois) {
    if (roi->type == OF) {
      OFROIs.push_back(roi.get());
    }
  }

  for (auto it = rois.begin(); it != rois.end();) {
    if ((*it)->type == OF) {
      it++;
      continue;
    }
    auto& pdROI = *it;
    float ofCoverage = 0;
    for (ROI* ofROI : OFROIs) {
      ofCoverage += pdROI->paddedLoc.intersection(ofROI->paddedLoc);
    }
    if (ofCoverage / pdROI->paddedArea() >= overlap_thres) {
      it = rois.erase(it);
    } else {
      it++;
    }
  }
}

void Frame::assignPDROIIDs() {
  for (auto& roi : rois) {
    if (roi->type == PD) {
      assert(roi->id == INVALID_ID);
      roi->id = ROI::getNewIds(1).first;
    } else {
      assert(roi->id != INVALID_ID);
    }
  }
}

void Frame::resizeROIs(ROIResizer* roiResizer) {
  for (auto& roi : rois) {
    if (roi->type == OF) {
      auto [scale, level] = roiResizer->getTargetScale(roi->id, roi->features);
      assert(0.0f < scale && scale <= 1.0f);
      roi->scaleTo(scale, level);
    } else {
      roi->scaleTo(1.0f, ROIResizer::INVALID_LEVEL);
    }
  }
}

void Frame::resetMergedROIs() {
  for (const auto& roi : rois) {
    assert(roi->frame == this);
  }
  for (const auto& mergedROI : mergedROIs) {
    assert(mergedROI->frame() == this);
  }
  for (const auto& mergedROI : mergedROIs) {
    for (const auto& roi : mergedROI->rois()) {
      roi->mergedROI = nullptr;
    }
  }
  mergedROIs.clear();

  for (const auto& roi : rois) {
    std::unique_ptr<MergedROI> mergedROI(new MergedROI({roi.get()}, roi->targetScale(), roi->type));

    assert(mergedROI->frame() == this);
    for (const auto& r : mergedROI->rois()) {
      assert(r->frame == mergedROI->frame());
    }
    mergedROIs.push_back(std::move(mergedROI));
  }

  for (const auto& mergedROI : mergedROIs) {
    assert(mergedROI->frame() == this);
    for (const auto& r : mergedROI->rois()) {
      assert(r->frame == mergedROI->frame());
    }
  }
}

void Frame::mergeMergedROIs(int maxSize) {
  for (const auto& mergedROI : mergedROIs) {
    assert(mergedROI->frame() == this);
    for (const auto& r : mergedROI->rois()) {
      assert(r->frame == mergedROI->frame());
    }
  }

  std::vector<int> root(mergedROIs.size());
  std::iota(root.begin(), root.end(), 0);

  std::function<int(int)> findRoot = [&root, &findRoot](int i) {
    if (root[i] != i) {
      root[i] = findRoot(root[i]);
    }
    return root[i];
  };

  for (int i = 0; i < mergedROIs.size(); i++) {
    for (int j = i + 1; j < mergedROIs.size(); j++) {
      if (mergedROIs[i]->loc().overlap(mergedROIs[j]->loc())) {
        int ri = findRoot(i);
        int rj = findRoot(j);
        if (ri != rj) {
          root[ri] = rj;
        }
      }
    }
  }

  std::map<int, std::vector<std::unique_ptr<MergedROI>>> groupedMergedROIs;
  for (int i = 0; i < mergedROIs.size(); i++) {
    groupedMergedROIs[findRoot(i)].push_back(std::move(mergedROIs[i]));
  }
  mergedROIs.clear();

  for (auto& [_, aMergedROIsGroup] : groupedMergedROIs) {
    for (const auto& mergedROI : aMergedROIsGroup) {
      assert(mergedROI->frame() == this);
      for (const auto& roi : mergedROI->rois()) {
        assert(roi->frame == mergedROI->frame());
      }
    }
  }

  for (auto& [_, aMergedROIsGroup] : groupedMergedROIs) {
    MergedROI::mergeROIs(aMergedROIsGroup, maxSize);
    mergedROIs.insert(mergedROIs.end(),
                      std::make_move_iterator(aMergedROIsGroup.begin()),
                      std::make_move_iterator(aMergedROIsGroup.end()));
  }
}

void Frame::sortMergedROIs() {
  std::sort(mergedROIs.begin(), mergedROIs.end(),
            [](const std::unique_ptr<MergedROI>& m0, const std::unique_ptr<MergedROI>& m1) {
              return m0->loc().maxWH > m1->loc().maxWH;
            });
}

void Frame::resetProbeROIs() {
  for (auto& roi : rois) {
    roi->roisForProbing.clear();
    probingROIs.clear();
  }
}

IntPairs Frame::boxesIfLast(ROIResizer* roiResizer,
                            ExecutionType executionType,
                            bool noDownsampling) {
  // TODO: Synchronize simulation with add logics
  IntPairs boxWHs;
  for (const auto& mergedROI : mergedROIs) {
    // TODO: Make below two condition as single value(or function) of condition
    float scale = mergedROI->targetScale();
    if (executionType == MONDRIAN && noDownsampling) {
      scale = 1.0f;
    }
    auto [bw, bh] = mergedROI->borderedMatWH(scale);
    boxWHs.emplace_back(bw, bh);
  }
  for (const auto& roi : rois) {
    if (roi->scaleLevel() == ROIResizer::INVALID_LEVEL) {
      roi->probeScales.clear();
      continue;
    }
    roiResizer->getProbingCandidates(roi.get());
    for (auto scale : roi->probeScales) {
      int bw = MergedROI::borderedLengthOf(roi->paddedLoc.w, scale);
      int bh = MergedROI::borderedLengthOf(roi->paddedLoc.h, scale);
      boxWHs.emplace_back(bw, bh);
    }
  }
  return boxWHs;
}

void Frame::prepareFrameLast(const IntPairs& indices,
                             const IntPairs& locations,
                             ExecutionType executionType,
                             int roiSize,
                             bool noDownsampling) {
  assert(indices.size() == locations.size());
  isLastFrame = true;
  resetProbeROIs();
  int i = 0;
  for (const auto& mergedROI : mergedROIs) {
    if (executionType == MONDRIAN && noDownsampling) {
      mergedROI->setTargetScale(1.0f);
    }
    mergedROI->setPackInfo(locations[i], indices[i].first, executionType, roiSize);
    i++;
  }
  for (const auto& roi : rois) {
    if (roi->scaleLevel() == ROIResizer::INVALID_LEVEL) {
      assert(roi->probeScales.empty());
      continue;
    }
    for (auto probeScale : roi->probeScales) {
      std::unique_ptr<MergedROI> probeROI(new MergedROI({roi.get()}, probeScale, true));
      assert(0.0f < probeScale && probeScale <= 1.0f);
      probeROI->setPackInfo(locations[i], indices[i].first, executionType, roiSize);
      roi->roisForProbing.push_back(probeROI.get());
      probingROIs.push_back(std::move(probeROI));
      i++;
    }
  }
  assert(i == locations.size());
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
    assert(roi->type == PD);
    roi->mergedROI = nullptr;
  });
  useInferenceResultForOF = false;
  reprocessOF = false;
  isROIsReady = false;
}

std::string str(const MultiStream& streams) {
  std::stringstream ss;
  for (const auto& [vid, stream] : streams) {
    ss << "vid" << vid << "=";
    if (stream.empty()) {
      ss << "EMPTY";
    } else {
      Frame* firstFrame = *stream.begin();
      Frame* lastFrame = *stream.rbegin();
      ss << "[" << firstFrame->frameIndex << ", " << lastFrame->frameIndex << "]";
    }
    ss << " ";
  }
  return ss.str();
}

std::string str(const Stream& stream) {
  MultiStream streams;
  for (const auto& frame : stream) {
    streams[frame->vid].insert(frame);
  }
  return str(streams);
}

} // namespace md
