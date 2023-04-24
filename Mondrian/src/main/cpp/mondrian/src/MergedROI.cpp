#include "mondrian/MergedROI.hpp"

namespace md {

MergedROI::MergedROI(const std::vector<ROI*>& rois, int border, float targetScale, Type type)
    : rois(rois), border(border), targetScale(targetScale), type(type), loc(locOf(rois)) {}

Rect MergedROI::locOf(const std::vector<ROI*>& rois) {
  assert(!rois.empty());
  float newL = rois[0]->paddedLoc.l;
  float newT = rois[0]->paddedLoc.t;
  float newR = rois[0]->paddedLoc.r;
  float newB = rois[0]->paddedLoc.b;
  for (int i = 1; i < rois.size(); i++) {
    newL = std::min(newL, rois[i]->paddedLoc.l);
    newT = std::min(newT, rois[i]->paddedLoc.t);
    newR = std::max(newR, rois[i]->paddedLoc.r);
    newB = std::max(newB, rois[i]->paddedLoc.b);
  }
  return {newL, newT, newR, newB};
}

std::unique_ptr<MergedROI> MergedROI::merge(const MergedROI* m0, const MergedROI* m1) {
  std::vector<ROI*> newROIs;
  newROIs.insert(newROIs.end(), m0->rois.begin(), m0->rois.end());
  newROIs.insert(newROIs.end(), m1->rois.begin(), m1->rois.end());
  assert(m0->border == m1->border);
  int newBorder = m0->border;
  float newScale = std::max(m0->targetScale, m1->targetScale);
  Type newType = (m0->type == OF && m1->type == OF) ? OF : PD;
  return std::make_unique<MergedROI>(newROIs, newBorder, newScale, newType);
}

std::vector<std::unique_ptr<MergedROI>> MergedROI::mergeROIs(
    const std::vector<std::unique_ptr<ROI>>& rois, int maxSize, int border) {
  std::vector<std::unique_ptr<MergedROI>> mergedROIs;
  mergedROIs.reserve(rois.size());
  for (const auto& roi: rois) {
    mergedROIs.emplace_back(new MergedROI({roi.get()}, border, roi->getTargetScale(), roi->type));
  }

  while (true) {
    int i, j;
    bool updated = false;
    std::unique_ptr<MergedROI> merged;
    for (i = 0; i < mergedROIs.size(); i++) {
      for (j = i + 1; j < mergedROIs.size(); j++) {
        const auto& mi = mergedROIs[i].get();
        const auto& mj = mergedROIs[j].get();
        merged = merge(mi, mj);
        int nw = ROI::getResizedMatEdgeLength(merged->locw, merged->targetScale);
        int nh = ROI::getResizedMatEdgeLength(merged->loch, merged->targetScale);
        if (std::max(nw + 2 * mi->border, nh + 2 * mi->border) > maxSize) {
          continue; // would be little more conservative for the general case
        }

        int newArea = ROI::getResizedArea(merged->locw, merged->loch,
                                          merged->targetScale);
        int origArea = mi->rois[0]->getResizedArea() + mj->rois[0]->getResizedArea();
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
              return m0->loc.maxLength() > m1->loc.maxLength();
            });

  for (auto& merged: mergedROIs) {
    for (auto& roi: merged->rois) {
      roi->mergedROI = merged.get();
    }
  }

  return mergedROIs;
}

} // namespace md
