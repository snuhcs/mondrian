package hcs.offloading.strm.datatypes;

import org.opencv.core.Mat;

import java.util.List;

public class MixedFrame {
    public final Mat packedMat;
    public final List<Frame> packedFrames;

    /**
     * Results of mixed frame inference.
     * Will not be used for single frame inference and single roi inference.
     */
    private List<BoundingBox> boxes;

    /**
     * Handle of InferenceEngine.
     * Used for asynchronous mixed frame inference.
     */
    private int handle;

    public MixedFrame(Mat packedMat, List<Frame> packedFrames) {
        this.packedMat = packedMat;
        this.packedFrames = packedFrames;
    }

    public void setResults(List<BoundingBox> boxes) {
        this.boxes = boxes;
    }

    public List<BoundingBox> getResults() {
        return boxes;
    }

    public void setHandle(int handle) {
        this.handle = handle;
    }

    public int getHandle() {
        return handle;
    }
}
