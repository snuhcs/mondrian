#include "strm/PatchMixer.hpp"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"

namespace rm {

int PatchMixer::mMixedFrameIndex = 0;

std::vector<MixedFrame> PatchMixer::pack(const std::map<std::string, SortedFrames>& frames,
                                         const Frame* fullFrameTarget,
                                         int mixedFrameSize, int numMixedFrames,
                                         bool probing) {
  // Collect RoIs. Later frame RoIs come first.
  std::vector<RoI*> rois;
  const float HIGH_PRIORITY = 1e9;

  // 1. Insert probe RoIs
  int numProbes = 0;
  if (probing) {
    int probeStep = 4;
    int probeRoINum = 1; // total 2 * probeRoINum + 1 number of probeRoIs
    for (auto it : frames) {
      if (it.second.empty() || (fullFrameTarget != nullptr && fullFrameTarget->key == it.first)) {
        continue;
      }
      for (RoI& roi : (*it.second.rbegin())->childRoIs) {
        for (int i = 0; i < 2 * probeRoINum + 1; i++) {
          roi.roisForProbing.emplace_back(nullptr, roi.id, roi.frame, roi.location, roi.type,
                                          roi.label,
                                          roi.features.shift, roi.features.err,
                                          roi.features.diffAreaRatio);
        }
        int probe = -probeStep * probeRoINum;
        for (RoI& probeRoI : roi.roisForProbing) {
          numProbes++;
          probeRoI.targetSize = roi.targetSize + probe;
          probe += probeStep;
          probeRoI.priority = HIGH_PRIORITY;
          rois.push_back(&probeRoI);
        }
        std::sort(roi.roisForProbing.begin(), roi.roisForProbing.end());
      }
    }
  }

  // 3. Set priority & sort rois
  int numParentRoIs = 0;
  std::map<idType, std::vector<RoI*>> roiStreams;
  for (const auto& it : frames) {
    for (Frame* frame : it.second) {
      for (RoI& pRoI : frame->parentRoIs) {
        numParentRoIs++;
        roiStreams[pRoI.id].push_back(&pRoI);
        if (pRoI.prevRoI != nullptr) {
          std::pair<int, int> shiftDiff{pRoI.features.shift.first - pRoI.prevRoI->features.shift.first,
                                        pRoI.features.shift.second - pRoI.prevRoI->features.shift.second};
          pRoI.priority = pRoI.features.err + (float) (shiftDiff.first * shiftDiff.first + shiftDiff.second * shiftDiff.second);
        } else {
          pRoI.priority = HIGH_PRIORITY;
        }
        rois.push_back(&pRoI);
      }
    }
  }

  // insert lastFrames first
  int numLastFrameRoIs = 0;
  SortedFrames lastFrames;
  for (const auto& it : frames) {
    if (it.second.empty() || (fullFrameTarget != nullptr && fullFrameTarget->key == it.first)) {
      continue;
    }
    lastFrames.insert(*it.second.rbegin());
    for (RoI& roi : (*it.second.rbegin())->parentRoIs) {
      numLastFrameRoIs++;
      roi.priority = HIGH_PRIORITY;
    }
  }
  std::sort(rois.begin(), rois.end(), [](const RoI* l, const RoI* r) { return l->priority > r->priority; });

  int numTotalRoIs = (int) rois.size();
  int numPackedRoIs = 0;
  std::vector<std::set<RoI*>> packedRoIs;
  packedRoIs.resize(numMixedFrames);
  time_us mixingStartTime = NowMicros();
  for (int i = 0; i < numMixedFrames; i++) {
    tryPackRoIs(rois, mixedFrameSize);
    for (auto it = rois.begin(); it != rois.end();) {
      if ((*it)->isPacked()) {
        numPackedRoIs++;
        (*it)->packedMixedFrameIndex = i;
        packedRoIs[i].insert(*it);
        it = rois.erase(it);
      } else {
        it++;
      }
    }
  }
  time_us mixingEndTime = NowMicros();

  time_us mixedFrameCreateStartTime = NowMicros();
  std::vector<MixedFrame> mixedFrames;
  mixedFrames.reserve(numMixedFrames);
  for (int i = 0; i < numMixedFrames; i++) {
    if (!packedRoIs[i].empty()) {
      mixedFrames.emplace_back(mMixedFrameIndex++, packedRoIs[i], mixedFrameSize);
    }
  }
  time_us mixedFrameCreateEndTime = NowMicros();

  for (auto& it : frames) {
    for (Frame* frame : it.second) {
      frame->mixingStartTime = mixingStartTime;
      frame->mixingEndTime = mixingEndTime;
      frame->mixedFrameCreateStartTime = mixedFrameCreateStartTime;
      frame->mixedFrameCreateEndTime = mixedFrameCreateEndTime;
    }
  }
  LOGD("PatchMixer::pack(%lu, %d, %d) took %lu and %lu us : %d / %d packed, %d lastFrameRoIs, %d Probes",
       frames.size(), mixedFrameSize, numMixedFrames, mixingEndTime - mixingStartTime, mixedFrameCreateEndTime - mixedFrameCreateStartTime,
       numPackedRoIs, numTotalRoIs, numLastFrameRoIs, numProbes);
  return mixedFrames;
}

void PatchMixer::tryPackRoIs(std::vector<RoI*>& rois, int mixedFrameSize) {
  int num_nodes = mixedFrameSize;
  auto* context = new stbrp_context;
  auto* nodes = new stbrp_node[num_nodes];
  stbrp_init_target(context, mixedFrameSize, mixedFrameSize, nodes, num_nodes);

  int num_rects = (int) rois.size();
  auto* rects = new stbrp_rect[num_rects];

  for (int i = 0; i < num_rects; i++) {
    rects[i].id = (int) rois[i]->id;
    auto wh = rois[i]->getResizedWidthHeight();
    rects[i].w = wh.first;
    rects[i].h = wh.second;
  }

  int is_all_packed = stbrp_pack_rects(context, rects, num_rects);

  for (int i = 0; i < num_rects; i++) {
    if (rects[i].was_packed) {
      rois[i]->packedLocation = std::make_pair(rects[i].x, rects[i].y);
    }
  }

  delete[] rects;
  delete[] nodes;
  delete context;
}

} // namespace rm
