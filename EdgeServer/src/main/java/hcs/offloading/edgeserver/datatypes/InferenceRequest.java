package hcs.offloading.edgeserver.datatypes;

import java.util.List;

public class InferenceRequest {
    public enum Type {
        FULL,
        PER_ROI,
        MIXED
    }

    public final Frame frame;
    public final Type type;

    // For SINGLE_ROI_FRAME / MIXED_FRAME
    public final List<Frame> frames;
    public final List<RoI> rois;

    // Logs
    public int queueSize = -1;
    public int preprocessingTimeUs = -1;
    public int inferenceTimeUs = -1;

    private InferenceRequest(Frame frame, Type type, List<Frame> frames, List<RoI> rois) {
        this.frame = frame;
        this.type = type;
        this.frames = frames;
        this.rois = rois;
    }

    public static InferenceRequest createFullFrameRequest(Frame frame) {
        return new InferenceRequest(frame, Type.FULL, null, null);
    }

    public static InferenceRequest createSingleRoIFrameRequest(List<Frame> frames, List<RoI> rois) {
        return new InferenceRequest(null, Type.PER_ROI, frames, rois);
    }

    public static InferenceRequest createMixedFrameRequest(Frame frame, List<Frame> frames, List<RoI> rois) {
        return new InferenceRequest(frame, Type.MIXED, frames, rois);
    }
}
