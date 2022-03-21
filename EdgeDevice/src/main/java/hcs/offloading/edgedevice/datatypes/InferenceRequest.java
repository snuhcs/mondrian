package hcs.offloading.edgedevice.datatypes;

import android.graphics.Bitmap;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class InferenceRequest {
    public enum Type {
        FULL,
        PER_ROI,
        MIXED
    }

    public final Type type;
    public final Bitmap bitmap;
    public int frameIndex;
    public String sourceIP;
    public final Set<Frame> frames;
    public final List<RoI> rois;

    // Logs
    public int queueSize = -1;
    public int preprocessingTimeUs = -1;
    public int inferenceTimeUs = -1;

    private InferenceRequest(Frame frame) {
        this.type = Type.FULL;
        this.bitmap = frame.bitmap;
        this.frameIndex = frame.frameIndex;
        this.sourceIP = frame.sourceIP;
        this.frames = null;
        this.rois = null;
    }

    private InferenceRequest(Frame frame, List<RoI> rois) {
        this.type = Type.PER_ROI;
        this.bitmap = null;
        this.frameIndex = -1;
        this.sourceIP = null;
        this.frames = new HashSet<Frame>() {{
            add(frame);
        }};
        this.rois = rois;
    }

    private InferenceRequest(Bitmap bitmap, Set<Frame> frames, List<RoI> rois) {
        this.type = Type.MIXED;
        this.bitmap = bitmap;
        this.frameIndex = -1;
        this.sourceIP = null;
        this.frames = frames;
        this.rois = rois;
    }

    public static InferenceRequest createFullFrameRequest(Frame frame) {
        return new InferenceRequest(frame);
    }

    public static InferenceRequest createPerRoIInferenceRequest(Frame frame, List<RoI> rois) {
        return new InferenceRequest(frame, rois);
    }

    public static InferenceRequest createMixedFrameRequest(Bitmap bitmap, Set<Frame> frames, List<RoI> rois) {
        return new InferenceRequest(bitmap, frames, rois);
    }
}
