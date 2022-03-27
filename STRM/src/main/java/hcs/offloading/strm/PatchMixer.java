package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.ListIterator;

import hcs.offloading.strm.config.PatchMixerConfig;
import hcs.offloading.strm.datatypes.Frame;
import hcs.offloading.strm.datatypes.MixedFrame;
import hcs.offloading.strm.datatypes.RoI;

@RequiresApi(api = Build.VERSION_CODES.P)
public class PatchMixer {
    private static final String TAG = PatchMixer.class.getName();

    private final PatchMixerConfig mConfig;

    private List<Frame> mPackedFrames;
    private List<Rect> mFreeRects;

    public PatchMixer(PatchMixerConfig config) {
        mConfig = config;
        reset();
    }

    private void reset() {
        mPackedFrames = new ArrayList<>();
        mFreeRects = new ArrayList<>();
        mFreeRects.add(new Rect(0, 0, mConfig.MIXED_FRAME_SIZE, mConfig.MIXED_FRAME_SIZE));
    }

    public MixedFrame tryPackFrameRoIs(Frame frame) {
        synchronized (this) {
            boolean isAllPacked = true;
            for (RoI roi : frame.getRoIs()) {
                int[] WH = roi.getResizedWidthHeight();
                ListIterator<Rect> iter = mFreeRects.listIterator();
                boolean isPacked = false;
                while (iter.hasNext()) {
                    Rect freeRect = iter.next();
                    if (canFit(WH, freeRect)) {
                        iter.remove();
                        roi.pack(new int[]{freeRect.left, freeRect.top});
                        Pair<Rect, Rect> newFreeRectPair = splitFreeRect(WH, freeRect);
                        mFreeRects.add(newFreeRectPair.first);
                        mFreeRects.add(newFreeRectPair.second);
                        isPacked = true;
                        break;
                    }
                }
                isAllPacked &= isPacked;
            }

            mPackedFrames.add(frame);
            int minPackedFrameIndex = mPackedFrames.stream()
                    .filter(f -> f.key == frame.key)
                    .map(f -> f.frameIndex)
                    .min(Comparator.comparingInt(i0 -> i0))
                    .get();
            int numPackedFrames = frame.frameIndex - minPackedFrameIndex + 1;
            if (!isAllPacked || numPackedFrames >= mConfig.MAX_PACKED_FRAMES) {
                if (mPackedFrames.stream().filter(f -> f.key == frame.key).count() >= 2) {
                    mPackedFrames.remove(frame);
                }
                Bitmap mixedImage = mConfig.PACKING
                        ? getMixedImage(mPackedFrames, mConfig.MIXED_FRAME_SIZE)
                        : null;
                MixedFrame mixedFrame = new MixedFrame(mixedImage, mPackedFrames);
                reset();
                return mixedFrame;
            } else {
                return null;
            }
        }
    }

    private static Bitmap getMixedImage(List<Frame> frames, int mixedFrameSize) {
        Bitmap bitmap = Bitmap.createBitmap(mixedFrameSize, mixedFrameSize, Bitmap.Config.ARGB_8888);
        bitmap.eraseColor(Color.BLACK);
        Canvas canvas = new Canvas(bitmap);
        for (Frame frame : frames) {
            for (RoI roi : frame.getRoIs()) {
                int[] packedLocation = roi.getPackedLocation();
                if (packedLocation != null) {
                    canvas.drawBitmap(roi.getResizedBitmap(), packedLocation[0], packedLocation[1], null);
                }
            }
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
