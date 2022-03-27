package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Pair;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.DispatcherConfig;
import hcs.offloading.strm.config.RoIExtractorConfig;
import hcs.offloading.strm.datatypes.BoundingBox;
import hcs.offloading.strm.datatypes.Frame;
import hcs.offloading.strm.datatypes.MixedFrame;
import hcs.offloading.strm.datatypes.RoI;

public class Dispatcher extends Consumer<Frame, Object> {
    private static final String TAG = Dispatcher.class.getName();

    private Frame prevFrame;
    private int mCountMixedFrameInference = 0;
    private boolean closed = false;
    private final int mKey;
    private final boolean OF_ROI;
    private final DispatcherConfig mConfig;
    private final Map<Integer, List<BoundingBox>> mResultsForRoIExtraction = new ConcurrentHashMap<>();
    private final Map<Integer, List<BoundingBox>> mResults = new ConcurrentHashMap<>();

    private final RoIExtractor mRoIExtractor;
    private final ResizeProfile mResizeProfile;
    private final InferenceEngine mInferenceEngine;

    private final PatchMixer mPatchMixer;
    private final PatchReconstructor mPatchReconstructor;

    @RequiresApi(api = Build.VERSION_CODES.P)
    Dispatcher(int key,
               DispatcherConfig config,
               RoIExtractorConfig roIExtractorConfig,
               ResizeProfile resizeProfile,
               RoIPrioritizer roIPrioritizer,
               InferenceEngine inferenceEngine,
               PatchMixer patchMixer,
               PatchReconstructor patchReconstructor) {
        super(TAG, config.MAX_QUEUE_SIZE, null);
        mKey = key;
        OF_ROI = roIExtractorConfig.OF_ROI;
        mConfig = config;
        mRoIExtractor = new RoIExtractor(roIExtractorConfig, roIPrioritizer);
        mResizeProfile = resizeProfile;
        mInferenceEngine = inferenceEngine;
        mPatchMixer = patchMixer;
        mPatchReconstructor = patchReconstructor;
    }

    public void enqueueImage(int frameIndex, Bitmap bitmap) throws InterruptedException {
        Frame currFrame = new Frame(bitmap, mKey, frameIndex);

        //Log.v(TAG, "Prev: " + (prevFrame != null ? prevFrame.frameIndex : -1) + " Curr: " + currFrame.frameIndex);
        if (mCountMixedFrameInference >= mConfig.FULL_INFERENCE_INTERVAL) {
            int key = mInferenceEngine.enqueue(bitmap);
            List<BoundingBox> results = mInferenceEngine.getResults(key);
            mCountMixedFrameInference = 0;
        } else {
            List<BoundingBox> prevResults;
            if (OF_ROI) {
                if (useInferenceResults) {
                    Frame finalCurrFrame = currFrame;
                    prevResults = getResults(prevFrame.frameIndex).stream()
                            .filter(box -> box.confidence > OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD)
                            .map(box -> new BoundingBox(new Rect(
                                    Math.max(0, box.location.left - ROI_PADDING),
                                    Math.max(0, box.location.top - ROI_PADDING),
                                    Math.min(finalCurrFrame.bitmap.getWidth() - 1, box.location.right + ROI_PADDING),
                                    Math.min(finalCurrFrame.bitmap.getHeight() - 1, box.location.bottom + ROI_PADDING)
                            ), box.confidence, box.labelName))
                            .collect(Collectors.toList());
                } else {
                    prevResults = opticalFlowRoIs.stream()
                            .map(roi -> new BoundingBox(roi.location, 1f, roi.labelName))
                            .collect(Collectors.toList());
                }
                opticalFlowRoIs = createRoIWithInferenceResult(prevFrame, currFrame, prevResults);
                rois.addAll(opticalFlowRoIs);
            }

            Frame frame = mRoIExtractor.process(null);
            List<RoI> rois =
            if (PD_ROI) {
                List<RoI> pixelDiffRoIs = createRoIsFromDiff(prevFrame, currFrame);
                rois.addAll(pixelDiffRoIs);
            }
            if (MERGE_ROI) {
                mergeSingleFrameRoIs(rois);
            }

            rois = rois.stream()
                    .sorted((r0, r1) -> Integer.compare(
                            r1.location.width() * r1.location.height(),
                            r0.location.width() * r0.location.height()))
                    .collect(Collectors.toList());

            rois = resizeRoIs(rois);
            InferenceRequest request = mCallback.tryMixingAndGetInferenceRequest(currFrame, rois, PACKING);
            if (request == null) {
                useInferenceResults = false;
            } else {
                mCountMixedFrameInference++;
                mCallback.enqueueInferenceRequest(request);
                // If prevFrame.frameIndex == mLastQueriedIndex,
                // it means that rois from single frame filled the mixed frame.
                // In that case, we have to continue processing.
                if (prevFrame.frameIndex != mLastQueriedIndex) {
                    updateFrame = false;
                }
            }
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public boolean onProcessEnd(Frame frame) {  // RoIExtraction End
        List<RoI> resizedRoIs = frame.getRoIs().stream()
                .map(roi -> roi.resize(mResizeProfile.getScale(
                        roi.labelName, roi.location.width(), roi.location.height(), roi.minOriginLength)))
                .collect(Collectors.toList());
        frame.setRoIs(resizedRoIs);
        MixedFrame mixedFrame = mPatchMixer.tryPackFrameRoIs(frame);
        if (mixedFrame != null) {
            mMixedFrameCount++;
            int key = mInferenceEngine.enqueue(mixedFrame.bitmap);
            mixedFrame.setResults(mInferenceEngine.getResults(key));
            mPatchReconstructor.enqueue(mixedFrame);
            return false;
        } else {
            return true;
        }
    }

    public void updateResults(List<Frame> frames) {
        synchronized (mResults) {

        }
    }

    public List<BoundingBox> getResults(int frameIndex) throws InterruptedException {
        synchronized (mResults) {
            while (!closed && !mResults.containsKey(frameIndex)) {
                mResults.wait();
            }
            if (closed) {
                return null;
            } else {
                return mResults.get(frameIndex);
            }
        }
    }

    @Override
    public Object process(Frame item) {
        return null;
    }

    public void close() {
        synchronized (mResults) {
            closed = true;
            mRoIExtractor.notifyAll();
        }
        mRoIExtractor.close();
    }
}
