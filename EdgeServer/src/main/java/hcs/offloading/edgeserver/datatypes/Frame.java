package hcs.offloading.edgeserver.datatypes;

import android.graphics.Bitmap;

public class Frame {
    private static final int MIXED_INDEX = -1;

    public final String sourceIP;
    public final int frameIndex;
    public final Bitmap bitmap;

    public Frame(String sourceIP, int frameIndex, Bitmap bitmap) {
        this.sourceIP = sourceIP;
        this.frameIndex = frameIndex;
        this.bitmap = bitmap;
    }

    public static Frame createMixedFrame(Bitmap bitmap) {
        return new Frame(null, MIXED_INDEX, bitmap);
    }

    public boolean isMixedFrame() {
        return frameIndex == MIXED_INDEX;
    }
}
