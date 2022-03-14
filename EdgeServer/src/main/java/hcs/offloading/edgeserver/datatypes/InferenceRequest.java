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

    private boolean isFullFrame = false;
    private boolean isBaseline = false;
    private boolean isMixedFrame = false;

    private InferenceRequest(Frame frame) {
        this.frame = frame;
        this.frames = null;
        this.rois = null;
        this.isFullFrame = true;
    }

    private InferenceRequest(List<Frame> frames, List<RoI> rois) {
        this.frame = null;
        this.frames = frames;
        this.rois = rois;
        this.isBaseline = true;
    }

    private InferenceRequest(Frame frame, List<Frame> frames, List<RoI> rois) {
        this.frame = frame;
        this.frames = frames;
        this.rois = rois;
        this.isMixedFrame = true;
    }

    public static InferenceRequest createFullFrameRequest(Frame frame) {
        return new InferenceRequest(frame);
    }

    public static InferenceRequest createBaselineRequest(List<Frame> frames, List<RoI> rois) {
        return new InferenceRequest(frames, rois);
    }

    public static InferenceRequest createMixedFrameRequest(Frame frame, List<Frame> frames, List<RoI> rois) {
        return new InferenceRequest(frame, frames, rois);
    }

    public boolean isFullFrame() {
        return isFullFrame;
    }

    public boolean isMixed() {
        return isMixedFrame;
    }

    public boolean isBaseline() {
        return isBaseline;
    }
}
