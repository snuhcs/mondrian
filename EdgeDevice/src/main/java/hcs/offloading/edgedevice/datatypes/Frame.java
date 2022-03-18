package hcs.offloading.edgedevice.datatypes;

import android.graphics.Bitmap;

public class Frame {
    public final Bitmap bitmap;
    public final String sourceIP;
    public final int frameIndex;

    private Frame(Bitmap bitmap, String sourceIP, int frameIndex) {
        this.bitmap = bitmap;
        this.sourceIP = sourceIP;
        this.frameIndex = frameIndex;
    }

    private Frame(Bitmap bitmap) {
        this.bitmap = bitmap;
        this.sourceIP = null;
        this.frameIndex = -1;
    }

    public static Frame createSingleFrame(Bitmap bitmap, String sourceIP, int frameIndex) {
        return new Frame(bitmap, sourceIP, frameIndex);
    }

    public static Frame createMixedFrame(Bitmap bitmap) {
        return new Frame(bitmap);
    }
}
