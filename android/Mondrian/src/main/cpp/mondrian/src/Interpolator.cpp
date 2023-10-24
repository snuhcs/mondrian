#include "mondrian/Interpolator.hpp"

#include "mondrian/ROI.hpp"

namespace md {

void Interpolator::interpolate(MultiStream& frames, int thres) {
  for (const auto& [vid, aStreamFrames] : frames) {
    std::set<OID> roiIds = getObjectIDs(aStreamFrames);
    for (auto oid : roiIds) {
      std::vector<ROI*> rois = getROIStream(aStreamFrames, oid);
      std::vector<int> validIndices = findValidROIIndices(rois);
      if (validIndices.empty() || validIndices.size() < thres) {
        continue;
      }
      extrapolateLeft(rois, validIndices.front());
      for (int i = 0; i < validIndices.size() - 1; i++) {
        int leftIdx = validIndices.at(i);
        int rightIdx = validIndices.at(i + 1);
        if (rightIdx - leftIdx == 1) continue;
        interpolateBetween(rois, leftIdx, rightIdx);
      }
      extrapolateRight(rois, validIndices.back());
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
    if (rois[i]->box() != nullptr) {
      assert(rois[i]->box()->oid != INVALID_OID);
      indices.push_back(i);
    }
  }
  return indices;
}

void Interpolator::extrapolateLeft(std::vector<ROI*> rois, int idx) {
  assert(rois.at(idx)->box() != nullptr);
  for (int i = idx; i > 0; i--) {
    ROI* prevROI = rois.at(i - 1);
    ROI* currROI = rois.at(i);
    std::pair<float, float> currCenter = currROI->box()->loc.center();
    std::pair<float, float> currShift = currROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> prevCenter = {currCenter.first - currShift.first,
                                          currCenter.second - currShift.second};
    BoundingBox* currBox = currROI->box();
    assert(currBox->oid == currROI->oid);
    addBoxWithRefBox(prevROI, currBox, prevCenter);
  }
}

void Interpolator::extrapolateRight(std::vector<ROI*> rois, int idx) {
  assert(rois.at(idx)->box() != nullptr);
  for (int i = idx + 1; i < rois.size(); i++) {
    ROI* prevROI = rois.at(i - 1);
    ROI* currROI = rois.at(i);
    std::pair<float, float> prevCenter = prevROI->box()->loc.center();
    std::pair<float, float> currShift = currROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> currCenter = {prevCenter.first + currShift.first,
                                          prevCenter.second + currShift.second};
    BoundingBox* prevBox = prevROI->box();
    assert(prevBox->oid == prevROI->oid);
    addBoxWithRefBox(currROI, prevBox, currCenter);
  }
}

void Interpolator::interpolateBetween(std::vector<ROI*> rois, int leftIdx, int rightIdx) {
  assert(rois.at(leftIdx)->box() != nullptr);
  assert(rois.at(rightIdx)->box() != nullptr);
  std::pair<float, float> totalShift = sumMotionVectors(rois, leftIdx, rightIdx);
  std::pair<float, float> boxShift = getBoxShift(rois, leftIdx, rightIdx);
  bool isCorrectMatch = (totalShift.first > 0) == (boxShift.first > 0)
      && (totalShift.second > 0) == (boxShift.second > 0)
      && std::abs(totalShift.first) * 5 > std::abs(boxShift.first)
      && std::abs(totalShift.second) * 5 > std::abs(boxShift.second);
  std::pair<float, float> roiBoxMoveRatio =
      isCorrectMatch ? std::make_pair(boxShift.first / totalShift.first,
                                      boxShift.second / totalShift.second)
                     : std::make_pair(1.0f, 1.0f);
  for (int i = leftIdx + 1; i < rightIdx; i++) {
    ROI* prevROI = rois.at(i - 1);
    ROI* currROI = rois.at(i);
    std::pair<float, float> prevCenter = prevROI->box()->loc.center();
    std::pair<float, float> currShift = currROI->features.ofFeatures.shiftAvg;
    std::pair<float, float> currCenter = {
        prevCenter.first + currShift.first * roiBoxMoveRatio.first,
        prevCenter.second + currShift.second * roiBoxMoveRatio.second};
    BoundingBox* prevBox = prevROI->box();
    assert(prevBox->oid == prevROI->oid);
    addBoxWithRefBox(currROI, prevBox, currCenter);
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
  return {xShift, yShift};
}

std::pair<float, float> Interpolator::getBoxShift(std::vector<ROI*> rois, int start, int end) {
  BoundingBox* box1 = rois.at(start)->box();
  std::pair<float, float> c1 = box1->loc.center();
  BoundingBox* box2 = rois.at(end)->box();
  std::pair<float, float> c2 = box2->loc.center();
  return {c2.first - c1.first, c2.second - c1.second};
}

void Interpolator::addBoxWithRefBox(ROI* currROI, const BoundingBox* refBox,
                                    const std::pair<float, float>& newCenter) {
  float newL = newCenter.first - refBox->loc.w / 2;
  float newT = newCenter.second - refBox->loc.h / 2;
  float newR = newL + refBox->loc.w;
  float newB = newT + refBox->loc.h;
  newL = std::min(float(currROI->frame->width()), std::max(0.0f, newL));
  newT = std::min(float(currROI->frame->height()), std::max(0.0f, newT));
  newR = std::min(float(currROI->frame->width()), std::max(0.0f, newR));
  newB = std::min(float(currROI->frame->height()), std::max(0.0f, newB));
  assert(0 <= newL && 0 <= newT);
  assert(newL <= newR && newT <= newB);
  Rect newBox(newL, newT, newR, newB);
  currROI->frame->boxes.emplace_back(new BoundingBox(
      refBox->oid, -1, newBox, refBox->confidence, refBox->label, BoxOrigin::INTERPOLATE));

  BoundingBox* box = currROI->frame->boxes.back().get();
  assert(box->oid == refBox->oid);
  assert(box->oid == currROI->oid);
  box->setSrcROI(currROI);
  currROI->setBox(box);
  currROI->setLabel(box->label);
}

} // namespace md
