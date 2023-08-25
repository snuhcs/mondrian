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
    std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable) {
  if (executionType_ == ExecutionType::ROI_WISE_INFERENCE) {
    std::vector<std::vector<IntRect>>& freeRectsVec = freeRectsVecTable.at(Device::INVALID);
    std::vector<InferenceInfo>& inferencePlan = inferencePlanTable.at(Device::INVALID);
    for (int i = 0; i < lastFrame->mergedROIs.size(); i++) {
      auto& mergedROI = lastFrame->mergedROIs[i];
      assert(!freeRectsVec[i].empty());
      mergedROI->setPackInfo(inferencePlan[i].device,
                             {0, 0},
                             i,
                             executionType_,
                             roiSize_);
      freeRectsVec[i].clear();
    }
  } else { // MONDRIAN, EMULATED_BATCH
    IntPairs frameBoxWHs = lastFrame->boxesIfLast(
        /*roiResizer=*/roiResizer_,
        /*executionType=*/executionType_,
        /*noDownsampling=*/config_.NO_DOWNSAMPLING_FOR_LAST_FRAME);

    std::vector<std::vector<IntRect>>
        & freeRectsVecForLast = freeRectsVecTable.at(LAST_FRAME_DEVICE);
    auto [indices, locations] = ROIPacker::pack(freeRectsVecForLast, frameBoxWHs);
    ROIPacker::apply(freeRectsVecForLast, frameBoxWHs, indices);
    lastFrame->prepareFrameLast(indices,
                                locations,
                                executionType_,
                                roiSize_,
                                config_.NO_DOWNSAMPLING_FOR_LAST_FRAME);
  }
}

void ROIPacker::processMergedROI(
    MergedROI* mergedROI,
    std::map<Device, std::vector<std::vector<IntRect>>>& freeRectsVecTable,
    std::map<Device, std::vector<InferenceInfo>>& inferencePlanTable) {
  if (executionType_ == ExecutionType::ROI_WISE_INFERENCE) {
    std::vector<std::vector<IntRect>>& freeRectsVec = freeRectsVecTable[Device::INVALID];
    auto it = std::find_if(freeRectsVec.begin(), freeRectsVec.end(),
                           [](const auto& rects) { return !rects.empty(); });
    if (it == freeRectsVec.end()) return;
    int index = (int) std::distance(freeRectsVec.begin(), it);
    Device device = inferencePlanTable[Device::INVALID][index].device;
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

std::map<Device, std::vector<PackedCanvas>> ROIPacker::packCanvases(const int currID,
                                                                    const MultiStream& streams,
                                                                    const std::vector<InferenceInfo>& inferencePlan,
                                                                    const Frame* fullFrameTarget) {
  time_us startTime = NowMicros();

  // 1. Split inferencePlan by device and prepare free rects for each device
  std::map<Device, std::vector<std::vector<IntRect>>> freeRectsVecTable;
  std::map<Device, std::vector<InferenceInfo>> inferencePlanTable;
  for (const auto& info : inferencePlan) {
    if (executionType_ == ExecutionType::ROI_WISE_INFERENCE) {
      // Perform device-agnostic dispatching for ROI-wise inference
      freeRectsVecTable[Device::INVALID].push_back({{0, 0, info.size, info.size}});
      inferencePlanTable[Device::INVALID].push_back(info);
    } else {
      freeRectsVecTable[info.device].push_back({{0, 0, info.size, info.size}});
      inferencePlanTable[info.device].push_back(info);
    }
  }

  // 2. Calculate total number of pixels and total latency for each device's plan
  std::map<Device, int> totalPixelsTable;
  std::map<Device, time_us> totalLatencyTable;
  for (const auto& info : inferencePlan) {
    totalPixelsTable[info.device] += info.size * info.size;
    totalLatencyTable[info.device] += info.latency;
  }
  std::map<Device, float> avgPerPixelLatency;
  for (const auto& [device, totalPixels] : totalPixelsTable) {
    avgPerPixelLatency[device] = (float) totalLatencyTable[device] / (float) totalPixels;
    LOGD("XXX: [Schedule %d] avgPerPixelLatency[%d] = %f",
         currID, device, avgPerPixelLatency[device]);
  }

  // 3. Find Dispatch Priority for mergedROIs in the streams (assuming there won't be overflow in canvas)
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

  // 3.5 Log how many mergedROIs dispatched for each processor
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
  LOGD("XXX: [Schedule %d] # Dispatched MergedROIs: %s", currID, ss.str().c_str());

  // 4. Pack mergedROIs from last frames
  for (const auto& [vid, stream] : streams) {
    Frame* lastFrame = *stream.rbegin();
    if (lastFrame == fullFrameTarget) continue;
    processLastFrame(lastFrame, freeRectsVecTable, inferencePlanTable);
  }
  time_us packLastTime = NowMicros();

  // 5. Pack mergedROIs from remaining frames (other than the last frames)
  auto orderedMergedROIs = ROIPrioritizer::order(streams, config_.TYPE);
  time_us orderOthersTime = NowMicros();
  for (MergedROI* mergedROI : orderedMergedROIs) {
    processMergedROI(mergedROI, freeRectsVecTable, inferencePlanTable);
  }
  time_us packOthersTime = NowMicros();

  // 6. Group MergedROIs by packed canvas index
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

  // 7. Generate packed canvases
  std::map<Device, std::vector<PackedCanvas>> packedCanvasesTable;
  for (auto& [device, groupedMergedROIs] : groupedMergedROIsTable) {
    for (auto& [packedCanvasIndex, mergedROIs] : groupedMergedROIs) {
      assert(packedCanvasIndex < inferencePlanTable[device].size());
      const auto& info = inferencePlanTable[device][packedCanvasIndex];
      assert(device == info.device);
      if (!mergedROIs.empty()) {
        packedCanvasesTable[device].emplace_back(mergedROIs, info.size, device);
      }
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

  LOGD("[Schedule %d] Packing %lu MergedROIs "
       "// total=%lld packLast=%lld orderOthers=%lld packOthers=%lld grouping=%lld generating=%lld release=%lld",
       currID, orderedMergedROIs.size(),
  /*total=*/releaseTime - startTime,
  /*packLast=*/packLastTime - startTime,
  /*orderOthers=*/orderOthersTime - packLastTime,
  /*packOthers=*/packOthersTime - orderOthersTime,
  /*grouping=*/groupingTime - packOthersTime,
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
