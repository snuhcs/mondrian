package hcs.offloading.strm;

import android.graphics.Rect;
import android.util.Log;
import android.util.Pair;

import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.DispatcherConfig;
import hcs.offloading.strm.config.RoIExtractorConfig;
import hcs.offloading.strm.datatypes.BoundingBox;
import hcs.offloading.strm.datatypes.Frame;

public class Dispatcher extends Consumer<Frame> {
    private static final String TAG = Dispatcher.class.getName();

    private final int mLastPackedFrameIndex = -1;
    private int mCountMixedFrameInference = Integer.MAX_VALUE;
    private Frame mPrevFrame;
    private List<BoundingBox> mPrevResults;

    private final DispatcherConfig mConfig;
    private final Map<Integer, Frame> mFrames = new HashMap<>();

    private final RoIExtractor mRoIExtractor;
    private final RoIPrioritizer mRoIPrioritizer;
    private final ResizeProfile mResizeProfile;
    private final InferenceEngine mInferenceEngine;

    private final PatchMixer mPatchMixer;
    private final PatchReconstructor mPatchReconstructor;

    Dispatcher(DispatcherConfig config,
               RoIExtractorConfig roIExtractorConfig,
               ResizeProfile resizeProfile,
               RoIPrioritizer roIPrioritizer,
               InferenceEngine inferenceEngine,
               PatchMixer patchMixer,
               PatchReconstructor patchReconstructor) {
        super(TAG, config.MAX_QUEUE_SIZE, null);
        mConfig = config;
        mRoIExtractor = new RoIExtractor(roIExtractorConfig);
        mRoIPrioritizer = roIPrioritizer;
        mResizeProfile = resizeProfile;
        mInferenceEngine = inferenceEngine;
        mPatchMixer = patchMixer;
        mPatchReconstructor = patchReconstructor;
    }

    @Override
    public void process(Frame currFrame) throws InterruptedException {
        assert mPrevFrame == null || mPrevFrame.frameIndex + 1 == currFrame.frameIndex;
        /* Cases
         * 1. Full frame inference
         * 2. Mixed frame inference
         *   2.1. If only single frame packed into mixed frame, use inference result of the packed frame.
         *   2.2. Else if multiple frames packed into mixed frame, exclude the last packed frame
         *        and re-pack the frame into next mixed frame.
         */
        synchronized (mFrames) {
            mFrames.put(currFrame.frameIndex, currFrame);
            mFrames.notifyAll();
        }
//        Log.v(TAG, "Start Process " + currFrame.key + ", " + currFrame.frameIndex);
        if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
            mCountMixedFrameInference = 0;
//            Log.v(TAG, "Start Full    " + currFrame.key + ", " + currFrame.frameIndex);
            List<BoundingBox> results = mInferenceEngine.getResults(mInferenceEngine.enqueue(currFrame.bitmap.copy(currFrame.bitmap.getConfig(), false), true));
//            Log.v(TAG, "End   Full    " + currFrame.key + ", " + currFrame.frameIndex);
            currFrame.setResults(results);
            setPrevResults(results, false);
        } else {
//            Log.v(TAG, "getPrevResults");
            List<BoundingBox> prevResults = getPrevResults();
//            Log.v(TAG, "Start RoIExtraction " + currFrame.key + ", " + currFrame.frameIndex + ", " + prevResults);
            mRoIExtractor.process(new Pair<>(new Pair<>(mPrevFrame, currFrame), prevResults));
//            Log.v(TAG, "End   RoIExtraction " + currFrame.key + ", " + currFrame.frameIndex);
            currFrame.sortRoIs(Comparator.comparingInt(mRoIPrioritizer::priority));
            currFrame.getRoIs().forEach(roi -> roi.resize(
                    mResizeProfile.getScale(roi.labelName, roi.location.width(), roi.location.height(), roi.minOriginLength)));
            int lastEnqueuedFrameIndex = mPatchMixer.tryPackAndEnqueueMixedFrame(currFrame);
//            Log.v(TAG, "MixedFrame    " + currFrame.key + ", " + currFrame.frameIndex + ", " + lastEnqueuedFrameIndex);
            if (lastEnqueuedFrameIndex != -1) {
                mCountMixedFrameInference++;
                if (lastEnqueuedFrameIndex == currFrame.frameIndex) { // 2.1.
//                    Log.v(TAG, "NoAgainFrame  " + currFrame.key + ", " + currFrame.frameIndex);
                } else if (lastEnqueuedFrameIndex == currFrame.frameIndex - 1) { // 2.2.
//                    Log.v(TAG, "AgainFrame    " + currFrame.key + ", " + currFrame.frameIndex);
                    process(currFrame);
                } else {
                    Log.e(TAG, "Wrong last index of mixed frame");
                }
            } else {
                setPrevResults(currFrame.getOpticalFlowRoIs().stream()
                        .map(roi -> new BoundingBox(roi.location, 1f, roi.labelName))
                        .collect(Collectors.toList()), true);
            }
        }
        mPrevFrame = currFrame;
    }

    void setPrevResults(List<BoundingBox> results, boolean isRoI) {
        synchronized (this) {
            if (isRoI) { // Optical Flow RoIs
                mPrevResults = results;
            } else { // Inference Results
                mPrevResults = results.stream()
                        .map(box -> new BoundingBox(new Rect(
                                box.location.left - mConfig.ROI_PADDING,
                                box.location.top - mConfig.ROI_PADDING,
                                box.location.right + mConfig.ROI_PADDING,
                                box.location.bottom + mConfig.ROI_PADDING),
                                box.confidence, box.labelName))
                        .collect(Collectors.toList());
            }
            notifyAll();
        }
    }

    private List<BoundingBox> getPrevResults() throws InterruptedException {
        if (!mRoIExtractor.useOpticalFlowRoIs()) {
            return null;
        }
//        Log.v(TAG, "getPrevResults: " + mPrevFrame.frameIndex + ", " + mPrevResults);
        synchronized (this) {
            while (mPrevResults == null) {
                wait();
            }
            List<BoundingBox> prevResults = mPrevResults;
            mPrevResults = null;
            return prevResults;
        }
    }

    public Frame getResults(int frameIndex) throws InterruptedException {
        Frame frame;
        synchronized (mFrames) {
            frame = mFrames.remove(frameIndex);
//            Log.v(TAG, "getResults() " + frameIndex + ", " + frame);
            while (!isClosed() && frame == null) {
                mFrames.wait();
                frame = mFrames.remove(frameIndex);
//                Log.v(TAG, "getResults() " + frameIndex + ", " + frame);
            }
//            Log.v(TAG, "getResults() Loop Out " + frameIndex + ", " + frame);
        }
        if (frame == null) {
            return null;
        } else {
//            Log.v(TAG, "frame.waitForResults() Start " + frame.key + ", " + frame.frameIndex);
            frame.waitForResults();
//            Log.v(TAG, "frame.waitForResults() End   " + frame.key + ", " + frame.frameIndex);
            return frame;
        }
    }

    @Override
    public void close() {
        super.close();
        synchronized (mFrames) {
            mFrames.notifyAll();
        }
    }
}
