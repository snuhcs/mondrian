package hcs.offloading.strmcpp;

import android.graphics.Bitmap;

public class SpatioTemporalRoIMixer {

    private long strmHandle;

    SpatioTemporalRoIMixer() {
        strmHandle = createSpatioTemporalRoIMixer();
    }

    public void enqueueImage(String key, int frameIndex, Bitmap mat) {
        enqueueImage(strmHandle, key, frameIndex, mat);
    }

    private native long createSpatioTemporalRoIMixer();

    private native void enqueueImage(long strmHandle, String key, int frameIndex, Bitmap mat);
}
