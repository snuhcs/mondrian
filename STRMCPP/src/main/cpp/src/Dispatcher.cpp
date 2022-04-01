#include "Dispatcher.hpp"

#include <cassert>

namespace rm {

void Dispatcher::process(Frame& currFrame) {
    assert(mPrevFrame == nullptr || mPrevFrame->frameIndex + 1 == currFrame.frameIndex);
    /* Cases
     * 1. Full frame inference
     * 2. Mixed frame inference
     *   2.1. If multiple frames from the source stream packed into mixed frame,
     *        PatchMixer will exclude the last packed frame from the packed frames.
     *        We have to re-pack the frame into next mixed frame.
     *   2.2. Else wait for mixed frame result and continue to process next frame.
     */
    mFrames[currFrame.frameIndex] = std::make_unique<Frame>(currFrame);
    if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
        mCountMixedFrameInference = 0;
        int handle = mInferenceEngine->enqueue(currFrame.mat.clone(), true);
        std::vector<BoundingBox> results = mInferenceEngine->getResults(handle);
        currFrame.addResults(results);
        notifyResults(currFrame);
    } else {
        std::vector<BoundingBox> prevResults = getPrevBoxes();
        mRoIExtractor->process(make_pair(std::make_pair(mPrevFrame, currFrame), prevResults));
//        currFrame->sortRoIs(mRoIPrioritizer.priority); TODO: fix sortRoIs with priority function
        std::vector<RoI>* rois = currFrame->getRoIs();
        for (auto& roi : *rois) {
            float scale = mResizeProfile.getScale(roi.labelName, roi.location.width(),
                                                  roi.location.height(), roi.minOriginLength);
            roi.resize(scale);
        }
        PatchMixer::Status status = mPatchMixer.tryPackAndEnqueueMixedFrame(currFrame);
        if (status == PatchMixer::CONTINUE_PACKING) {
            std::vector<RoI>* opticalFlowRoIs = currFrame->getOpticalFlowRoIs();
            std::vector<BoundingBox> roIBbx;
            for (auto& roi : *opticalFlowRoIs) {
                roIBbx.push_back(BoundingBox(roi.location, 1.0, roi.labelName));
            }
            setPrevBoxesWithRoIs(roIBbx);
        } else if (status == PatchMixer::FINISHED) {
            mCountMixedFrameInference++;
        } else if (status == PatchMixer::FINISHED_AND_PROCESS_LAST_FRAME_AGAIN) {
            mCountMixedFrameInference++;
            process(currFrame);
        } else {

        }
    }
    mPrevFrame = currFrame;
}

void Dispatcher::setPrevBoxesWithRoIs(vector<BoundingBox>& prevBoxes) {
    mPrevResults = prevBoxes;
}

std::vector<BoundingBox> Dispatcher::getPrevBoxes() {
    std::vector<BoundingBox> prevResults;
    if (!mRoIExtractor->useOpticalFlowRoIs()) {
        return prevResults;
    }
    prevResults = mPrevResults;
    return prevResults;
}

void Dispatcher::notifyResults(const int frameIndex) {
    vector<BoundingBox> prevResults;
    for (auto& bbx : *(fullFrame.getResults())) {
        int x = bbx.location.x - mConfig.ROI_PADDING;
        int y = bbx.location.y - mConfig.ROI_PADDING;
        int width = bbx.location.width + 2 * mConfig.ROI_PADDING;
        int height = bbx.location.height + 2 * mConfig.ROI_PADDING;
        cv::Rect location = cv::Rect(x, y, width, height);
        prevResults.emplace_back(location, bbx.confidence, bbx.labelName);
    }
    mPrevResults = prevResults;
    mResults.insert(make_pair(fullFrame->frameIndex, *fullFrame->getResults()));
}

void Dispatcher::notifyResults(const std::vector<int>& frameIndices) {
    auto lastFrame = max_element(packedFrames->begin(), packedFrames->end(),
                                 [](const Frame& a, const Frame& b) {
                                     return a.frameIndex < b.frameIndex;
                                 });
    std::vector<BoundingBox> prevResults;
    for (auto& bbx : *(lastFrame->getResults())) {
        int x = bbx.location.x - mConfig.ROI_PADDING;
        int y = bbx.location.y - mConfig.ROI_PADDING;
        int width = bbx.location.width + 2 * mConfig.ROI_PADDING;
        int height = bbx.location.height + 2 * mConfig.ROI_PADDING;
        cv::Rect location = cv::Rect(x, y, width, height);
        prevResults.push_back(BoundingBox(location, bbx.confidence, bbx.labelName));
    }
    mPrevResults = prevResults;
    for (auto& frame : *packedFrames) {
        mResults.insert(make_pair(frame.frameIndex, *frame.getResults()));
    }
}

vector<BoundingBox> Dispatcher::getResults(int frameIndex) {
    vector<BoundingBox> results = mResults.at(frameIndex);
    mResults.erase(frameIndex);
    return results;
}

} // namespace rm
