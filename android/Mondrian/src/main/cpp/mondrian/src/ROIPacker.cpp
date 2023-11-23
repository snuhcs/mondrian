#include "mondrian/ROIPacker.hpp"

#include <numeric>
#include <sstream>

#include "mondrian/InferencePlanner.hpp"
#include "mondrian/PackedCanvas.hpp"
#include "mondrian/ROIPrioritizer.hpp"
#include "mondrian/ROIResizer.hpp"

namespace md {

ROIPacker::ROIPacker(
    const ROIPackerConfig& config,
    const ExecutionType executionType,
    const int roiSize,
    ROIResizer* roiResizer)
    : config_(config),
      executionType_(executionType),
      roiSize_(roiSize),
      roiResizer_(roiResizer) {}

void ROIPacker::processLastFrame(
    Frame* lastFrame,
    std::map<Device, std::vector<std::vector<IntRect>>>& freeRectsVecTable,
    const std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable) {
  if (executionType_ == ExecutionType::ROI_WISE_INFERENCE) {
    std::vector<std::vector<IntRect>>& freeRectsVec = freeRectsVecTable.at(Device::INVALID);
    const std::vector<InferenceInfo>& inferencePlan = inferencePlanTable.at(Device::INVALID);
    for (int i = 0; i < lastFrame->mergedROIs.size(); i++) {
      auto& mergedROI = lastFrame->mergedROIs[i];
      assert(!freeRectsVec[i].empty());
      freeRectsVec[i].clear();
      mergedROI->setPackInfo(inferencePlan[i].device,
                             {0, 0},
                             i,
                             executionType_,
                             roiSize_);
    }
  } else { // MONDRIAN, EMULATED_BATCH
    IntPairs frameBoxWHs = lastFrame->boxesIfLast(
        /*roiResizer=*/roiResizer_,
        /*executionType=*/executionType_);

    std::vector<std::vector<IntRect>>& freeRectsVecForLast =
        freeRectsVecTable.at(LAST_FRAME_DEVICE);
    auto [indices, locations] = ROIPacker::pack(freeRectsVecForLast, frameBoxWHs);
    ROIPacker::apply(freeRectsVecForLast, frameBoxWHs, indices);
    lastFrame->prepareFrameLast(indices,
                                locations,
                                executionType_,
                                roiSize_);
  }
}

void ROIPacker::processMergedROI(
    MergedROI* mergedROI,
    std::map<Device, std::vector<std::vector<IntRect>>>& freeRectsVecTable,
    const std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable) {
  if (executionType_ == ExecutionType::ROI_WISE_INFERENCE) {
    std::vector<std::vector<IntRect>>& freeRectsVec = freeRectsVecTable[Device::INVALID];
    auto it = std::find_if(freeRectsVec.begin(), freeRectsVec.end(),
                           [](const auto& rects) { return !rects.empty(); });
    if (it == freeRectsVec.end()) return;
    int index = (int) std::distance(freeRectsVec.begin(), it);
    freeRectsVec[index].clear();
    Device device = inferencePlanTable.at(Device::INVALID)[index].device;
    mergedROI->setPackInfo(device, {0, 0}, index, executionType_, roiSize_);
  } else {
    for (Device device : mergedROI->devicePriority()) {
      auto [bw, bh] = mergedROI->borderedMatWH(device);
      auto [indices, locations] = ROIPacker::pack(freeRectsVecTable[device], {{bw, bh}});
      if (!indices.empty()) {
        ROIPacker::apply(freeRectsVecTable[device], {{bw, bh}}, indices);
        mergedROI->setPackInfo(device,
                               locations[0],
                               indices[0].first,
                               executionType_,
                               roiSize_);
        break;
      }
    }
  }
}

static std::map<Device, std::vector<InferenceInfo>> inferencePlanTableOf(
    const std::vector<InferenceInfo>& inferencePlan, ExecutionType executionType) {
  std::map<Device, std::vector<InferenceInfo>> inferencePlanTable;
  for (const auto& info : inferencePlan) {
    if (executionType == ExecutionType::ROI_WISE_INFERENCE) {
      // Perform device-agnostic dispatching for ROI-wise inference
      inferencePlanTable[Device::INVALID].push_back(info);
    } else {
      inferencePlanTable[info.device].push_back(info);
    }
  }
  return inferencePlanTable;
}

static std::map<Device, std::vector<std::vector<IntRect>>> freeRectsVecTableOf(
    const std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable) {
  std::map<Device, std::vector<std::vector<IntRect>>> freeRectsVecTable;
  for (const auto& [device, infos] : inferencePlanTable) {
    for (const auto& info : infos) {
      freeRectsVecTable[device].push_back({{0, 0, info.size, info.size}});
    }
  }
  return freeRectsVecTable;
}

void ROIPacker::pack(const int currID,
                     const MultiStream& streams,
                     const std::vector<InferenceInfo>& inferencePlan,
                     const Frame* fullFrameTarget) {
  time_us startTime = NowMicros();

  // Split inferencePlan by device and prepare free rects for each device
  auto inferencePlanTable = inferencePlanTableOf(inferencePlan, executionType_);
  auto freeRectsVecTable = freeRectsVecTableOf(inferencePlanTable);

  // Calculate total number of pixels and total latency for each device's plan
  std::map<Device, int> totalPixelsTable;
  std::map<Device, time_us> totalLatencyTable;
  for (const auto& info : inferencePlan) {
    totalPixelsTable[info.device] += info.size * info.size;
    totalLatencyTable[info.device] += info.latency;
  }
  std::map<Device, float> avgPerPixelLatency;
  for (const auto& [device, totalPixels] : totalPixelsTable) {
    avgPerPixelLatency[device] = (float) totalLatencyTable[device] / (float) totalPixels;
    LOGD("[Schedule %d] avgPerPixelLatency[%s] = %f",
         currID, str(device).c_str(), avgPerPixelLatency[device]);
  }

  // Find Dispatch Priority for mergedROIs in the streams (assuming there won't be overflow in canvas)
  for (const auto& [vid, stream] : streams) {
    for (Frame* frame : stream) {
      if (frame == fullFrameTarget) continue;
      for (auto& mergedROI : frame->mergedROIs) {
        std::vector<std::pair<Device, float>> deviceLatencies;
        for (const auto& [device, perPixelLatency] : avgPerPixelLatency) {
          float latency = perPixelLatency * (float) mergedROI->borderedArea(device);
          deviceLatencies.emplace_back(device, latency);
        }
        std::sort(deviceLatencies.begin(), deviceLatencies.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });
        std::vector<Device> devicePriority;
        std::transform(deviceLatencies.begin(), deviceLatencies.end(),
                       std::back_inserter(devicePriority),
                       [](const auto& deviceLatency) { return deviceLatency.first; });
        mergedROI->setDevicePriority(devicePriority);
      }
    }
  }

