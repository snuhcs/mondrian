package hcs.offloading.edgeserver.datatypes;

import android.graphics.Bitmap;
import android.graphics.Rect;

public class RoI {
    public final Frame frame;
    public final Rect position;

    public float scale = 1f;
    public int[] packedLocation;

    public RoI(Frame frame, Rect position) {
        this.frame = frame;
        this.position = position;
    }

    public Bitmap getBitmap() {
        return Bitmap.createBitmap(frame.bitmap, position.left, position.top, position.width(), position.height());
    }

    public boolean isPacked() {
        return packedLocation != null;
    }

    public void resizeRoI(int areaThreshold) {
        if (position.height() * position.width() > areaThreshold) {
            this.scale = (float) Math.sqrt((double) areaThreshold / (position.height() * position.width()));
        }
    }

    public int[] resizedWH() {
        return new int[]{(int) Math.max(1f, position.width() * scale), (int) Math.max(1f, position.height() * scale)};
    }

    public Bitmap resizedBitmap() {
        int[] wh = resizedWH();
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
