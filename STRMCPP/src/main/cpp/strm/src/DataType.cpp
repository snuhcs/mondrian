#include "strm/DataType.hpp"

namespace rm {

const idType UNASSIGNED_ID = 0;

bool Frame::isAllRoIPrepared() const {
  bool prepared = true;
  for (const RoI& roi : rois) {
    prepared &= roi.isDone;
  }
  return prepared;
}

void Frame::updateBoxesToTrackWithInferenceResult() {
  std::transform(boxes.begin(), boxes.end(),
                 std::back_inserter(boxesToTrack),
                 [this](const BoundingBox& box) {
                   return BoundingBox{
                       box.id, Rect(
                           std::max(0, box.location.left - ROI_PADDING),
                           std::max(0, box.location.top - ROI_PADDING),
                           std::min(width, box.location.right + ROI_PADDING),
                           std::min(height, box.location.bottom + ROI_PADDING)),
                       box.confidence, box.labelName};
                 });
  isOFReady = true;
}

void Frame::updateBoxesToTrackWithRoIs() {
  std::transform(origRoIs.begin(), origRoIs.end(), std::back_inserter(boxesToTrack),
                 [](const RoI& roi) {
                   return BoundingBox{roi.id, roi.location, 1, roi.labelName};
                 });
  isOFReady = true;
}

bool Frame::readyForOFExtraction() const {
  return roiExtractionStatus == OF_WAITING && prevFrame->isOFReady;
}

} // namespace rm
