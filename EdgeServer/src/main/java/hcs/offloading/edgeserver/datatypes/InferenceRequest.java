package hcs.offloading.edgeserver.datatypes;

import java.util.List;
import java.util.Map;

public class InferenceRequest {
    // Informations
    public final Frame frame;
    public final List<RoI> rois; // For Mixed Frame
    public final Map<String, List<Integer>> mixedFrameIndices;

    // Logs
    public int queueSize;
    public float preprocessingTime;
    public float inferenceTime;

    public InferenceRequest(Frame frame) {
        this.frame = frame;
        this.rois = null;
        this.mixedFrameIndices = null;
    }

    public InferenceRequest(Frame frame, List<RoI> rois, Map<String, List<Integer>> mixedFrameIndices) {
        this.frame = frame;
        this.rois = rois;
        this.mixedFrameIndices = mixedFrameIndices;
    }
}
