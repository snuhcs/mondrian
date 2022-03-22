package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Pair;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.ListIterator;
import java.util.Set;
import java.util.stream.Collectors;

import hcs.offloading.edgedevice.config.PatchMixerConfig;
import hcs.offloading.edgedevice.datatypes.Frame;
import hcs.offloading.edgedevice.datatypes.InferenceRequest;
import hcs.offloading.edgedevice.datatypes.RoI;

@RequiresApi(api = Build.VERSION_CODES.P)
public class PatchMixer {
    private static final String TAG = PatchMixer.class.getName();

    private final int MIXED_FRAME_SIZE;
    private final int MAX_OPTICAL_FLOW_INTERVAL;

    private Set<Frame> mPackedFrames;
    private List<RoI> mPackedRoIs;
    private List<Rect> mFreeRects;

    public PatchMixer(PatchMixerConfig config) {
        MAX_OPTICAL_FLOW_INTERVAL = config.MAX_PACKED_FRAMES;
        MIXED_FRAME_SIZE = config.MIXED_FRAME_SIZE;
        reset();
    }

    public void close() {
        reset();
    }

    private void reset() {
        mPackedFrames = new HashSet<>();
        mPackedRoIs = new ArrayList<>();
        mFreeRects = new ArrayList<>();
        mFreeRects.add(new Rect(0, 0, MIXED_FRAME_SIZE, MIXED_FRAME_SIZE));
    }

    public InferenceRequest tryPackRoIs(Frame frame, List<RoI> rois, boolean needPacking) {
        //Log.v(TAG, "Start tryPackRoI : " + frame.sourceIP + " " + frame.frameIndex);
        synchronized (this) {
            mPackedFrames.add(frame);
            int minPackedFrameIndex = mPackedFrames.stream()
                    .filter(f -> f.sourceIP.equals(frame.sourceIP))
                    .map(f -> f.frameIndex)
                    .min(Comparator.comparingInt(i0 -> i0)).get();
            int numPackedFrames = frame.frameIndex - minPackedFrameIndex + 1;
            boolean needInference = numPackedFrames >= MAX_OPTICAL_FLOW_INTERVAL;
            boolean isAllPacked = true;
            for (RoI roi : rois) {
                int[] WH = roi.getResizedWidthHeight();
                ListIterator<Rect> iter = mFreeRects.listIterator();
                boolean isPacked = false;
                while (iter.hasNext()) {
                    Rect freeRect = iter.next();
                    if (canFit(WH, freeRect)) {
                        iter.remove();
                        mPackedRoIs.add(roi.pack(new int[]{freeRect.left, freeRect.top}));
                        Pair<Rect, Rect> newFreeRectPair = splitFreeRect(WH, freeRect);
                        mFreeRects.add(newFreeRectPair.first);
                        mFreeRects.add(newFreeRectPair.second);
                        isPacked = true;
                        break;
                    }
                }
                isAllPacked &= isPacked;
            }
            //Log.v(TAG, "End tryPackRoI : " + frame.sourceIP + " " + frame.frameIndex);
            if (!isAllPacked || needInference) {
                if (mPackedFrames.stream().filter(f -> f.sourceIP.equals(frame.sourceIP)).count() > 1) {
                    mPackedFrames.remove(frame);
                    mPackedRoIs = mPackedRoIs.stream().filter(r -> r.frame != frame).collect(Collectors.toList());
                }
                InferenceRequest request = needPacking
                        ? InferenceRequest.createMixedFrameRequest(
                        getMixedImage(mPackedRoIs), mPackedFrames, mPackedRoIs)
                        : InferenceRequest.createPerRoIInferenceRequest(mPackedFrames, mPackedRoIs);
                reset();
                return request;
            } else {
                return null;
            }
        }
    }

    private Bitmap getMixedImage(List<RoI> rois) {
        Bitmap bitmap = Bitmap.createBitmap(MIXED_FRAME_SIZE, MIXED_FRAME_SIZE, Bitmap.Config.ARGB_8888);
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
