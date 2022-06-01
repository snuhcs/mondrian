package hcs.offloading.edgedevicecpp;

import android.graphics.Bitmap;

import java.util.List;

import hcs.offloading.strmcpp.BoundingBox;

public interface ResultCallback {
    void log(String key, int frameIndex, List<BoundingBox> results);

    void drawObjectDetectionResult(Bitmap bitmap);

    void drawFPS(String fpsStr);
}
