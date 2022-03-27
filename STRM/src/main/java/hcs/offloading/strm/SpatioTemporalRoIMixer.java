package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.STRMConfig;
import hcs.offloading.strm.config.DispatcherConfig;
import hcs.offloading.strm.config.RoIExtractorConfig;
import hcs.offloading.strm.datatypes.BoundingBox;
import hcs.offloading.strm.datatypes.Frame;

public class SpatioTemporalRoIMixer {
    private static final String TAG = SpatioTemporalRoIMixer.class.getName();

    private final AtomicBoolean isClosed = new AtomicBoolean(false);

    private final ResizeProfile mResizeProfile;
    private final RoIPrioritizer mRoIPrioritizer;
    private final InferenceEngine mInferenceEngine;

    private final PatchMixer mPatchMixer;
    private final PatchReconstructor mPatchReconstructor;
    private final Map<Integer, Dispatcher> mDispatchers = new ConcurrentHashMap<>();

    private final DispatcherConfig mDispatcherConfig;
    private final RoIExtractorConfig mRoIExtractorConfig;

    @RequiresApi(api = Build.VERSION_CODES.P)
    public SpatioTemporalRoIMixer(STRMConfig config,
                                  ResizeProfile resizeProfile,
                                  RoIPrioritizer roIPrioritizer,
                                  InferenceEngine inferenceEngine) {
        mResizeProfile = resizeProfile;
        mRoIPrioritizer = roIPrioritizer;
        mInferenceEngine = inferenceEngine;

        mPatchMixer = new PatchMixer(config.patchMixerConfig);
        mPatchReconstructor = new PatchReconstructor(config.patchReconstructorConfig, mInferenceEngine,
                mixedFrame -> mixedFrame.packedFrames.stream()
                        .collect(Collectors.groupingBy(f -> f.key)).entrySet()
                        .forEach(kv -> {
                            Dispatcher dispatcher = mDispatchers.get(kv.getKey());
                            if (dispatcher != null) {
                                Frame lastFrame = kv.getValue().stream()
                                        .max(Comparator.comparingInt(f0 -> f0.frameIndex)).get();
                                try {
                                    dispatcher.setPrevResults(lastFrame.getResults(), false);
                                } catch (InterruptedException e) {
                                    Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
                                }
                            }
                        }));

        mDispatcherConfig = config.dispatcherConfig;
        mRoIExtractorConfig = config.roIExtractorConfig;
    }

    public void enqueueImage(int key, int frameIndex, Bitmap bitmap) throws InterruptedException {
        if (isClosed.get()) {
            return;
        }
        Dispatcher dispatcher = mDispatchers.get(key);
        if (dispatcher != null) {
            dispatcher.enqueue(new Frame(bitmap, key, frameIndex));
        }
    }

    public List<BoundingBox> getResults(int key, int frameIndex) throws InterruptedException {
        if (isClosed.get()) {
            return null;
        }
        Dispatcher dispatcher = mDispatchers.get(key);
        if (dispatcher != null) {
            return dispatcher.getResults(frameIndex);
        }
        return null;
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    public void addSource(int key) {
        if (isClosed.get()) {
            return;
        }
        if (!mDispatchers.containsKey(key)) {
            mDispatchers.put(key, new Dispatcher(
                    mDispatcherConfig, mRoIExtractorConfig,
                    mResizeProfile, mRoIPrioritizer, mInferenceEngine,
                    mPatchMixer, mPatchReconstructor));
        }
    }

    public void removeSource(int key) {
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
        for (int key : mDispatchers.keySet()) {
            Dispatcher dispatcher = mDispatchers.remove(key);
            if (dispatcher != null) {
                dispatcher.close();
            }
        }
        mInferenceEngine.close();
        mPatchReconstructor.close();
    }
}
