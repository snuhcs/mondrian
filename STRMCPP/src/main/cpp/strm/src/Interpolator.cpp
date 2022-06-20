#include "strm/Interpolator.hpp"

namespace rm {

std::set<idType> Interpolator::interpolate(std::map<std::string, SortedFrames>& frames) {
  std::set<idType> droppedIDs;
  for (auto it : frames) {
    std::set<idType> roIIds = getRoIIds(it.second);
    for (auto id : roIIds) {
      std::vector<RoI*> childRoIs = getRoIStream(it.second, id);
      std::vector<int> validIndices = findValidRoIs(childRoIs);
      if (validIndices.empty()) {
        droppedIDs.insert(id);
        continue;
      }
      extrapolateLeft(childRoIs, validIndices.at(0));
      for (int i = 0; i < validIndices.size() - 1; i++) {
        int leftIdx = validIndices.at(i);
        int rightIdx = validIndices.at(i + 1);
        if (rightIdx - leftIdx == 1) continue;
        interpolateBetween(childRoIs, leftIdx, rightIdx);
      }
      extrapolateRight(childRoIs, validIndices.at(validIndices.size() - 1));
    }
  }
  return droppedIDs;
}

std::set<idType> Interpolator::getRoIIds(SortedFrames& frames) {
  std::set<idType> childIDs;
  for (const Frame* frame : frames) {
    for (const RoI& cRoI : frame->childRoIs) {
      childIDs.insert(cRoI.id);
    }
  }
  return childIDs;
}

std::vector<RoI*> Interpolator::getRoIStream(SortedFrames& frames, idType roIId) {
  std::vector<RoI*> childRoIStream;
  for (Frame* frame : frames) {
    for (RoI& cRoI : frame->childRoIs) {
      if (cRoI.id == roIId) {
        childRoIStream.push_back(&cRoI);
      }
    }
  }
  return childRoIStream;
}

std::vector<int> Interpolator::findValidRoIs(std::vector<RoI*>& childRoIs) {
  std::vector<int> indices;
  for (int i = 0; i < childRoIs.size(); i++) {
    if (childRoIs[i]->box != nullptr) {
      assert(childRoIs[i]->box->id != UNASSIGNED_ID);
      indices.push_back(i);
    }
  }
  return indices;
}

void Interpolator::extrapolateLeft(std::vector<RoI*> childRoIs, int idx) {
  RoI* prevRoI = childRoIs.at(idx);
  assert(prevRoI->box != nullptr);
  assert(0 <= prevRoI->box->label);
  assert(0 <= prevRoI->label);
  std::pair<int, int> prevCenter = prevRoI->box->location.center();
  for (int current = idx - 1; current >= 0; current--) {
    RoI* currRoI = childRoIs.at(current);
    std::pair<int, int> shift = prevRoI->features.shift;
    std::pair<int, int> newCenter = std::make_pair(prevCenter.first - shift.first,
                                                   prevCenter.second - shift.second);
    BoundingBox* prevBox = prevRoI->box;
    assert(prevBox->id == prevRoI->id);
    addBoxWithPrevInfo(currRoI, prevBox, newCenter);

    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

void Interpolator::extrapolateRight(std::vector<RoI*> childRoIs, int idx) {
  RoI* prevRoI = childRoIs.at(idx);
  assert(prevRoI->box != nullptr);
  assert(0 <= prevRoI->box->label);
  assert(0 <= prevRoI->label);
  std::pair<int, int> prevCenter = prevRoI->box->location.center();
  for (int current = idx + 1; current < childRoIs.size(); current++) {
    RoI* currRoI = childRoIs.at(current);
    std::pair<int, int> shift = currRoI->features.shift;
    std::pair<int, int> newCenter = std::make_pair(prevCenter.first + shift.first,
                                                   prevCenter.second + shift.second);
    BoundingBox* prevBox = prevRoI->box;
    assert(prevBox->id == prevRoI->id);
    addBoxWithPrevInfo(currRoI, prevBox, newCenter);

    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

void Interpolator::interpolateBetween(std::vector<RoI*> childRoIs, int leftIdx, int rightIdx) {
  std::pair<int, int> totalShift = sumMotionVectors(childRoIs, leftIdx, rightIdx);
  std::pair<int, int> bbxShift = getBbxShift(childRoIs, leftIdx, rightIdx);

  RoI* prevRoI = childRoIs.at(leftIdx);
  assert(prevRoI->box != nullptr);
  assert(0 <= prevRoI->box->label);
  assert(0 <= prevRoI->label);
  std::pair<int, int> prevCenter = prevRoI->box->location.center();
  for (int current = leftIdx + 1; current < rightIdx; current++) {
    RoI* currRoI = childRoIs.at(current);
    std::pair<int, int> shift = currRoI->features.shift;
    std::pair<int, int> newCenter = std::make_pair(
        prevCenter.first + shift.first * bbxShift.first / totalShift.first,
        prevCenter.second + shift.second * bbxShift.second / totalShift.second);
    BoundingBox* prevBox = prevRoI->box;
    assert(prevBox->id == prevRoI->id);
    addBoxWithPrevInfo(currRoI, prevBox, newCenter);

    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

std::pair<int, int> Interpolator::sumMotionVectors(std::vector<RoI*> childRoIs,
                                                   int start, int end) {
  int xShift = 0, yShift = 0;
  for (int i = start + 1; i <= end; i++) {
    xShift += childRoIs.at(i)->features.shift.first;
    yShift += childRoIs.at(i)->features.shift.second;
  }
  return std::make_pair(xShift, yShift);
}

std::pair<int, int> Interpolator::getBbxShift(std::vector<RoI*> childRoIs, int start, int end) {
  BoundingBox* bbx1 = childRoIs.at(start)->box;
  std::pair<int, int> c1 = bbx1->location.center();
  BoundingBox* bbx2 = childRoIs.at(end)->box;
  std::pair<int, int> c2 = bbx2->location.center();
  return std::make_pair(c2.first - c1.first, c2.second - c2.first);
}

void Interpolator::addBoxWithPrevInfo(RoI* currRoI, const BoundingBox* prevBox,
                                      const std::pair<int, int>& newCenter) {
  int newWidth = prevBox->location.width();
  int newHeight = prevBox->location.height();
  Rect newBox(newCenter, newWidth, newHeight);
  currRoI->frame->boxes.emplace_back(
      new BoundingBox(prevBox->id, newBox, prevBox->confidence, prevBox->label));

  BoundingBox* box = currRoI->frame->boxes.back().get();
  assert(box->id == prevBox->id);
  assert(box->id == currRoI->id);
  box->srcRoI = currRoI;
  currRoI->box = box;
  currRoI->label = box->label;
}

} // namespace rm
