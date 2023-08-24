#include "mondrian/Frame.hpp"

#include <numeric>

#include "mondrian/MergedROI.hpp"
#include "mondrian/ROI.hpp"
#include "mondrian/ROIResizer.hpp"
#include "mondrian/Log.hpp"
#include "mondrian/Utils.hpp"

namespace md {

Frame::Frame(const VID vid, const FID fid, const cv::Mat& yuvMat,
             const Frame* prevFrame, const time_us& enqueueTime)
    : vid(vid),
      fid(fid),
      scheduleID(-1),
      yuvMat(yuvMat),
      width_(0),
      height_(0),
      prevFrame(prevFrame),
      useInferenceResultForOF(false),
      reprocessOF(false),
      enqueueTime(enqueueTime),
      isBoxesReady(false),
      isROIsReady(false),
      numFeaturePoints(-1),
      PDExtractorID(-1),
      OFExtractorID(-1),
      isLastFrame(false),
      inferenceFrameSize(0),
      deviceIfFullFrame(Device::INVALID) {}

cv::Mat Frame::rgbMat() const {
  cv::Mat rgbMat;
  cv::cvtColor(yuvMat, rgbMat, cv::COLOR_YUV2RGB_NV12);
  return rgbMat;
}

void Frame::prepareResizedGrayMat(const cv::Size& targetSize) {
  assert(yuvMat.rows % 3 == 0);
  width_ = yuvMat.cols;
  height_ = yuvMat.rows * 2 / 3;
  cv::cvtColor(yuvMat, resizedGrayMat, cv::COLOR_YUV2GRAY_NV12, 1);
  cv::resize(resizedGrayMat, resizedGrayMat, targetSize, 0, 0, CV_INTER_LINEAR);
}

void Frame::eatPDROIs(float overlap_thres) {
  std::vector<ROI*> ofROIs;
  for (auto& roi : rois) {
    if (roi->type() == ROIType::OF) {
      ofROIs.push_back(roi.get());
    }
  }

  for (auto it = rois.begin(); it != rois.end();) {
    if ((*it)->type() == ROIType::OF) {
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
    if (roi->type() == ROIType::OF) {
      OFROIs.push_back(roi.get());
    }
  }

  for (auto it = rois.begin(); it != rois.end();) {
    if ((*it)->type() == ROIType::OF) {
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
    if (roi->type() == ROIType::PD) {
      assert(roi->oid == INVALID_OID);
      roi->oid = ROI::getNewOIDs(1).first;
    } else {
      assert(roi->oid != INVALID_OID);
    }
  }
}

void Frame::resizeROIs(ROIResizer* roiResizer) {
  for (auto& roi : rois) {
    if (roi->type() == ROIType::OF) {
      auto scaleLevelTable = roiResizer->getTargetScale(roi->oid,
                                                       roi->features,
                                                       roi->paddedArea());

      for (auto& it : scaleLevelTable) {
        Device device = it.first;
        float scale = it.second.first;
        int level = it.second.second;
        assert(0.0f < scale && scale <= 1.0f);
        roi->scaleTo(scale, level, device);
      }
    } else {
      for (Device device : roiResizer->getDevices()) {
        roi->scaleTo(1.0f, ROIResizer::INVALID_LEVEL, device);
      }
    }
  }
}

bool Frame::testROIsIntegrity() const {
  bool ret = true;
  for (const auto& roi : rois) {
    //assert(roi->frame == this);
    ret &= roi->frame == this;
  }

  for (const auto& mergedROI : mergedROIs) {
    //assert(mergedROI->frame() == this);
    ret &= mergedROI->frame() == this;
    for (const auto& r : mergedROI->rois()) {
      //assert(r->frame == mergedROI->frame());
      ret &= r->frame == mergedROI->frame();
    }
  }
  return ret;
}

void Frame::resetMergedROIs() {
  assert(testROIsIntegrity());
  for (const auto& mergedROI : mergedROIs) {
    for (const auto& roi : mergedROI->rois()) {
      roi->mergedROI = nullptr;
    }
  }
  mergedROIs.clear();

  for (const auto& roi : rois) {
    std::unique_ptr<MergedROI> mergedROI(new MergedROI({roi.get()}, roi->targetScaleTable(), false));
    mergedROIs.push_back(std::move(mergedROI));
  }
  assert(testROIsIntegrity());
}

void Frame::mergeMergedROIs(int maxSize) {
  assert(testROIsIntegrity());

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
    for (Device device : Devices) {
      roi->roisForProbingTable[device].clear();
      probingROIsTable[device].clear();
    }
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
    if (executionType == ExecutionType::MONDRIAN && noDownsampling) {
      scale = 1.0f;
    }
    auto [bw, bh] = mergedROI->borderedMatWH(scale);
    boxWHs.emplace_back(bw, bh);
  }
  for (const auto& roi : rois) {
    if (roi->scaleLevel() == ROIResizer::INVALID_LEVEL) {
      for (Device device : Devices) {
        roi->probeScalesTable[device].clear();
      }
      continue;
    }
    roi->probeScalesTable = roiResizer->getProbingCandidatesTable(roi->targetScaleTable(),
                                                                  roi->scaleLevel(),
                                                                  roi->paddedArea());
    for (auto scale : roi->probeScalesTable[Device::GPU]) {
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
    if (executionType == ExecutionType::MONDRIAN && noDownsampling) {
      mergedROI->setTargetScale(1.0f);
    }
    mergedROI->setPackInfo(locations[i], indices[i].first, executionType, roiSize);
    i++;
  }
  for (const auto& roi : rois) {
    if (roi->scaleLevel() == ROIResizer::INVALID_LEVEL) {
      assert(roi->probeScalesTable[Device::GPU].empty());
      continue;
    }

    for (auto it = roi->probeScalesTable.begin(); it != roi->probeScalesTable.end(); it++) {
      Device device = it->first;
      auto& probeScales = it->second;
      for (auto probeScale : probeScales) {
        assert(0.0f < probeScale && probeScale <= 1.0f);
        std::map<Device, float> probeScaleTable;
        probeScaleTable[device] = probeScale;
        std::unique_ptr<MergedROI> probeROI(new MergedROI({roi.get()}, probeScaleTable, true));
        probeROI->setPackInfo(locations[i], indices[i].first, executionType, roiSize);
        roi->roisForProbingTable[it->first].push_back(probeROI.get());
        probingROIsTable[it->first].push_back(std::move(probeROI));
        i++;
      }
    }
  }
  //assert(i == locations.size());
}

bool Frame::isReadyToMarry(int packedCanvasIndex) const {
  auto isROIReady = [&packedCanvasIndex](const std::unique_ptr<MergedROI>& mergedROI) {
    return !mergedROI->isPacked() || mergedROI->packedCanvasIndex() <= packedCanvasIndex;
  };
  bool isAllReady = std::all_of(mergedROIs.begin(), mergedROIs.end(), isROIReady)
      && std::all_of(probingROIsTable.at(Device::GPU).begin(), probingROIsTable.at(Device::GPU).end(), isROIReady);
  bool isAllUnassigned = std::all_of(boxes.begin(), boxes.end(),
                                     [](auto& box) { return box->oid == INVALID_OID; });
  bool isAllAssigned = std::all_of(boxes.begin(), boxes.end(),
                                   [](auto& box) { return box->oid != INVALID_OID; });
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
                            [](const auto& roi) { return roi->type() == ROIType::OF; }),
             rois.end());
  std::for_each(rois.begin(), rois.end(), [](auto& roi) {
    assert(roi->type() == ROIType::PD);
    roi->mergedROI = nullptr;
  });
  useInferenceResultForOF = false;
  reprocessOF = false;
  isROIsReady = false;
}

std::string Frame::header() {
  std::stringstream ss;
  ss << "vid" << DELIM
     << "fid" << DELIM
     << "numROIs" << DELIM
     << "numMergedROIs" << DELIM
     << "numProbingROIs" << DELIM
     << "numBoxes" << DELIM
     << "numProbingBoxes" << DELIM
     << "scheduleID" << DELIM
     << "PDExtractorID" << DELIM
     << "OFExtractorID" << DELIM
     << "numFeaturePoints" << DELIM
     << "inferenceFrameSize" << DELIM
     << "deviceIfFullFrame" << DELIM
     << "enqueueTime" << DELIM
     << "fullInferenceStartTime" << DELIM
     << "fullInferenceEndTime" << DELIM
     << "pixelDiffROIProcessStartTime" << DELIM
     << "pixelDiffROIProcessEndTime" << DELIM
     << "opticalFlowROIProcessStartTime" << DELIM
     << "opticalFlowROIProcessEndTime" << DELIM
     << "resizeStartTime" << DELIM
     << "resizeEndTime" << DELIM
     << "mergeROIStartTime" << DELIM
     << "mergeROIEndTime" << DELIM
     << "packingStartTime" << DELIM
     << "packingEndTime" << DELIM
     << "scheduledTime" << DELIM
     << "packedInferenceStartTime" << DELIM
     << "packedInferenceEndTime" << DELIM
     << "reconstructStartTime" << DELIM
     << "reconstructEndTime" << DELIM
     << "endTime";
  return ss.str();
}

std::string Frame::str(time_us baseTime) const {
  auto fromBaseTime = [baseTime](time_us time) { return time != 0 ? time - baseTime : 0; };
  std::stringstream ss;
  ss << vid << DELIM
     << fid << DELIM
      << rois.size() << DELIM
      << mergedROIs.size() << DELIM
      << (probingROIsTable.find(Device::GPU) == probingROIsTable.end() ? 0 : probingROIsTable.at(Device::GPU).size()) << DELIM
      << boxes.size() << DELIM
      << (probingBoxesTable.find(Device::GPU) == probingBoxesTable.end() ? 0 : probingBoxesTable.at(Device::GPU).size()) << DELIM
      << scheduleID << DELIM
      << PDExtractorID << DELIM
      << OFExtractorID << DELIM
      << numFeaturePoints << DELIM
      << inferenceFrameSize << DELIM
      << ::md::str(deviceIfFullFrame) << DELIM
     << fromBaseTime(enqueueTime) << DELIM
     << fromBaseTime(fullInferenceStartTime) << DELIM
     << fromBaseTime(fullInferenceEndTime) << DELIM
     << fromBaseTime(pixelDiffROIProcessStartTime) << DELIM
     << fromBaseTime(pixelDiffROIProcessEndTime) << DELIM
     << fromBaseTime(opticalFlowROIProcessStartTime) << DELIM
     << fromBaseTime(opticalFlowROIProcessEndTime) << DELIM
     << fromBaseTime(resizeStartTime) << DELIM
     << fromBaseTime(resizeEndTime) << DELIM
     << fromBaseTime(mergeROIStartTime) << DELIM
     << fromBaseTime(mergeROIEndTime) << DELIM
     << fromBaseTime(packingStartTime) << DELIM
     << fromBaseTime(packingEndTime) << DELIM
     << fromBaseTime(scheduledTime) << DELIM
     << fromBaseTime(packedInferenceStartTime) << DELIM
     << fromBaseTime(packedInferenceEndTime) << DELIM
     << fromBaseTime(reconstructStartTime) << DELIM
     << fromBaseTime(reconstructEndTime) << DELIM
     << fromBaseTime(endTime);
  return ss.str();
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
      ss << "[" << firstFrame->fid << ", " << lastFrame->fid << "]";
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
