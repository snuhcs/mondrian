#include "strm/DataType.hpp"

namespace rm {

const idType UNASSIGNED_ID = 0;

const std::pair<int, int> RoI::NOT_PACKED{-1, -1};

void Frame::filterPDRoIs(float threshold) {
  std::vector<RoI> newChildRoIs;
  std::vector<RoI*> PDRoIs;
  std::vector<RoI*> OFRoIs;
  for (RoI& cRoI : childRoIs) {
    if (cRoI.type == RoI::Type::PD) {
      PDRoIs.push_back(&cRoI);
    } else {
      OFRoIs.push_back(&cRoI);
    }
  }
  for (RoI* PDRoI : PDRoIs) {
    bool foundSimilar = false;
    for (RoI* OFRoI : OFRoIs) {
      int intersection = PDRoI->location.intersection(OFRoI->location);
      if ((float) intersection / (float) PDRoI->getArea() >= threshold) {
        foundSimilar = true;
        break;
      }
    }
    if (!foundSimilar) {
      newChildRoIs.push_back(*PDRoI);
    }
  }
  for (RoI* OFRoI : OFRoIs) {
    newChildRoIs.push_back(*OFRoI);
  }
  childRoIs = newChildRoIs;
}

bool Frame::isAllRoIPrepared() const {
  return std::all_of(parentRoIs.begin(), parentRoIs.end(), [](const RoI& pRoI) { return pRoI.isBoxReady; });
}

bool Frame::readyForOFExtraction() const {
  if (prevFrame->useInferenceResultForOF) {
    return prevFrame->isBoxesReady;
  } else {
    return true;
  }
}

} // namespace rm
