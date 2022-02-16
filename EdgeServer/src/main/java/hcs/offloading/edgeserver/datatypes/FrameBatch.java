package hcs.offloading.edgeserver.datatypes;

import java.util.List;

public class FrameBatch {
    public final List<Frame> frames;
    public final Frame prevFrame;
    public final List<BoundingBox> prevResults;

    public FrameBatch(List<Frame> frames, Frame prevFrame, List<BoundingBox> prevResults) {
        this.frames = frames;
        this.prevFrame = prevFrame;
        this.prevResults = prevResults;
    }
}
