#include "strm/DataType.hpp"
#include "strm/Log.hpp"

namespace rm {

const idType UNASSIGNED_ID = -1;
const idType MERGED_ROI_ID = -2;

const std::pair<int, int> RoI::NOT_PACKED{-1, -1};

void Frame::filterPDRoIs(float threshold) {
  std::vector<RoI*> OFRoIs;
  for (auto& cRoI : childRoIs) {
    if (cRoI->type == RoI::Type::OF) {
      OFRoIs.push_back(cRoI.get());
    }
  }

  for (auto it = childRoIs.begin(); it != childRoIs.end();) {
    auto& cRoI = *it;
    if (cRoI->type == RoI::Type::PD) {
      int totalOFCoverage = 0;
      for (RoI* OFRoI : OFRoIs) {
        int intersection = cRoI->location.intersection(OFRoI->location);
        totalOFCoverage += intersection;
      }
      if ((float) totalOFCoverage / (float) cRoI->getArea() >= threshold) {
        it = childRoIs.erase(it);
        continue;
      }
    }
    it++;
  }
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
