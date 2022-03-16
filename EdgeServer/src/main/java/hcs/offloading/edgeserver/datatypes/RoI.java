package hcs.offloading.edgeserver.datatypes;

import android.graphics.Bitmap;
import android.graphics.Rect;

import java.util.List;

public class RoI {
    public final Frame frame;
    public final Rect position;

    public final float widthScale;
    public final float heightScale;
    public final int[] packedLocation;

    public final RoIType type;
    public final String labelName;

    public List<BoundingBox> boundingBoxes;

    public RoI(Frame frame, Rect position, RoIType type, String labelName) {
        this.frame = frame;
        this.position = position;
        this.widthScale = 1f;
        this.heightScale = 1f;
        this.packedLocation = null;
        this.type = type;
        this.labelName = labelName;
    }

    private RoI(RoI roi, float widthScale, float heightScale) {
        this.frame = roi.frame;
        this.position = roi.position;
        this.widthScale = widthScale;
        this.heightScale = heightScale;
        this.packedLocation = roi.packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    private RoI(RoI roi, int[] packedLocation) {
        this.frame = roi.frame;
        this.position = roi.position;
        this.widthScale = roi.widthScale;
        this.heightScale = roi.heightScale;
        this.packedLocation = packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    public Bitmap getBitmap() {
        return Bitmap.createBitmap(frame.bitmap, position.left, position.top, position.width(), position.height());
    }

    public boolean isPacked() {
        return packedLocation != null;
    }

    public RoI resize(int lengthThreshold) {
        float widthScale = Math.min(1f, (float) lengthThreshold / position.width());
        float heightScale = Math.min(1f, (float) lengthThreshold / position.height());
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
        return position.width() * position.height();
    }

    public int[] getResizedWidthHeight() {
        return new int[]{
                (int) Math.max(1f, position.width() * widthScale),
                (int) Math.max(1f, position.height() * heightScale)
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
        String result = "RoI " + frame.frameIndex + ": " + position.toString();
        return result.trim();
    }
}
