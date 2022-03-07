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
    public static Bitmap packRoIs(List<RoI> rois, int targetSize) {
        Bitmap bitmap = Bitmap.createBitmap(targetSize, targetSize, Bitmap.Config.ARGB_8888);
        bitmap.eraseColor(Color.BLACK);
        Canvas canvas = new Canvas(bitmap);

        List<Rect> freeRectList = new ArrayList<>();
        freeRectList.add(new Rect(0, 0, targetSize, targetSize));

        for (RoI roi : rois) {
            int[] WH = roi.resizedWH();
            ListIterator<Rect> iter = freeRectList.listIterator();
            while (iter.hasNext()) {
                Rect freeRect = iter.next();
                if (canFit(WH, freeRect)) {
                    iter.remove();
                    Bitmap resizedRoI = roi.resizedBitmap();
                    roi.packedLocation = new int[]{freeRect.left, freeRect.top};
                    canvas.drawBitmap(resizedRoI, freeRect.left, freeRect.top, null);
                    Pair<Rect, Rect> newFreeRectPair = splitFreeRect(resizedRoI, freeRect);
                    freeRectList.add(newFreeRectPair.first);
                    freeRectList.add(newFreeRectPair.second);
                    break;
                }
            }
        }
        return bitmap;
    }

    private static boolean canFit(int[] WH, Rect rect) {
        return (WH[1] <= rect.height()) && (WH[0] <= rect.width());
    }

    private static Pair<Rect, Rect> splitFreeRect(Bitmap bm, Rect rect) {
        int w = bm.getWidth();
        int h = bm.getHeight();
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
