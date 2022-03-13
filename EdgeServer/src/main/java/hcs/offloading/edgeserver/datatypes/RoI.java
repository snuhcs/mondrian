package hcs.offloading.edgeserver.datatypes;

import android.graphics.Bitmap;
import android.graphics.Rect;

public class RoI {
    public final Frame frame;
    public final Rect position;

    public final float scale;
    public final int[] packedLocation;

    public RoI(Frame frame, Rect position) {
        this.frame = frame;
        this.position = position;
        this.scale = 1f;
        this.packedLocation = null;
    }

    private RoI(RoI roi, float scale) {
        this.frame = roi.frame;
        this.position = roi.position;
        this.scale = scale;
        this.packedLocation = roi.packedLocation;
    }

    private RoI(RoI roi, int[] packedLocation) {
        this.frame = roi.frame;
        this.position = roi.position;
        this.scale = roi.scale;
        this.packedLocation = packedLocation;
    }

    public Bitmap getBitmap() {
        return Bitmap.createBitmap(frame.bitmap, position.left, position.top, position.width(), position.height());
    }

    public boolean isPacked() {
        return packedLocation != null;
    }

    public RoI resize(int areaThreshold) {
        if (position.height() * position.width() > areaThreshold) {
            float updatedScale = (float) Math.sqrt((double) areaThreshold / (position.height() * position.width()));
            return new RoI(this, updatedScale);
        }
        return this;
    }

    public RoI pack(int[] packedLocation) {
        return new RoI(this, packedLocation);
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

    @Override
    public String toString() {
        String result = "RoI " + frame.frameIndex + ": " + position.toString();
        return result.trim();
    }
}
