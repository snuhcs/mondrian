package hcs.offloading.mondrian;

import android.graphics.Rect;
import android.view.Surface;
import android.view.SurfaceHolder;

public class JniRenderer {
    public static boolean draw(SurfaceHolder surfaceHolder, byte[] img) {
        Rect surfaceFrame = surfaceHolder.getSurfaceFrame();
        Surface surface = surfaceHolder.getSurface();
        if (!surface.isValid()) return false;
        return drawFrameByte(surface, img, surfaceFrame.width(), surfaceFrame.height());
    }

    private static native boolean drawFrameByte(Surface surface, byte[] img, int width, int height);
}
