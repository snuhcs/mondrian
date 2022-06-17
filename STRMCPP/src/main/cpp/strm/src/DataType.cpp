#include "strm/DataType.hpp"

namespace rm {

const idType UNASSIGNED_ID = 0;

const std::pair<int, int> RoI::NOT_PACKED = std::make_pair(-1, -1);

bool Frame::isAllRoIPacked() const {
  return std::all_of(rois.begin(), rois.end(), [](const RoI& roi) { return roi.isPacked(); });
}

bool Frame::isAllRoIPrepared() const {
  return std::all_of(rois.begin(), rois.end(), [](const RoI& roi) { return roi.isBoxReady; });
}

void Frame::updateBoxesToTrackWithInferenceResult() {
  // assert(boxesToTrack.empty());  TODO: uncomment assert
  boxesToTrack.clear();
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
  // assert(boxesToTrack.empty());  TODO: uncomment assert
  boxesToTrack.clear();
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
