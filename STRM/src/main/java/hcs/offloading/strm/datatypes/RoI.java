package hcs.offloading.strm.datatypes;

import android.graphics.Bitmap;
import android.graphics.Rect;

import java.util.List;

public class RoI {
    public enum Type {
        OF,
        PD
    }

    public final Frame frame;
    public final Rect location;
    private List<BoundingBox> boxes;

    public final int minOriginLength;
    private final float scale;
    private int[] packedLocation;

    public final Type type;
    public final String labelName;


    public RoI(Frame frame, Rect location, Type type, String labelName) {
        this.frame = frame;
        this.location = location;
        this.minOriginLength = -1;
        this.scale = 1f;
        this.packedLocation = null;
        this.type = type;
        this.labelName = labelName;
    }

    public RoI(Frame frame, Rect location, Type type, String labelName, int minOriginLength) {
        this.frame = frame;
        this.location = location;
        this.minOriginLength = minOriginLength;
        this.scale = 1f;
        this.packedLocation = null;
        this.type = type;
        this.labelName = labelName;
    }

    private RoI(RoI roi, float scale) {
        this.frame = roi.frame;
        this.location = roi.location;
        this.minOriginLength = -1;
        this.scale = scale;
        this.packedLocation = roi.packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    public Bitmap getBitmap() {
        return Bitmap.createBitmap(frame.bitmap, location.left, location.top, location.width(), location.height());
    }

    public RoI resize(float scale) {
        return new RoI(this, scale);
    }

    public float getScale() {
        return scale;
    }

    public void pack(int[] packedLocation) {
        this.packedLocation = packedLocation;
    }

    public int[] getPackedLocation() {
        return packedLocation;
    }

    public int getArea() {
        return location.width() * location.height();
    }

    public int[] getResizedWidthHeight() {
        return new int[]{
                Math.max(1, (int) (location.width() * scale)),
                Math.max(1, (int) (location.height() * scale))};
    }

    public Bitmap getResizedBitmap() {
        int[] wh = getResizedWidthHeight();
        return Bitmap.createScaledBitmap(getBitmap(), wh[0], wh[1], false);
    }

    public int getFrameIndex() {
        return frame.frameIndex;
    }

    public void setResults(List<BoundingBox> boxes) {
        this.boxes = boxes;
    }

    public List<BoundingBox> getResults() {
        return boxes;
    }

    @Override
    public String toString() {
        String result = "RoI " + frame.frameIndex + ": " + location.toString();
        return result.trim();
    }
}
