package hcs.offloading.strm;

import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
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
import hcs.offloading.strm.datatypes.MixedFrame;
import hcs.offloading.strm.datatypes.RoI;

public class Dispatcher extends Consumer<Frame> {
    private static final String TAG = Dispatcher.class.getName();

    private int mCountMixedFrameInference;
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

    @RequiresApi(api = Build.VERSION_CODES.P)
    Dispatcher(DispatcherConfig config,
               RoIExtractorConfig roIExtractorConfig,
               ResizeProfile resizeProfile,
               RoIPrioritizer roIPrioritizer,
               InferenceEngine inferenceEngine,
               PatchMixer patchMixer,
               PatchReconstructor patchReconstructor) {
        super(TAG, config.MAX_QUEUE_SIZE, null);
        mConfig = config;
        mCountMixedFrameInference = mConfig.FULL_INFERENCE_INTERVAL;
        mRoIExtractor = new RoIExtractor(roIExtractorConfig);
        mRoIPrioritizer = roIPrioritizer;
        mResizeProfile = resizeProfile;
        mInferenceEngine = inferenceEngine;
        mPatchMixer = patchMixer;
        mPatchReconstructor = patchReconstructor;
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
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
        }
        if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
            mCountMixedFrameInference = 0;
            List<BoundingBox> results = mInferenceEngine.getResults(mInferenceEngine.enqueue(currFrame.bitmap));
            currFrame.setResults(results);
            setPrevResults(results, false);
        } else {
            mRoIExtractor.process(new Pair<>(new Pair<>(mPrevFrame, currFrame), getPrevResults()));
            currFrame.sortRoIs(Comparator.comparingInt(mRoIPrioritizer::priority));
            currFrame.getRoIs().forEach(roi -> roi.resize(
                    mResizeProfile.getScale(roi.labelName, roi.location.width(), roi.location.height(), roi.minOriginLength)));
            MixedFrame mixedFrame = mPatchMixer.tryPackFrameRoIs(currFrame);
            if (mixedFrame != null) {
                mCountMixedFrameInference++;
                if (mixedFrame.packedBitmap != null) { // PACKING = true, Mixed Frame inference
                    mixedFrame.setHandle(mInferenceEngine.enqueue(mixedFrame.packedBitmap));
                } else { // PACKING = false, RoI-wise inference
                    for (Frame frame : mixedFrame.packedFrames) {
                        for (RoI roi : frame.getRoIs()) {
                            roi.setHandle(mInferenceEngine.enqueue(roi.getBitmap()));
                        }
                    }
                }
                mPatchReconstructor.enqueue(mixedFrame);
                if (!mixedFrame.packedFrames.contains(currFrame)) { // 2.2.
                    process(currFrame);
                }
            } else {
                setPrevResults(currFrame.getOpticalFlowRoIs().stream()
                        .map(roi -> new BoundingBox(roi.location, 1f, roi.labelName))
                        .collect(Collectors.toList()), true);
            }
        }
        mPrevFrame = currFrame;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
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
        if (mRoIExtractor.useOpticalFlowRoIs()) {
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

    public List<BoundingBox> getResults(int frameIndex) throws InterruptedException {
        synchronized (mFrames) {
            Frame frame = mFrames.remove(frameIndex);
            while (!isClosed() && frame == null) {
                mFrames.wait();
                frame = mFrames.remove(frameIndex);
            }
            if (frame == null) {
                return null;
            } else {
                return frame.getResults();
            }
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
