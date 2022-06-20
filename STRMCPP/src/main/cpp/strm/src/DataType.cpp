#include "strm/DataType.hpp"
#include "strm/Log.hpp"

namespace rm {

const idType UNASSIGNED_ID = -1;
const idType MERGED_ROI_ID = -2;

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
    int totalOFCoverage = 0;
    for (RoI* OFRoI : OFRoIs) {
      int intersection = PDRoI->location.intersection(OFRoI->location);
      totalOFCoverage += intersection;
    }
    if ((float) totalOFCoverage / (float) PDRoI->getArea() >= threshold) {
      continue;
    }
    newChildRoIs.push_back(*PDRoI);
  }
  for (RoI* OFRoI : OFRoIs) {
    newChildRoIs.push_back(*OFRoI);
  }
  childRoIs = newChildRoIs;
}

bool Frame::isReadyToMarry(int mixedFrameIndex) const {
  bool atLeastOneIndexIsSame = false;
  for (const auto& pRoI : parentRoIs) {
    if (!pRoI->isPacked()) {
      continue;
    }
    if (pRoI->packedMixedFrameIndex > mixedFrameIndex) {
      return false;
    }
    atLeastOneIndexIsSame |= (pRoI->packedMixedFrameIndex == mixedFrameIndex);
  }
  return atLeastOneIndexIsSame;
}

bool Frame::readyForOFExtraction() const {
  if (prevFrame->useInferenceResultForOF) {
    return prevFrame->isBoxesReady;
  } else {
    return prevFrame->isRoIsReady;
  }
}

} // namespace rm
