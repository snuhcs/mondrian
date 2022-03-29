package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.List;
import java.util.stream.Collectors;

import hcs.offloading.strm.datatypes.BoundingBox;

public class STRMUtils {
    public static List<BoundingBox> filterPerson(List<BoundingBox> results) {
        return results.stream().filter(result -> result.labelName.equals("person")).collect(Collectors.toList());
    }

    public static Bitmap drawBoxes(Bitmap image, List<BoundingBox> boxes, float drawConfidence) {
        final Canvas canvas = new Canvas(image);
        final Paint paint = new Paint();
        paint.setColor(Color.HSVToColor(new float[]{120f, 1f, 1f}));
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5.0f);
        for (BoundingBox box : boxes) {
            if (box.location != null && box.confidence >= drawConfidence) {
                canvas.drawRect(box.location, paint);
            }
        }
        return image;
    }

    public static float box_iou(Rect a, Rect b) {
        return box_intersection(a, b) / box_union(a, b);
    }

    public static float box_intersection(Rect a, Rect b) {
        float w = overlap((a.left + a.right) / 2, a.right - a.left,
                (b.left + b.right) / 2, b.right - b.left);
        float h = overlap((a.top + a.bottom) / 2, a.bottom - a.top,
                (b.top + b.bottom) / 2, b.bottom - b.top);
        if (w < 0 || h < 0) return 0;
        return w * h;
    }

    public static float box_union(Rect a, Rect b) {
        float i = box_intersection(a, b);
        return (a.right - a.left) * (a.bottom - a.top) + (b.right - b.left) * (b.bottom - b.top) - i;
    }

    public static float overlap(int x1, int w1, int x2, int w2) {
        float l1 = x1 - w1 / 2;
        float l2 = x2 - w2 / 2;
        float left = Math.max(l1, l2);
        float r1 = x1 + w1 / 2;
        float r2 = x2 + w2 / 2;
        float right = Math.min(r1, r2);
        return right - left;
    }


    private static String convertStreamToString(InputStream is) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            sb.append(line).append("\n");
        }
        reader.close();
        return sb.toString();
    }

    public static String getStringFromFile(String filePath) throws IOException {
        File fl = new File(filePath);
        FileInputStream fin = new FileInputStream(fl);
        String ret = convertStreamToString(fin);
        fin.close();
        return ret;
    }
}
