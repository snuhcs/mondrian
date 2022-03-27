package hcs.offloading.edgedevice;

import android.graphics.Bitmap;

import hcs.offloading.strm.datatypes.Frame;

public interface ResultCallback {
    void log(Frame frame);

    void drawInferenceResult(Bitmap bitmap);

    void drawObjectDetectionResult(Bitmap bitmap);

    void drawFPS(String fpsStr);
}