  // Log how many mergedROIs dispatched for each processor
  std::map<Device, int> counts;
  for (const auto& [vid, stream] : streams) {
    for (Frame* frame : stream) {
      if (frame == fullFrameTarget) continue;
      for (auto& mergedROI : frame->mergedROIs) {
        counts[mergedROI->devicePriority().front()]++;
      }
    }
  }
  std::stringstream ss;
  for (const auto& [device, count] : counts) {
    ss << str(device) << "=" << count << " ";
  }
  time_us dispatchTime = NowMicros();
  LOGD("[Schedule %d] # Dispatched: %s", currID, ss.str().c_str());

  // Pack mergedROIs from last frames
  for (const auto& [vid, stream] : streams) {
    Frame* lastFrame = *stream.rbegin();
    if (lastFrame == fullFrameTarget) continue;
    processLastFrame(lastFrame, freeRectsVecTable, inferencePlanTable);
  }
  time_us packLastTime = NowMicros();

  // Pack mergedROIs from remaining frames (other than the last frames)
  auto orderedMergedROIs = ROIPrioritizer::order(streams, config_.TYPE);
  time_us orderOthersTime = NowMicros();

  // Pack others
  for (MergedROI* mergedROI : orderedMergedROIs) {
    processMergedROI(mergedROI, freeRectsVecTable, inferencePlanTable);
  }
  time_us packOthersTime = NowMicros();

