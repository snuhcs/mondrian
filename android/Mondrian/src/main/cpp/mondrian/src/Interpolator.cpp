#include "mondrian/Interpolator.hpp"

#include "mondrian/ROI.hpp"

namespace md {

void Interpolator::interpolate(MultiStream& frames, float thres) {
  for (const auto& [vid, aStreamFrames] : frames) {
    std::set<OID> roiIds = getObjectIDs(aStreamFrames);
    for (auto oid : roiIds) {
      std::vector<ROI*> rois = getROIStream(aStreamFrames, oid);
      std::vector<int> validIndices = findValidROIIndices(rois);
      if (validIndices.empty() ||
          float(validIndices.size()) / float(rois.size()) < thres) {
        continue;
      }
      extrapolateLeft(rois, validIndices.at(0));
      for (int i = 0; i < validIndices.size() - 1; i++) {
        int leftIdx = validIndices.at(i);
        int rightIdx = validIndices.at(i + 1);
        if (rightIdx - leftIdx == 1) continue;
        interpolateBetween(rois, leftIdx, rightIdx);
      }
      extrapolateRight(rois, validIndices.at(validIndices.size() - 1));
    }
  }
}

std::set<OID> Interpolator::getObjectIDs(const Stream& frames) {
  std::set<OID> childIDs;
  for (const Frame* frame : frames) {
    for (const auto& roi : frame->rois) {
      childIDs.insert(roi->oid);
    }
  }
  return childIDs;
}

std::vector<ROI*> Interpolator::getROIStream(const Stream& frames, OID oid) {
  std::vector<ROI*> roiStream;
  for (const Frame* frame : frames) {
    for (const auto& roi : frame->rois) {
      if (roi->oid == oid) {
        roiStream.push_back(roi.get());
      }
    }
  }
  return roiStream;
}

std::vector<int> Interpolator::findValidROIIndices(std::vector<ROI*>& rois) {
  std::vector<int> indices;
  for (int i = 0; i < rois.size(); i++) {
    if (rois[i]->box != nullptr) {
      assert(rois[i]->box->oid != INVALID_OID);
      indices.push_back(i);
    }
  }
  return indices;
}

void Interpolator::extrapolateLeft(std::vector<ROI*> rois, int idx) {
  ROI* prevROI = rois.at(idx);
  assert(prevROI->box != nullptr);
  assert(0 <= prevROI->box->label);
  assert(0 <= prevROI->label);
  std::pair<float, float> prevCenter = prevROI->box->loc.center();
  for (int current = idx - 1; current >= 0; current--) {
    ROI* currROI = rois.at(current);
    std::pair<float, float> shift = prevROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> newCenter = std::make_pair(prevCenter.first - shift.first,
                                                       prevCenter.second - shift.second);
    BoundingBox* prevBox = prevROI->box;
    assert(prevBox->oid == prevROI->oid);
    addBoxWithPrevInfo(currROI, prevBox, newCenter);

    prevROI = currROI;
    prevCenter = newCenter;
  }
}

void Interpolator::extrapolateRight(std::vector<ROI*> rois, int idx) {
  ROI* prevROI = rois.at(idx);
  assert(prevROI->box != nullptr);
  assert(0 <= prevROI->box->label);
  assert(0 <= prevROI->label);
  std::pair<float, float> prevCenter = prevROI->box->loc.center();
  for (int current = idx + 1; current < rois.size(); current++) {
    ROI* currROI = rois.at(current);
    std::pair<float, float> shift = currROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> newCenter = std::make_pair(prevCenter.first + shift.first,
                                                       prevCenter.second + shift.second);
    BoundingBox* prevBox = prevROI->box;
    assert(prevBox->oid == prevROI->oid);
    addBoxWithPrevInfo(currROI, prevBox, newCenter);

    prevROI = currROI;
    prevCenter = newCenter;
  }
}

void Interpolator::interpolateBetween(std::vector<ROI*> rois, int leftIdx, int rightIdx) {
  std::pair<float, float> totalShift = sumMotionVectors(rois, leftIdx, rightIdx);
  std::pair<float, float> boxShift = getBoxShift(rois, leftIdx, rightIdx);

  ROI* prevROI = rois.at(leftIdx);
  assert(prevROI->box != nullptr);
  assert(0 <= prevROI->box->label);
  assert(0 <= prevROI->label);
  std::pair<float, float> prevCenter = prevROI->box->loc.center();
  for (int current = leftIdx + 1; current < rightIdx; current++) {
    ROI* currROI = rois.at(current);
    std::pair<float, float> shift = currROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> newCenter = std::make_pair(
        prevCenter.first + shift.first * (float) boxShift.first / totalShift.first,
        prevCenter.second + shift.second * (float) boxShift.second / totalShift.second);
    BoundingBox* prevBox = prevROI->box;
    assert(prevBox->oid == prevROI->oid);
    addBoxWithPrevInfo(currROI, prevBox, {newCenter.first, newCenter.second});

    prevROI = currROI;
    prevCenter = newCenter;
  }
}

std::pair<float, float> Interpolator::sumMotionVectors(std::vector<ROI*> rois,
                                                       int start, int end) {
  float xShift = 0;
  float yShift = 0;
  for (int i = start + 1; i <= end; i++) {
    xShift += rois.at(i)->features.ofFeatures.shiftAvg.first;
    yShift += rois.at(i)->features.ofFeatures.shiftAvg.second;
  }
  return std::make_pair(xShift, yShift);
}

std::pair<float, float> Interpolator::getBoxShift(std::vector<ROI*> rois, int start, int end) {
  BoundingBox* box1 = rois.at(start)->box;
  std::pair<float, float> c1 = box1->loc.center();
  BoundingBox* box2 = rois.at(end)->box;
  std::pair<float, float> c2 = box2->loc.center();
  return std::make_pair(c2.first - c1.first, c2.second - c2.second);
}

void Interpolator::addBoxWithPrevInfo(ROI* currROI, const BoundingBox* prevBox,
                                      const std::pair<float, float>& newCenter) {
  float newL = newCenter.first - prevBox->loc.w / 2;
  float newT = newCenter.second - prevBox->loc.h / 2;
  float newR = newL + prevBox->loc.w;
  float newB = newT + prevBox->loc.h;
  newL = std::min(float(currROI->frame->width()), std::max(0.0f, newL));
  newT = std::min(float(currROI->frame->height()), std::max(0.0f, newT));
  newR = std::min(float(currROI->frame->width()), std::max(0.0f, newR));
  newB = std::min(float(currROI->frame->height()), std::max(0.0f, newB));
  assert(0 <= newL && 0 <= newT);
  assert(newL <= newR && newT <= newB);
  Rect newBox(newL, newT, newR, newB);
  currROI->frame->boxes.push_back(std::make_unique<BoundingBox>(
      prevBox->oid, newBox, prevBox->confidence, prevBox->label, Origin::INTERPOLATE));

  BoundingBox* box = currROI->frame->boxes.back().get();
  assert(box->oid == prevBox->oid);
  assert(box->oid == currROI->oid);
  box->srcROI = currROI;
  currROI->box = box;
  currROI->label = box->label;
}

} // namespace md
