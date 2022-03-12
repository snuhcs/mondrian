package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Pair;

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;

import hcs.offloading.edgeserver.datatypes.RoI;

@RequiresApi(api = Build.VERSION_CODES.P)
public class PatchMixer {
    public static List<RoI> packRoIs(List<RoI> rois, int targetSize) {
        List<RoI> packedRoIs = new ArrayList<>();

        List<Rect> freeRectList = new ArrayList<>();
        freeRectList.add(new Rect(0, 0, targetSize, targetSize));

        for (RoI roi : rois) {
            int[] WH = roi.getResizedWidthHeight();
            ListIterator<Rect> iter = freeRectList.listIterator();
            while (iter.hasNext()) {
                Rect freeRect = iter.next();
                if (canFit(WH, freeRect)) {
                    iter.remove();
                    packedRoIs.add(roi.pack(new int[]{freeRect.left, freeRect.top}));
                    Pair<Rect, Rect> newFreeRectPair = splitFreeRect(WH, freeRect);
                    freeRectList.add(newFreeRectPair.first);
                    freeRectList.add(newFreeRectPair.second);
                    break;
                }
            }
        }
        return packedRoIs;
    }

    public static Bitmap getMixedFrame(List<RoI> rois, int targetSize) {
        Bitmap bitmap = Bitmap.createBitmap(targetSize, targetSize, Bitmap.Config.ARGB_8888);
        bitmap.eraseColor(Color.BLACK);

        Canvas canvas = new Canvas(bitmap);
        for (RoI roi : rois) {
            int[] packedLocation = roi.packedLocation;
            canvas.drawBitmap(roi.getResizedBitmap(), packedLocation[0], packedLocation[1], null);
        }

        return bitmap;
    }

    private static boolean canFit(int[] wh, Rect rect) {
        return (wh[1] <= rect.height()) && (wh[0] <= rect.width());
    }

    private static Pair<Rect, Rect> splitFreeRect(int[] wh, Rect rect) {
        int w = wh[0];
        int h = wh[1];
        Rect rect1, rect2;
        if (rect.width() > rect.height()) {
            rect1 = new Rect(rect.left + w, rect.top, rect.right, rect.bottom);
            rect2 = new Rect(rect.left, rect.top + h, rect.left + w, rect.bottom);
        } else {
            rect1 = new Rect(rect.left, rect.top + h, rect.right, rect.bottom);
            rect2 = new Rect(rect.left + w, rect.top, rect.right, rect.top + h);
        }
        return new Pair<>(rect1, rect2);
    }
}