  LOGD("[Schedule %d] Packing %lu MergedROIs "
       "// total=%lld dispatch=%lld packLast=%lld orderOthers=%lld packOthers=%lld",
       currID, orderedMergedROIs.size(),
  /*total=*/packOthersTime - startTime,
  /*dispatch=*/dispatchTime - startTime,
  /*packLast*/packLastTime - dispatchTime,
  /*orderOthers*/orderOthersTime - packLastTime,
  /*packOthers*/packOthersTime - orderOthersTime);
}

std::map<Device, std::vector<PackedCanvas>> ROIPacker::generatePackedCanvases(
    const int currID,
    const md::MultiStream& streams,
    const std::vector<InferenceInfo>& inferencePlan,
    const md::Frame* fullFrameTarget) {
  time_us startTime = NowMicros();

  // Group MergedROIs by packed canvas index
  std::map<Device, std::map<int, std::set<MergedROI*>>> groupedMergedROIsTable;
  for (const auto& [vid, stream] : streams) {
    for (Frame* frame : stream) {
      if (frame == fullFrameTarget) continue;
      for (auto& mergedROI : frame->mergedROIs) {
        if (!mergedROI->isPacked()) continue;
        const Device& device = mergedROI->targetDevice();
        const int& canvasIndex = mergedROI->packedCanvasIndex();
        groupedMergedROIsTable[device][canvasIndex].insert(mergedROI.get());
      }
      for (const auto& [device, probeROIs] : frame->probingROIsTable) {
        for (const auto& probeROI : probeROIs) {
          if (!probeROI->isPacked()) continue;
          const int& canvasIndex = probeROI->packedCanvasIndex();
          groupedMergedROIsTable[device][canvasIndex].insert(probeROI.get());
        }
      }
    }
  }
  time_us groupingTime = NowMicros();

  // Generate packed canvases
  auto inferencePlanTable = inferencePlanTableOf(inferencePlan, executionType_);
  std::map<Device, std::vector<PackedCanvas>> packedCanvasesTable;
  for (auto& [device, groupedMergedROIs] : groupedMergedROIsTable) {
    for (auto& [packedCanvasIndex, mergedROIs] : groupedMergedROIs) {
      assert(!mergedROIs.empty());
      int packedCanvasSize = executionType_ != ExecutionType::ROI_WISE_INFERENCE
                             ? inferencePlanTable[device][packedCanvasIndex].size
                             : inferencePlanTable[Device::INVALID][packedCanvasIndex].size;
      packedCanvasesTable[device].emplace_back(mergedROIs, packedCanvasSize, device);
    }
  }
  time_us generatingTime = NowMicros();

  // 8. Release Mats
  for (const auto& [vid, frames] : streams) {
    for (Frame* frame : frames) {
      if (frame == fullFrameTarget) continue;
      frame->yuvMat.release();
    }
  }
  time_us releaseTime = NowMicros();

  LOGD("[Schedule %d] Generating Packed Canvases "
       "// total=%lld grouping=%lld generating=%lld release=%lld",
       currID,
  /*total=*/releaseTime - startTime,
  /*grouping=*/groupingTime - startTime,
  /*generating=*/generatingTime - groupingTime,
  /*release=*/releaseTime - generatingTime);
  return packedCanvasesTable;
}

