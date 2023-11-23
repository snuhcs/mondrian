package hcs.offloading.mondrian;

import android.graphics.Rect;
import android.view.Surface;
import android.view.SurfaceHolder;

public class JniRenderer {
    public static boolean draw(SurfaceHolder surfaceHolder, byte[] img, int width, int height) {
        assert (img.length == 4 * width * height);
        assert (width == surfaceHolder.getSurfaceFrame().width());
        assert (height == surfaceHolder.getSurfaceFrame().height());
        Surface surface = surfaceHolder.getSurface();
        if (!surface.isValid()) return false;
        return drawFrameByte(surface, img, width, height);
    }

    private static native boolean drawFrameByte(Surface surface, byte[] img, int width, int height);
}
