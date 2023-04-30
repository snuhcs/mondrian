package hcs.offloading.mondrian;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

public class ImageUtils {
    public static Bitmap drawBoxes(Bitmap bitmap, List<BoundingBox> boxes) {
        final Canvas canvas = new Canvas(bitmap);
        for (BoundingBox box : boxes) {
            Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
            paint.setColor(Color.GREEN);
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(5.0f);
            canvas.drawRect(new Rect(box.l, box.t, box.r, box.b), paint);
        }
        return bitmap;
    }

    public static void saveBitmap(Bitmap bmp, String filepath) {
        try (FileOutputStream out = new FileOutputStream(filepath)) {
            bmp.compress(Bitmap.CompressFormat.PNG, 100, out);
            // PNG is a lossless format, the compression factor (100) is ignored
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