std::pair<IntPairs, IntPairs> ROIPacker::pack(
    const std::vector<std::vector<IntRect>>& freeRectsVec,
    const IntPairs& boxWHs, bool backward) const {
  auto copiedFreeRectsVec = freeRectsVec;
  IntPairs packIndices;
  IntPairs packLocations;
  for (const auto& [w, h] : boxWHs) {
    int pack_i = -1;
    int pack_j = -1;
    for (int _i = 0; _i < copiedFreeRectsVec.size(); _i++) {
      int i = backward
              ? int(copiedFreeRectsVec.size()) - 1 - _i
              : _i;
      if (executionType_ == ExecutionType::EMULATED_BATCH) {
        pack_j = !copiedFreeRectsVec[i].empty() ? 0 : -1;
      } else {
        pack_j = getBestFitFreeRectIndex(copiedFreeRectsVec[i], w, h);
      }
      if (pack_j != -1) {
        pack_i = i;
        break;
      }
    }
    if (pack_i == -1 || pack_j == -1) {
      assert(pack_i == -1 && pack_j == -1);
      assert(packIndices.size() == packLocations.size());
      return {packIndices, packLocations};
    }
    const IntRect& rect = copiedFreeRectsVec[pack_i][pack_j];
    packIndices.emplace_back(pack_i, pack_j);
    packLocations.emplace_back(rect.l, rect.t);
    if (executionType_ == ExecutionType::EMULATED_BATCH) {
      packBox(copiedFreeRectsVec, roiSize_, roiSize_, pack_i, pack_j);
    } else {
      packBox(copiedFreeRectsVec, w, h, pack_i, pack_j);
    }
  }
  assert(packIndices.size() == packLocations.size());
  assert(packIndices.size() <= boxWHs.size());
  return {packIndices, packLocations};
}

void ROIPacker::apply(std::vector<std::vector<IntRect>>& freeRectsVec,
                      const IntPairs& boxWH, const IntPairs& indices) const {
  assert(indices.size() <= boxWH.size());
  for (int i = 0; i < indices.size(); i++) {
    auto [w, h] = boxWH[i];
    auto [pack_i, pack_j] = indices[i];
    if (executionType_ == ExecutionType::EMULATED_BATCH) {
      packBox(freeRectsVec, roiSize_, roiSize_, pack_i, pack_j);
    } else {
      packBox(freeRectsVec, w, h, pack_i, pack_j);
    }
  }
}

int ROIPacker::getBestFitFreeRectIndex(const std::vector<IntRect>& freeRects, int w, int h) {
  int minRemainingArea = INT_MAX / 2;
  int best_index = -1;
  for (int i = 0; i < freeRects.size(); i++) {
    const IntRect& freeRect = freeRects[i];
    if (canFit(w, h, freeRect)) {
      int remainingArea = freeRect.area - w * h;
      if (minRemainingArea > remainingArea) {
        minRemainingArea = remainingArea;
        best_index = i;
      }
    }
  }
  return best_index;
}

void ROIPacker::packBox(std::vector<std::vector<IntRect>>& freeRectsVec,
                        int w, int h, int pack_i, int pack_j) {
  assert(pack_i < freeRectsVec.size() && pack_j < freeRectsVec[pack_i].size());
  IntRect freeRectToPack = freeRectsVec[pack_i][pack_j];
  freeRectsVec[pack_i].erase(freeRectsVec[pack_i].begin() + pack_j);
  auto [rect0, rect1] = splitFreeRect(w, h, freeRectToPack);
  if (rect0.area > 0) {
    freeRectsVec[pack_i].push_back(rect0);
  }
  if (rect1.area > 0) {
    freeRectsVec[pack_i].push_back(rect1);
  }
}

bool ROIPacker::canFit(int w, int h, const IntRect& freeRect) {
  return w <= freeRect.w && h <= freeRect.h;
}

std::pair<IntRect, IntRect> ROIPacker::splitFreeRect(int w, int h, const IntRect& freeRect) {
  if (freeRect.w > freeRect.h) {
    return {IntRect(freeRect.l + w, freeRect.t, freeRect.r, freeRect.b),
            IntRect(freeRect.l, freeRect.t + h, freeRect.l + w, freeRect.b)};
  } else {
    return {IntRect(freeRect.l, freeRect.t + h, freeRect.r, freeRect.b),
            IntRect(freeRect.l + w, freeRect.t, freeRect.r, freeRect.t + h)};
  }
}

} // namespace md
