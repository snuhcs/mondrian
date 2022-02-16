package hcs.offloading.edgeserver.datatypes;

import android.graphics.Bitmap;

public class MixedFrame extends Frame {
    public static final String IP = "MIXED_IP";
    public static final int INDEX = -1;

    public MixedFrame(Bitmap bitmap) {
        super(IP, INDEX, bitmap);
    }
}
