package hcs.offloading.strm;

import org.opencv.core.Mat;

import java.util.List;

public class SpatioTemporalRoIMixer {
    static {
        System.loadLibrary("strm");
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
    private final InferenceViewCallback inferenceViewCallback;

    public SpatioTemporalRoIMixer(InferenceViewCallback inferenceViewCallback) {
        handle = createSpatioTemporalRoIMixer();
        this.inferenceViewCallback = inferenceViewCallback;
    }

    public void drawInferenceResult(long addr, List<BoundingBox> results) {
        inferenceViewCallback.drawInferenceResult(addr, results);
    }

    public void drawObjectDetectionResult(long addr, List<BoundingBox> results) {
        inferenceViewCallback.drawObjectDetectionResult(addr, results);
    }

    public void enqueueImage(String key, Mat mat) {
        enqueueImage(handle, key, mat.getNativeObjAddr());
    }

    public void close() {
        close(handle);
    }

    private native long createSpatioTemporalRoIMixer();

    private native void enqueueImage(long handle, String key, long matAddr);

    private native void close(long handle);
}
