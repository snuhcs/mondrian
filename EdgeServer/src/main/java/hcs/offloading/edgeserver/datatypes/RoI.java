package hcs.offloading.edgeserver.datatypes;

import android.graphics.Bitmap;
import android.graphics.Rect;

import java.util.List;

public class RoI {
    public final Frame frame;
    public final Rect position;

    public final float scale;
    public final int[] packedLocation;

    public final RoIType type;
    public final String labelName;

    public List<BoundingBox> boundingBoxes;

    public RoI(Frame frame, Rect position, RoIType type, String labelName) {
        this.frame = frame;
        this.position = position;
        this.scale = 1f;
        this.packedLocation = null;
        this.type = type;
        this.labelName = labelName;
    }

    private RoI(RoI roi, float scale) {
        this.frame = roi.frame;
        this.position = roi.position;
        this.scale = scale;
        this.packedLocation = roi.packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    private RoI(RoI roi, int[] packedLocation) {
        this.frame = roi.frame;
        this.position = roi.position;
        this.scale = roi.scale;
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
        int basis = Math.max(position.height(), position.width());
        if (basis > lengthThreshold) {
            float updatedScale = ((float) lengthThreshold) / basis;
            return new RoI(this, updatedScale);
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
        return new int[]{(int) Math.max(1f, position.width() * scale), (int) Math.max(1f, position.height() * scale)};
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
