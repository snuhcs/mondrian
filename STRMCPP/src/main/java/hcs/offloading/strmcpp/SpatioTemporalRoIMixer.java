package hcs.offloading.strmcpp;

import org.opencv.core.Mat;

import java.util.List;

public class SpatioTemporalRoIMixer {
    static {
        System.loadLibrary("strmcpp");
        System.loadLibrary("opencv_core");
        System.loadLibrary("opencv_dnn");
        System.loadLibrary("opencv_video");
        System.loadLibrary("opencv_imgcodecs");
        System.loadLibrary("opencv_imgproc");
        System.loadLibrary("MNN");
        System.loadLibrary("MNN_CL");
        System.loadLibrary("MNN_Express");
    }

    private final long handle;

    public SpatioTemporalRoIMixer(int frameSize, int fullFrameSize) {
        handle = createSpatioTemporalRoIMixer(frameSize, fullFrameSize);
    }

    public int enqueueImage(String key, Mat mat) {
        return enqueueImage(handle, key, mat.getNativeObjAddr());
    }

    public List<BoundingBox> getResults(String key, int frameIndex) {
        return getResults(handle, key, frameIndex);
    }

    public void removeSource(String key) {
        removeSource(handle, key);
    }

    public void close() {
        close(handle);
    }

    private native long createSpatioTemporalRoIMixer(int frameSize, int fullFrameSize);

    private native int enqueueImage(long handle, String key, long matAddr);

    private native List<BoundingBox> getResults(long handle, String key, int frameIndex);

    private native void removeSource(long handle, String key);

    private native void close(long handle);
}
