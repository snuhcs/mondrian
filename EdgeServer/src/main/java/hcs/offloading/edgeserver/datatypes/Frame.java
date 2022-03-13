package hcs.offloading.edgeserver.datatypes;

import android.graphics.Bitmap;

public class Frame {
    public final Bitmap bitmap;
    public final String sourceIP;
    public final int index;
    public final long timeNs;

    private Frame(Bitmap bitmap, String sourceIP, int index, long timeNs) {
        this.bitmap = bitmap;
        this.sourceIP = sourceIP;
        this.index = index;
        this.timeNs = timeNs;
    }

    private Frame(Bitmap bitmap) {
        this.bitmap = bitmap;
        this.sourceIP = null;
        this.index = -1;
        this.timeNs = -1;
    }

    public static Frame createSingleFrame(Bitmap bitmap, String sourceIP, int index, long timeNs) {
        return new Frame(bitmap, sourceIP, index, timeNs);
    }

    public static Frame createMixedFrame(Bitmap bitmap) {
        return new Frame(bitmap);
    }

    boolean isMixed() {
        return sourceIP == null;
    }
}
