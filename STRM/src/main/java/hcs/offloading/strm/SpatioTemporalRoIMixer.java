package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.Config;
import hcs.offloading.strm.config.RoIExtractorConfig;
import hcs.offloading.strm.config.DispatcherConfig;
import hcs.offloading.strm.datatypes.BoundingBox;
import hcs.offloading.strm.datatypes.Frame;

public class SpatioTemporalRoIMixer implements ConsumerCallback<Frame> {
    private final DispatcherConfig mDispatcherConfig;
    private final RoIExtractorConfig mRoIExtractorConfig;
    private final ResizeProfile mResizeProfile;
    private final RoIPrioritizer mRoIPrioritizer;
    private final InferenceEngine mInferenceEngine;

    private final PatchMixer mPatchMixer;
    private final PatchReconstructor mPatchReconstructor;
    private final Map<Integer, Dispatcher> mDispatchers = new ConcurrentHashMap<>();

    @RequiresApi(api = Build.VERSION_CODES.P)
    public SpatioTemporalRoIMixer(Config config,
                                  ResizeProfile resizeProfile,
                                  RoIPrioritizer roIPrioritizer,
                                  InferenceEngine inferenceEngine) {
        mDispatcherConfig = config.dispatcherConfig;
        mRoIExtractorConfig = config.roIExtractorConfig;
        mResizeProfile = resizeProfile;
        mRoIPrioritizer = roIPrioritizer;
        mInferenceEngine = inferenceEngine;

        mPatchMixer = new PatchMixer(config.patchMixerConfig);
        mPatchReconstructor = new PatchReconstructor(config.patchReconstructorConfig, frames -> {
            frames.stream()
                    .collect(Collectors.groupingBy(f -> f.key))
                    .forEach((key, value) -> {
                        Dispatcher dispatcher = mDispatchers.get(key);
                        if (dispatcher != null) {
                            dispatcher.updateResults(value);
                        }
                    });
        });
    }

    public List<BoundingBox> getResults(int key, int frameIndex) throws InterruptedException {
        Dispatcher dispatcher = mDispatchers.get(key);
        if (dispatcher == null) {
            return null;
        } else {
            return dispatcher.getResults(frameIndex);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    public void addSource(int key) throws IllegalArgumentException {
        if (mDispatchers.containsKey(key)) {
            throw new IllegalArgumentException("Add already existing key: " + key);
        } else {
            mDispatchers.put(key, new Dispatcher(
                    key, mDispatcherConfig, mRoIExtractorConfig,
                    mResizeProfile, mRoIPrioritizer, mInferenceEngine, mPatchMixer, mPatchReconstructor));
        }
    }

    public void removeSource(int key) throws IllegalArgumentException {
        Dispatcher dispatcher = mDispatchers.remove(key);
        if (dispatcher == null) {
            throw new IllegalArgumentException("Remove not existing key: " + key);
        } else {
            dispatcher.close();
        }
    }

    public void enqueueImage(int key, int frameIndex, Bitmap bitmap) throws IllegalArgumentException, InterruptedException {
        Dispatcher dispatcher = mDispatchers.get(key);
        if (dispatcher == null) {
            throw new IllegalArgumentException("Enqueued image to wrong key: " + key);
        } else {
            dispatcher.enqueueImage(frameIndex, bitmap);
        }
    }

    @Override
    public void onProcessEnd(Frame result) {

    }
}
