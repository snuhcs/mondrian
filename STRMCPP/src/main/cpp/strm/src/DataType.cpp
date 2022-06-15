#include "strm/DataType.hpp"

namespace rm {

void Frame::updateBoxesToTrackWithInferenceResult() {
  std::transform(boxes.begin(), boxes.end(),
                 std::back_inserter(boxesToTrack),
                 [this](const BoundingBox& box) {
                   return BoundingBox{Rect(
                       std::max(0, box.location.left - ROI_PADDING),
                       std::max(0, box.location.top - ROI_PADDING),
                       std::min(width, box.location.right + ROI_PADDING),
                       std::min(height, box.location.bottom + ROI_PADDING)),
                                      box.confidence, box.labelName};
                 });
  isOFReady = true;
}

void Frame::updateBoxesToTrackWithOFRoIs(const std::vector<RoI>& opticalFlowRoIs) {
  std::transform(opticalFlowRoIs.begin(), opticalFlowRoIs.end(), std::back_inserter(boxesToTrack),
                 [](const RoI& roi) { return BoundingBox{roi.location, 1, roi.labelName}; });
  isOFReady = true;
}

bool Frame::readyForExtraction() const {
  return roiExtractionStatus == PD_WAITING ||
         (roiExtractionStatus == OF_WAITING && prevFrame->isOFReady);
}

} // namespace rm
