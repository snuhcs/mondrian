package hcs.offloading.edgedevice.datatypes;

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

    public final float widthScale;
    public final float heightScale;
    public final int[] packedLocation;

    public final Type type;
    public final String labelName;

    public List<BoundingBox> boundingBoxes;

    public RoI(Frame frame, Rect location, Type type, String labelName) {
        this.frame = frame;
        this.location = location;
        this.widthScale = 1f;
        this.heightScale = 1f;
        this.packedLocation = null;
        this.type = type;
        this.labelName = labelName;
    }

    private RoI(RoI roi, float widthScale, float heightScale) {
        this.frame = roi.frame;
        this.location = roi.location;
        this.widthScale = widthScale;
        this.heightScale = heightScale;
        this.packedLocation = roi.packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    private RoI(RoI roi, int[] packedLocation) {
        this.frame = roi.frame;
        this.location = roi.location;
        this.widthScale = roi.widthScale;
        this.heightScale = roi.heightScale;
        this.packedLocation = packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    public Bitmap getBitmap() {
        return Bitmap.createBitmap(frame.bitmap, location.left, location.top, location.width(), location.height());
    }

    public boolean isPacked() {
        return packedLocation != null;
    }

    public RoI resize(int lengthThreshold) {
        float widthScale = Math.min(1f, (float) lengthThreshold / location.width());
        float heightScale = Math.min(1f, (float) lengthThreshold / location.height());
        widthScale = Math.min(widthScale, heightScale);
        heightScale = widthScale;
        if (widthScale != 1f || heightScale != 1f) {
            return new RoI(this, widthScale, heightScale);
        }
        return this;
    }

    public RoI pack(int[] packedLocation) {
        return new RoI(this, packedLocation);
    }

    public int getArea() {
        return location.width() * location.height();
    }

    public int[] getResizedWidthHeight() {
        return new int[]{
                (int) Math.max(1f, location.width() * widthScale),
                (int) Math.max(1f, location.height() * heightScale)
        };
    }

    public Bitmap getResizedBitmap() {
        int[] wh = getResizedWidthHeight();
        return Bitmap.createScaledBitmap(getBitmap(), wh[0], wh[1], false);
    }

    public String getSourceIP() {
        return frame.sourceIP;
    }

    public int getFrameIndex() {
        return frame.frameIndex;
    }

    public void setBoundingBoxes(List<BoundingBox> boundingBoxes) {
        this.boundingBoxes = boundingBoxes;
    }

    @Override
    public String toString() {
        String result = "RoI " + frame.frameIndex + ": " + location.toString();
        return result.trim();
    }
}
