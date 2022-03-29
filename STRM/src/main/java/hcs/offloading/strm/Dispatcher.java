package hcs.offloading.strm;

import static hcs.offloading.strm.PatchMixer.CONTINUE_PACKING;

import android.graphics.Bitmap;
import android.graphics.Rect;
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

    Dispatcher(DispatcherConfig config,
               RoIExtractorConfig roIExtractorConfig,
               ResizeProfile resizeProfile,
               RoIPrioritizer roIPrioritizer,
               InferenceEngine inferenceEngine,
               PatchMixer patchMixer) {
        super(TAG, config.MAX_QUEUE_SIZE, null);
        mConfig = config;
        mRoIExtractor = new RoIExtractor(roIExtractorConfig);
        mRoIPrioritizer = roIPrioritizer;
        mResizeProfile = resizeProfile;
        mInferenceEngine = inferenceEngine;
        mPatchMixer = patchMixer;
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
        if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
            mCountMixedFrameInference = 0;
            Bitmap copiedBitmap = currFrame.bitmap.copy(currFrame.bitmap.getConfig(), false);
            int handle = mInferenceEngine.enqueue(copiedBitmap, true);
            List<BoundingBox> results = mInferenceEngine.getResults(handle);
            currFrame.setResults(results);
            setPrevBoxesForOpticalFlowRoI(results, false);
        } else {
            List<BoundingBox> prevResults = getPrevResults();
            mRoIExtractor.process(new Pair<>(new Pair<>(mPrevFrame, currFrame), prevResults));
            currFrame.sortRoIs(Comparator.comparingInt(mRoIPrioritizer::priority));
            currFrame.getRoIs().forEach(roi -> roi.resize(
                    mResizeProfile.getScale(roi.labelName, roi.location.width(), roi.location.height(), roi.minOriginLength)));
            int lastPackedIndex = mPatchMixer.tryPackAndEnqueueMixedFrame(currFrame);
            if (lastPackedIndex == CONTINUE_PACKING) {
                setPrevBoxesForOpticalFlowRoI(currFrame.getOpticalFlowRoIs().stream()
                        .map(roi -> new BoundingBox(roi.location, 1f, roi.labelName))
                        .collect(Collectors.toList()), true);
            } else {
                mCountMixedFrameInference++;
                boolean isCurrFrameExcludedFromMixedFrame = lastPackedIndex == currFrame.frameIndex - 1;
                if (isCurrFrameExcludedFromMixedFrame) {
                    process(currFrame);
                }
            }
        }
        mPrevFrame = currFrame;
    }

    void setPrevBoxesForOpticalFlowRoI(List<BoundingBox> prevBoxes, boolean isRoI) {
        synchronized (this) {
            if (isRoI) { // Optical Flow RoIs
                mPrevResults = prevBoxes;
            } else { // Inference Results
                mPrevResults = prevBoxes.stream()
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
            while (!isClosed() && frame == null) {
                mFrames.wait();
                frame = mFrames.remove(frameIndex);
            }
        }
        if (frame == null) {
            return null;
        } else {
            frame.waitForResults();
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
