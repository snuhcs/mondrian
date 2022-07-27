package hcs.offloading.strm;

import java.util.List;

public class Emulator {
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

    public Emulator(InferenceViewCallback inferenceViewCallback) {
        handle = createSpatioTemporalRoIMixer();
        this.inferenceViewCallback = inferenceViewCallback;
    }

    public void close() {
        close(handle);
    }

    public void drawInferenceResult(long addr, List<BoundingBox> results) {
        inferenceViewCallback.drawInferenceResult(addr, results);
    }

    public void drawObjectDetectionResult(long addr, List<BoundingBox> results) {
        inferenceViewCallback.drawObjectDetectionResult(addr, results);
    }

    private native long createSpatioTemporalRoIMixer();

    private native void close(long handle);
}
