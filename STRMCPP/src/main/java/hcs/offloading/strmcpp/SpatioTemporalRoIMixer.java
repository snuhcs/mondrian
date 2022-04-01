package hcs.offloading.strmcpp;

import org.opencv.core.Mat;

import java.util.List;

public class SpatioTemporalRoIMixer {

    private final long strmHandle;

    public SpatioTemporalRoIMixer() {
        strmHandle = createSpatioTemporalRoIMixer();
    }

    public void enqueueImage(String key, int frameIndex, Mat mat) {
        enqueueImage(strmHandle, key, frameIndex, mat.getNativeObjAddr());
    }

    public List<BoundingBox> getResults(String key, int frameIndex) {
        return getResults(strmHandle, key, frameIndex);
    }

    public void addSource(String key) {
        addSource(strmHandle, key);
    }

    public void removeSource(String key) {
        removeSource(strmHandle, key);
    }

    public void close() {
        close(strmHandle);
    }

    private native long createSpatioTemporalRoIMixer();

    private native void enqueueImage(long strmHandle, String key, int frameIndex, long matAddr);

    private native List<BoundingBox> getResults(long strmHandle, String key, int frameIndex);

    private native void addSource(long strmHandle, String key);

    private native void removeSource(long strmHandle, String key);

    private native void close(long strmHandle);
}
