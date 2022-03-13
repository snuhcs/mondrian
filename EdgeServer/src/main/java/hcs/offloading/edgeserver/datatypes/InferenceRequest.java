package hcs.offloading.edgeserver.datatypes;

import java.util.List;

public class InferenceRequest {
    public final Frame frame;

    // For Mixed Frame
    public final List<Frame> frames;
    public final List<RoI> rois;

    // Logs
    public int queueSize = -1;
    public int preprocessingTimeUs = -1;
    public int inferenceTimeUs = -1;

    private InferenceRequest(Frame frame) {
        this.frame = frame;
        this.frames = null;
        this.rois = null;
    }

    private InferenceRequest(Frame frame, List<Frame> frames, List<RoI> rois) {
        this.frame = frame;
        this.frames = frames;
        this.rois = rois;
    }

    public static InferenceRequest createFullFrameRequest(Frame frame) {
        assert !frame.isMixed();
        return new InferenceRequest(frame);
    }

    public static InferenceRequest createMixedFrameRequest(Frame frame, List<Frame> frames, List<RoI> rois) {
        assert frame.isMixed();
        return new InferenceRequest(frame, frames, rois);
    }

    public boolean isMixed() {
        return frame.isMixed();
    }
}
