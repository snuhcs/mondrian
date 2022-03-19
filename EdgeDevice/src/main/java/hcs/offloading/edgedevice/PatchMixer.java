package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.ListIterator;
import java.util.Set;

import hcs.offloading.edgedevice.datatypes.InferenceRequest;
import hcs.offloading.edgedevice.datatypes.RoI;

@RequiresApi(api = Build.VERSION_CODES.P)
public class PatchMixer {
    private static final String TAG = PatchMixer.class.getName();

    private final int MIXED_FRAME_SIZE;

    private final Set<Pair<String, Integer>> mPackedFrames = new HashSet<>();
    private final List<RoI> mPackedRoIs = new ArrayList<>();
    private final List<Rect> mFreeRects = new ArrayList<>();

    public PatchMixer(int mixedFrameSize) {
        MIXED_FRAME_SIZE = mixedFrameSize;
        reset();
    }

    public void close() {
        reset();
    }

    private void reset() {
        mPackedRoIs.clear();
        mFreeRects.clear();
        mFreeRects.add(new Rect(0, 0, MIXED_FRAME_SIZE, MIXED_FRAME_SIZE));
    }

    public InferenceRequest tryPackRoI(Pair<String, Integer> ipIndex, RoI roi) {
        Log.v(TAG, "Start tryPackRoI : " + roi.frame.sourceIP + ", " + roi.frame.frameIndex);
        mPackedFrames.add(ipIndex);
        int[] WH = roi.getResizedWidthHeight();
        synchronized (this) {
            ListIterator<Rect> iter = mFreeRects.listIterator();
            while (iter.hasNext()) {
                Rect freeRect = iter.next();
                if (canFit(WH, freeRect)) {
                    iter.remove();
                    mPackedRoIs.add(roi.pack(new int[]{freeRect.left, freeRect.top}));
                    Pair<Rect, Rect> newFreeRectPair = splitFreeRect(WH, freeRect);
                    mFreeRects.add(newFreeRectPair.first);
                    mFreeRects.add(newFreeRectPair.second);
                    return null;
                }
            }
            Log.v(TAG, "End tryPackRoI : " + roi.frame.sourceIP + ", " + roi.frame.frameIndex);
            return getMixedFrameRequest();
        }
    }

    public InferenceRequest getMixedFrameRequest() {
        Bitmap bitmap = Bitmap.createBitmap(MIXED_FRAME_SIZE, MIXED_FRAME_SIZE, Bitmap.Config.ARGB_8888);
        bitmap.eraseColor(Color.BLACK);
        Canvas canvas = new Canvas(bitmap);
        for (RoI roi : mPackedRoIs) {
            int[] packedLocation = roi.packedLocation;
            canvas.drawBitmap(roi.getResizedBitmap(), packedLocation[0], packedLocation[1], null);
        }
        reset();
        return InferenceRequest.createMixedFrameRequest(bitmap, mPackedRoIs, mPackedFrames);
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
