#include "strm/Interpolator.hpp"

namespace rm {

void Interpolator::interpolate(std::set<Frame*>& frames) {
  std::set<idType> roIIds = getRoIIds(frames);
  for (auto id : roIIds) {
    std::vector<RoI*> rois = getRoIStream(frames, id);
    std::vector<int> validIndices = findValidRoIs(rois);
    if (!validIndices.empty()) {
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

std::set<idType> Interpolator::getRoIIds(std::set<Frame*>& frames) {
  std::set<idType> ids;
  for (const Frame* frame : frames) {
    for (const RoI& roi : frame->origRoIs) {
      ids.insert(roi.id);
    }
  }
  return ids;
}

std::vector<RoI*> Interpolator::getRoIStream(std::set<Frame*>& frames, idType roIId) {
  std::vector<RoI*> rois;
  for (Frame* frame : frames) {
    for (RoI& roi : frame->origRoIs) {
      if (roi.id == roIId) {
        rois.push_back(&roi);
      }
    }
  }
  return rois;
}

std::vector<int> Interpolator::findValidRoIs(std::vector<RoI*>& rois) {
  std::vector<int> indices;
  for (int i = 0; i < rois.size(); i++) {
    if (rois[i]->matchingBox() != nullptr) {
      indices.push_back(i);
    }
  }
  return indices;
}

void Interpolator::extrapolateLeft(std::vector<RoI*> rois, int idx) {
  RoI* prevRoI = rois.at(idx);
  std::pair<int, int> prevCenter = prevRoI->getMatchedBbx()->location.center();
  for (int current = idx - 1; current >= 0; current--) {
    RoI* currRoI = rois.at(current);
    std::pair<int, int> shift = prevRoI->features.shift;
    std::pair<int, int> newCenter = std::make_pair(prevCenter.first - shift.first,
                                                   prevCenter.second - shift.second);
    BoundingBox* prevBox = prevRoI->getMatchedBbx();
    int newWidth = prevBox->location.width();
    int newHeight = prevBox->location.height();
    Rect newBox(newCenter, newWidth, newHeight);
    BoundingBox box(prevRoI->id, newBox, prevBox->confidence, prevRoI->labelName);
    currRoI->boxes.push_back(box);
    currRoI->frame->boxes.push_back(box);
    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

void Interpolator::extrapolateRight(std::vector<RoI*> rois, int idx) {
  RoI* prevRoI = rois.at(idx);
  std::pair<int, int> prevCenter = prevRoI->getMatchedBbx()->location.center();
  for (int current = idx + 1; current < rois.size(); current++) {
    RoI* currRoI = rois.at(current);
    std::pair<int, int> shift = currRoI->features.shift;
    std::pair<int, int> newCenter = std::make_pair(prevCenter.first + shift.first,
                                                   prevCenter.second + shift.second);
    BoundingBox* prevBox = prevRoI->getMatchedBbx();
    int newWidth = prevBox->location.width();
    int newHeight = prevBox->location.height();
    Rect newBox(newCenter, newWidth, newHeight);
    BoundingBox box(prevRoI->id, newBox, prevBox->confidence, prevRoI->labelName);
    currRoI->boxes.push_back(box);
    currRoI->frame->boxes.push_back(box);
    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

void Interpolator::interpolateBetween(std::vector<RoI*> rois, int leftIdx, int rightIdx) {
  std::pair<int, int> totalShift = sumMotionVectors(rois, leftIdx, rightIdx);
  std::pair<int, int> bbxShift = getBbxShift(rois, leftIdx, rightIdx);
  float xRatio = (float) bbxShift.first / (float) totalShift.first;
  float yRatio = (float) bbxShift.second / (float) totalShift.second;

  RoI* prevRoI = rois.at(leftIdx);
  std::pair<int, int> prevCenter = prevRoI->getMatchedBbx()->location.center();
  for (int current = leftIdx + 1; current < rightIdx; current++) {
    RoI* currRoI = rois.at(current);
    std::pair<int, int> shift = currRoI->features.shift;
    std::pair<int, int> newCenter = std::make_pair(
        prevCenter.first + (int) ((float) shift.first * xRatio),
        prevCenter.second + (int) ((float) shift.second * yRatio));
    BoundingBox* prevBox = prevRoI->getMatchedBbx();
    int newWidth = prevBox->location.width();
    int newHeight = prevBox->location.height();
    Rect newBox(newCenter, newWidth, newHeight);
    BoundingBox box(prevRoI->id, newBox, prevBox->confidence, prevRoI->labelName);
    currRoI->boxes.push_back(box);
    currRoI->frame->boxes.push_back(box);
    prevRoI = currRoI;
    prevCenter = newCenter;
  }
}

std::pair<int, int> Interpolator::sumMotionVectors(std::vector<RoI*> rois, int start, int end) {
  int xShift = 0, yShift = 0;
  for (int i = start + 1; i <= end; i++) {
    xShift += rois.at(i)->features.shift.first;
    yShift += rois.at(i)->features.shift.second;
  }
  return std::make_pair(xShift, yShift);
}

std::pair<int, int> Interpolator::getBbxShift(std::vector<RoI*> rois, int start, int end) {
  BoundingBox* bbx1 = rois.at(start)->getMatchedBbx();
  std::pair<int, int> c1 = bbx1->location.center();
  BoundingBox* bbx2 = rois.at(end)->getMatchedBbx();
  std::pair<int, int> c2 = bbx2->location.center();
  return std::make_pair(c2.first - c1.first, c2.second - c2.first);
}


} // namespace rm
