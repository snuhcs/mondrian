package hcs.offloading.edgedevice;

import android.graphics.Bitmap;

import java.util.List;

import hcs.offloading.strm.datatypes.BoundingBox;

public interface ResultCallback {
    void log(String key, int frameIndex, List<BoundingBox> results);

    void drawInferenceResult(Bitmap bitmap);

    void drawObjectDetectionResult(Bitmap bitmap);

    void drawFPS(String fpsStr);
}
