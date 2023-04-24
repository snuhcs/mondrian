#include "mondrian/Interpolator.hpp"

#include "mondrian/ROI.hpp"

namespace md {

void Interpolator::interpolate(MultiStream& frames, float threshold) {
  for (const auto&[vid, aStreamFrames]: frames) {
    std::set<idType> roiIds = getROIIds(aStreamFrames);
    for (auto id: roiIds) {
      std::vector<ROI*> childROIs = getROIStream(aStreamFrames, id);
      std::vector<int> validIndices = findValidROIs(childROIs);
      if (validIndices.empty() ||
          float(validIndices.size()) / float(childROIs.size()) < threshold) {
        continue;
      }
      extrapolateLeft(childROIs, validIndices.at(0));
      for (int i = 0; i < validIndices.size() - 1; i++) {
        int leftIdx = validIndices.at(i);
        int rightIdx = validIndices.at(i + 1);
        if (rightIdx - leftIdx == 1) continue;
        interpolateBetween(childROIs, leftIdx, rightIdx);
      }
      extrapolateRight(childROIs, validIndices.at(validIndices.size() - 1));
    }
  }
}

std::set<idType> Interpolator::getROIIds(const Stream& frames) {
  std::set<idType> childIDs;
  for (const Frame* frame : frames) {
    for (const auto& cROI : frame->childROIs) {
      childIDs.insert(cROI->id);
    }
  }
  return childIDs;
}

std::vector<ROI*> Interpolator::getROIStream(const Stream& frames, idType roiId) {
  std::vector<ROI*> childROIStream;
  for (const Frame* frame : frames) {
    for (const auto& cROI : frame->childROIs) {
      if (cROI->id == roiId) {
        childROIStream.push_back(cROI.get());
      }
    }
  }
  return childROIStream;
}

std::vector<int> Interpolator::findValidROIs(std::vector<ROI*>& childROIs) {
  std::vector<int> indices;
  for (int i = 0; i < childROIs.size(); i++) {
    if (childROIs[i]->box != nullptr) {
      assert(childROIs[i]->box->id != UNASSIGNED_ID);
      indices.push_back(i);
    }
  }
  return indices;
}

void Interpolator::extrapolateLeft(std::vector<ROI*> childROIs, int idx) {
  ROI* prevROI = childROIs.at(idx);
  assert(prevROI->box != nullptr);
  assert(0 <= prevROI->box->label);
  assert(0 <= prevROI->label);
  std::pair<float, float> prevCenter = prevROI->box->location.center();
  for (int current = idx - 1; current >= 0; current--) {
    ROI* currROI = childROIs.at(current);
    std::pair<float, float> shift = prevROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> newCenter = std::make_pair(prevCenter.first - shift.first,
                                                       prevCenter.second - shift.second);
    BoundingBox* prevBox = prevROI->box;
    assert(prevBox->id == prevROI->id);
    addBoxWithPrevInfo(currROI, prevBox, newCenter);

    prevROI = currROI;
    prevCenter = newCenter;
  }
}

void Interpolator::extrapolateRight(std::vector<ROI*> childROIs, int idx) {
  ROI* prevROI = childROIs.at(idx);
  assert(prevROI->box != nullptr);
  assert(0 <= prevROI->box->label);
  assert(0 <= prevROI->label);
  std::pair<float, float> prevCenter = prevROI->box->location.center();
  for (int current = idx + 1; current < childROIs.size(); current++) {
    ROI* currROI = childROIs.at(current);
    std::pair<float, float> shift = currROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> newCenter = std::make_pair(prevCenter.first + shift.first,
                                                       prevCenter.second + shift.second);
    BoundingBox* prevBox = prevROI->box;
    assert(prevBox->id == prevROI->id);
    addBoxWithPrevInfo(currROI, prevBox, newCenter);

    prevROI = currROI;
    prevCenter = newCenter;
  }
}

void Interpolator::interpolateBetween(std::vector<ROI*> childROIs, int leftIdx, int rightIdx) {
  std::pair<float, float> totalShift = sumMotionVectors(childROIs, leftIdx, rightIdx);
  std::pair<float, float> bbxShift = getBbxShift(childROIs, leftIdx, rightIdx);

  ROI* prevROI = childROIs.at(leftIdx);
  assert(prevROI->box != nullptr);
  assert(0 <= prevROI->box->label);
  assert(0 <= prevROI->label);
  std::pair<float, float> prevCenter = prevROI->box->location.center();
  for (int current = leftIdx + 1; current < rightIdx; current++) {
    ROI* currROI = childROIs.at(current);
    std::pair<float, float> shift = currROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> newCenter = std::make_pair(
        prevCenter.first + shift.first * (float) bbxShift.first / totalShift.first,
        prevCenter.second + shift.second * (float) bbxShift.second / totalShift.second);
    BoundingBox* prevBox = prevROI->box;
    assert(prevBox->id == prevROI->id);
    addBoxWithPrevInfo(currROI, prevBox, {newCenter.first, newCenter.second});

    prevROI = currROI;
    prevCenter = newCenter;
  }
}

std::pair<float, float> Interpolator::sumMotionVectors(std::vector<ROI*> childROIs,
                                                       int start, int end) {
  float xShift = 0;
  float yShift = 0;
  for (int i = start + 1; i <= end; i++) {
    xShift += childROIs.at(i)->features.ofFeatures.shiftAvg.first;
    yShift += childROIs.at(i)->features.ofFeatures.shiftAvg.second;
  }
  return std::make_pair(xShift, yShift);
}

std::pair<float, float> Interpolator::getBbxShift(std::vector<ROI*> childROIs, int start, int end) {
  BoundingBox* bbx1 = childROIs.at(start)->box;
  std::pair<float, float> c1 = bbx1->location.center();
  BoundingBox* bbx2 = childROIs.at(end)->box;
  std::pair<float, float> c2 = bbx2->location.center();
  return std::make_pair(c2.first - c1.first, c2.second - c2.second);
}

void Interpolator::addBoxWithPrevInfo(ROI* currROI, const BoundingBox* prevBox,
                                      const std::pair<float, float>& newCenter) {
  float newW = prevBox->location.w;
  float newH = prevBox->location.h;
  Rect newBox(newCenter, newW, newH);
  currROI->frame->boxes.push_back(
      std::make_unique<BoundingBox>(prevBox->id, newBox, prevBox->confidence, prevBox->label, O_INTERPOLATE));

  BoundingBox* box = currROI->frame->boxes.back().get();
  assert(box->id == prevBox->id);
  assert(box->id == currROI->id);
  box->srcROI = currROI;
  currROI->box = box;
  currROI->label = box->label;
}

} // namespace md
