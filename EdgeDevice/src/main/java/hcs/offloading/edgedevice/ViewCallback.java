package hcs.offloading.edgedevice;

import android.graphics.Bitmap;

public interface ViewCallback {
    void drawInferenceResult(Bitmap bitmap);

    void drawObjectDetectionResult(Bitmap bitmap);

    void drawFPS(String fpsStr);
}
