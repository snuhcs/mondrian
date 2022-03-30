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

    /**
     * Results of RoI inference.
     * Will not be used for single frame inference and mixed frame inference.
     */
    private List<BoundingBox> boxes;

    /**
     * Handle of InferenceEngine.
     * Used for asynchronous RoI frame inference.
     */
    private int handle;

    public final int minOriginLength;
    private float scale;
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

    public Bitmap getBitmap() {
        return Bitmap.createBitmap(frame.bitmap, location.left, location.top, location.width(), location.height());
    }

    public void setHandle(int handle) {
        this.handle = handle;
    }

    public int getHandle() {
        return handle;
    }

    public void resize(float scale) {
        this.scale = scale;
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

    public boolean isPacked() {
        return packedLocation != null;
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
