package hcs.offloading.strm.datatypes;

import android.graphics.Bitmap;

import java.util.ArrayList;
import java.util.List;

public class Frame {
    public final Bitmap bitmap;
    public final int key;
    public final int frameIndex;
    private List<RoI> rois;
    private final List<BoundingBox> boxes = new ArrayList<>();

    public Frame(Bitmap bitmap, int key, int frameIndex) {
        this.bitmap = bitmap;
        this.key = key;
        this.frameIndex = frameIndex;
    }

    public void setRoIs(List<RoI> rois) {
        this.rois = rois;
    }

    public List<RoI> getRoIs() {
        return rois;
    }

    public void addResult(BoundingBox box) {
        boxes.add(box);
    }

    public List<BoundingBox> getResults() {
        return boxes;
    }
}
