#include "PatchReconstructor.hpp"

#include <algorithm>

namespace rm {

void PatchReconstructor::process(MixedFrame item) {

}

void PatchReconstructor::updateMixedFrameInferenceResults(MixedFrame& mixedFrame, int matchPadding, float useIoUThreshold) {
    for (const BoundingBox& box : mixedFrame.getResults()) {
        float maxOverlap = -1f;
        Rect maxBoxPos;
        Frame* maxFrame;
        for (Frame* frame : mixedFrame.packedFrames) {
            for (RoI& roi : frame->getRoIs()) {
                if (roi.isPacked()) {
                    Rect paddedRoIPos(std::max(0, roi.location.left - matchPadding),
                                      std::max(0, roi.location.top - matchPadding),
                                      std::min(roi.frame.mat->cols, roi.location.right + matchPadding),
                                      std::min(roi.frame.mat->rows, roi.location.bottom + matchPadding));
                    Rect movedAndResizedBoxPos(std::max(0, (int) ((float) (box.location.left - roi.packedLocation.first) / roi.getScale()) + roi.location.left),
                                               std::max(0, (int) ((float) (box.location.top - roi.packedLocation.second) / roi.getScale()) + roi.location.top),
                                               std::min(roi.frame.mat->cols, (int) ((float) (box.location.right - roi.packedLocation.first) / roi.getScale()) + roi.location.left),
                                               std::min(roi.frame.mat->rows, (int) ((float) (box.location.bottom - roi.packedLocation.second) / roi.getScale()) + roi.location.top));
                    float intersection = 0;
                    float overlapRatio = 0;
                    if (maxOverlap < overlapRatio) {

                    }
                }
            }
        }
    }
}

void PatchReconstructor::updateRoIInferenceResults(MixedFrame& mixedFrame) {
    for (Frame* frame : mixedFrame.packedFrames) {
        for (RoI& roi : frame->getRoIs()) {
            for (const BoundingBox& box : roi.getResults()) {
                frame->addResult(BoundingBox(
                        Rect(box.location.left + roi.location.left,
                             box.location.top + roi.location.top,
                             box.location.right + roi.location.left,
                             box.location.bottom + roi.location.top),
                             box.confidence, box.labelName));
            }
        }
    }
}

} // namespace rm

