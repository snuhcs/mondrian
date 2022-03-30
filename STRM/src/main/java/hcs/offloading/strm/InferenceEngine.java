package hcs.offloading.strm;

import android.graphics.Bitmap;

import java.util.List;

import hcs.offloading.strm.datatypes.BoundingBox;

public interface InferenceEngine {
    int enqueue(Bitmap bitmap, boolean isFull) throws InterruptedException;

    List<BoundingBox> getResults(int handle) throws InterruptedException;

    void close();
}
