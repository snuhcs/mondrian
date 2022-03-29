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

    private int mCountMixedFrameInference = Integer.MAX_VALUE;
    private Frame mPrevFrame;
    private List<BoundingBox> mPrevResults;

    private final DispatcherConfig mConfig;
    private final Map<Integer, List<BoundingBox>> mResults = new HashMap<>();

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
         *   2.1. If multiple frames from the source stream packed into mixed frame,
         *        PatchMixer will exclude the last packed frame from the packed frames.
         *        We have to re-pack the frame into next mixed frame.
         *   2.2. Else wait for mixed frame result and continue to process next frame.
         */
        if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
            mCountMixedFrameInference = 0;
            int handle = mInferenceEngine.enqueue(currFrame.mat.clone(), true);
            List<BoundingBox> results = mInferenceEngine.getResults(handle);
            currFrame.addResults(results);
            setResults(currFrame);
        } else {
            List<BoundingBox> prevResults = getPrevBoxes();
            mRoIExtractor.process(new Pair<>(new Pair<>(mPrevFrame, currFrame), prevResults));
            currFrame.sortRoIs(Comparator.comparingInt(mRoIPrioritizer::priority));
            currFrame.getRoIs().forEach(roi -> roi.resize(
                    mResizeProfile.getScale(roi.labelName, roi.location.width(), roi.location.height(), roi.minOriginLength)));
            PatchMixer.Status status = mPatchMixer.tryPackAndEnqueueMixedFrame(currFrame);
            if (status == PatchMixer.Status.CONTINUE_PACKING) {
                setPrevBoxesWithRoIs(currFrame.getOpticalFlowRoIs().stream()
                        .map(roi -> new BoundingBox(roi.location, 1f, roi.labelName))
                        .collect(Collectors.toList()));
            } else if (status == PatchMixer.Status.FINISHED) {
                // 2.2. Last packed frame in included
                mCountMixedFrameInference++;
            } else if (status == PatchMixer.Status.FINISHED_AND_PROCESS_LAST_FRAME_AGAIN) {
                // 2.1. Last packed frame is excluded
                mCountMixedFrameInference++;
                process(currFrame);
            } else {
                throw new IllegalArgumentException("Wrong PactchMixer.Status: " + status);
            }
        }
        mPrevFrame = currFrame;
    }

    void setPrevBoxesWithRoIs(List<BoundingBox> prevBoxes) {
        synchronized (this) {
            mPrevResults = prevBoxes;
            notifyAll();
        }
    }

    private List<BoundingBox> getPrevBoxes() throws InterruptedException {
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

    public void setResults(Frame fullFrame) {
        synchronized (this) {
            mPrevResults = fullFrame.getResults().stream()
                    .map(box -> new BoundingBox(new Rect(
                            box.location.left - mConfig.ROI_PADDING,
                            box.location.top - mConfig.ROI_PADDING,
                            box.location.right + mConfig.ROI_PADDING,
                            box.location.bottom + mConfig.ROI_PADDING),
                            box.confidence, box.labelName))
                    .collect(Collectors.toList());
            notifyAll();
        }
        synchronized (mResults) {
            mResults.put(fullFrame.frameIndex, fullFrame.getResults());
            mResults.notifyAll();
        }
    }

    public void setResults(List<Frame> packedFrames) {
        Frame lastFrame = packedFrames.stream()
                .max(Comparator.comparingInt(f0 -> f0.frameIndex))
                .orElseThrow(() -> new ArrayIndexOutOfBoundsException("No frames with given index"));
        synchronized (this) {
            mPrevResults = lastFrame.getResults().stream()
                    .map(box -> new BoundingBox(new Rect(
                            box.location.left - mConfig.ROI_PADDING,
                            box.location.top - mConfig.ROI_PADDING,
                            box.location.right + mConfig.ROI_PADDING,
                            box.location.bottom + mConfig.ROI_PADDING),
                            box.confidence, box.labelName))
                    .collect(Collectors.toList());
            notifyAll();
        }
        synchronized (mResults) {
            for (Frame frame : packedFrames) {
                mResults.put(frame.frameIndex, frame.getResults());
            }
            mResults.notifyAll();
        }
    }

    public List<BoundingBox> getResults(int frameIndex) throws InterruptedException {
        List<BoundingBox> results;
        synchronized (mResults) {
            results = mResults.remove(frameIndex);
            while (!isClosed() && results == null) {
                mResults.wait();
                results = mResults.remove(frameIndex);
            }
        }
        return results;
    }

    @Override
    public void close() {
        super.close();
        synchronized (mResults) {
            mResults.notifyAll();
        }
    }
}
