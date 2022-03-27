package hcs.offloading.strm.datatypes;

import android.graphics.Bitmap;

import java.util.List;

public class MixedFrame {
    public final Bitmap bitmap;
    public final List<Frame> frames;
    private List<BoundingBox> boxes;

    public MixedFrame(Bitmap bitmap, List<Frame> frames) {
        this.bitmap = bitmap;
        this.frames = frames;
    }

    public void setResults(List<BoundingBox> boxes) {
        this.boxes = boxes;
    }

    public List<BoundingBox> getResults() {
        return boxes;
    }
}
