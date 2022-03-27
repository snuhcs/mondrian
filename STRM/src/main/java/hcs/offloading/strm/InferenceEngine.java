package hcs.offloading.strm;

import android.graphics.Bitmap;

import java.util.List;

import hcs.offloading.strm.datatypes.BoundingBox;

public interface InferenceEngine {
    int enqueue(Bitmap bitmap);

    List<BoundingBox> getResults(int key);
}
