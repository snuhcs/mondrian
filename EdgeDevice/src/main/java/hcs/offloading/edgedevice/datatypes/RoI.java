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

    public final int minOriginMaxWidthHeight;
    public final float scale;
    public final int[] packedLocation;

    public final Type type;
    public final String labelName;

    public List<BoundingBox> boundingBoxes;

    public RoI(Frame frame, Rect location, Type type, String labelName) {
        this.frame = frame;
        this.location = location;
        this.minOriginMaxWidthHeight = -1;
        this.scale = 1f;
        this.packedLocation = null;
        this.type = type;
        this.labelName = labelName;
    }

    public RoI(Frame frame, Rect location, Type type, String labelName, int minOriginMaxWidthHeight) {
        this.frame = frame;
        this.location = location;
        this.minOriginMaxWidthHeight = minOriginMaxWidthHeight;
        this.scale = 1f;
        this.packedLocation = null;
        this.type = type;
        this.labelName = labelName;
    }

    private RoI(RoI roi, float scale) {
        this.frame = roi.frame;
        this.location = roi.location;
        this.minOriginMaxWidthHeight = -1;
        this.scale = scale;
        this.packedLocation = roi.packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    private RoI(RoI roi, int[] packedLocation) {
        this.frame = roi.frame;
        this.location = roi.location;
        this.minOriginMaxWidthHeight = -1;
        this.scale = roi.scale;
        this.packedLocation = packedLocation;
        this.type = roi.type;
        this.labelName = roi.labelName;
    }

    public Bitmap getBitmap() {
        return Bitmap.createBitmap(frame.bitmap, location.left, location.top, location.width(), location.height());
    }

    public RoI resize(int lengthThreshold) {
//        int maxWidthHeight = minOriginMaxWidthHeight == -1
//                ? Math.max(location.width(), location.height())
//                : minOriginMaxWidthHeight;
        int maxWidthHeight = Math.max(location.width(), location.height());
        if (maxWidthHeight > lengthThreshold) {
            return new RoI(this, (float) lengthThreshold / maxWidthHeight);
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
                Math.max(1, (int) (location.width() * scale)),
                Math.max(1, (int) (location.height() * scale))};
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
