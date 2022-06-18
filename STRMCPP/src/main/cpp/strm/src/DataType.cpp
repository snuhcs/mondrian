#include "strm/DataType.hpp"

namespace rm {

const idType UNASSIGNED_ID = 0;

const std::pair<int, int> RoI::NOT_PACKED{-1, -1};

bool Frame::isAllRoIPacked() const {
  return std::all_of(rois.begin(), rois.end(), [](const RoI& roi) { return roi.isPacked(); });
}

bool Frame::isAllRoIPrepared() const {
  return std::all_of(rois.begin(), rois.end(), [](const RoI& roi) { return roi.isBoxReady; });
}

bool Frame::readyForOFExtraction() const {
  if (prevFrame->useInferenceResultForOF) {
    return prevFrame->isBoxesReady;
  }
  return true;
}

} // namespace rm
