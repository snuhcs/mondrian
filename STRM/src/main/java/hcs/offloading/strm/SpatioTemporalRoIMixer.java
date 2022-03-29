package hcs.offloading.strm;

import android.graphics.Bitmap;

import java.util.Comparator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.DispatcherConfig;
import hcs.offloading.strm.config.RoIExtractorConfig;
import hcs.offloading.strm.config.STRMConfig;
import hcs.offloading.strm.datatypes.Frame;
import hcs.offloading.strm.datatypes.MixedFrame;

public class SpatioTemporalRoIMixer {
    private static final String TAG = SpatioTemporalRoIMixer.class.getName();

    private final AtomicBoolean isClosed = new AtomicBoolean(false);

    private final ResizeProfile mResizeProfile;
    private final RoIPrioritizer mRoIPrioritizer;
    private final InferenceEngine mInferenceEngine;

    private final PatchMixer mPatchMixer;
    private final PatchReconstructor mPatchReconstructor;
    private final Map<String, Dispatcher> mDispatchers = new ConcurrentHashMap<>();

    private final DispatcherConfig mDispatcherConfig;
    private final RoIExtractorConfig mRoIExtractorConfig;

    public SpatioTemporalRoIMixer(STRMConfig config,
                                  ResizeProfile resizeProfile,
                                  RoIPrioritizer roIPrioritizer,
                                  InferenceEngine inferenceEngine) {
        mResizeProfile = resizeProfile;
        mRoIPrioritizer = roIPrioritizer;
        mInferenceEngine = inferenceEngine;

        mPatchReconstructor = new PatchReconstructor(
                config.patchReconstructorConfig, mInferenceEngine, mOnPatchReconstructionEnd);
        mPatchMixer = new PatchMixer(
                config.patchMixerConfig, mInferenceEngine, mPatchReconstructor);

        mDispatcherConfig = config.dispatcherConfig;
        mRoIExtractorConfig = config.roIExtractorConfig;
    }

    private final ConsumerCallback<MixedFrame> mOnPatchReconstructionEnd = mixedFrame -> {
        if (isClosed.get()) {
            return;
        }
        mixedFrame.packedFrames.stream()
                .collect(Collectors.groupingBy(f -> f.key))
                .forEach((key, frames) -> {
                    Dispatcher dispatcher = mDispatchers.get(key);
                    if (dispatcher != null) {
                        Frame lastFrame = frames.stream()
                                .max(Comparator.comparingInt(f0 -> f0.frameIndex))
                                .orElseThrow(() -> new ArrayIndexOutOfBoundsException("No frames with given index"));
                        dispatcher.setPrevBoxesForOpticalFlowRoI(lastFrame.getResults(), false);
                    }
                });
    };

    public void enqueueImage(String key, int frameIndex, Bitmap bitmap) throws InterruptedException {
        if (isClosed.get()) {
            return;
        }
        Dispatcher dispatcher = mDispatchers.get(key);
        if (dispatcher != null) {
            dispatcher.enqueue(new Frame(bitmap, key, frameIndex));
        }
    }

    public Frame getResults(String key, int frameIndex) throws InterruptedException {
        if (isClosed.get()) {
            return null;
        }
        Dispatcher dispatcher = mDispatchers.get(key);
        if (dispatcher != null) {
            return dispatcher.getResults(frameIndex);
        }
        return null;
    }

    public void addSource(String key) {
        if (isClosed.get()) {
            return;
        }
        if (!mDispatchers.containsKey(key)) {
            mDispatchers.put(key, new Dispatcher(
                    mDispatcherConfig, mRoIExtractorConfig,
                    mResizeProfile, mRoIPrioritizer, mInferenceEngine,
                    mPatchMixer));
        }
    }

    public void removeSource(String key) {
        if (isClosed.get()) {
            return;
        }
        Dispatcher dispatcher = mDispatchers.remove(key);
        if (dispatcher != null) {
            dispatcher.close();
        }
    }

    public void close() {
        isClosed.set(true);
        for (String key : mDispatchers.keySet()) {
            Dispatcher dispatcher = mDispatchers.remove(key);
            if (dispatcher != null) {
                dispatcher.close();
            }
        }
        mInferenceEngine.close();
        mPatchReconstructor.close();
    }
}
