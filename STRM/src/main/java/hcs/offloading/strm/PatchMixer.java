package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.util.Pair;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.PatchMixerConfig;
import hcs.offloading.strm.datatypes.Frame;
import hcs.offloading.strm.datatypes.MixedFrame;
import hcs.offloading.strm.datatypes.RoI;

public class PatchMixer {
    private static final String TAG = PatchMixer.class.getName();

    public static final int CONTINUE_PACKING = -1;

    private final PatchMixerConfig mConfig;

    private final Map<String, Integer> mLastPackedIndices = new HashMap<>();
    private List<Frame> mPackedFrames;
    private List<Rect> mFreeRects;

    private final InferenceEngine mInferenceEngine;
    private final PatchReconstructor mPatchReconstructor;

    public PatchMixer(PatchMixerConfig config, InferenceEngine inferenceEngine, PatchReconstructor patchReconstructor) {
        mConfig = config;
        mInferenceEngine = inferenceEngine;
        mPatchReconstructor = patchReconstructor;
        reset();
    }

    private void reset() {
        mPackedFrames = new ArrayList<>();
        mFreeRects = new ArrayList<>();
        mFreeRects.add(new Rect(0, 0, mConfig.MIXED_FRAME_SIZE, mConfig.MIXED_FRAME_SIZE));
    }

    public int tryPackAndEnqueueMixedFrame(Frame frame) throws InterruptedException {
        synchronized (this) {
            Integer lastIndex = mLastPackedIndices.remove(frame.key);
            if (lastIndex != null) {
                return lastIndex;
            }
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
                    .filter(f -> f.key.equals(frame.key))
                    .map(f -> f.frameIndex)
                    .min(Integer::compare)
                    .orElseThrow(() -> new ArrayIndexOutOfBoundsException("No frames with given index"));
            int numPackedFrames = frame.frameIndex - minPackedFrameIndex + 1;
            if (!isAllPacked || numPackedFrames >= mConfig.MAX_PACKED_FRAMES) {
                if (mPackedFrames.stream().filter(f -> f.key.equals(frame.key)).count() >= 2) {
                    mPackedFrames.remove(frame);
                }
                mPackedFrames.stream()
                        .collect(Collectors.groupingBy(f -> f.key))
                        .forEach((key, frames) -> mLastPackedIndices.put(
                                key, frames.stream()
                                        .map(f -> f.frameIndex)
                                        .max(Integer::compare)
                                        .orElseThrow(() -> new ArrayIndexOutOfBoundsException("No frames with given index"))));
                if (mConfig.PACKING) {
                    Bitmap mixedImage = getMixedImage(mPackedFrames, mConfig.MIXED_FRAME_SIZE);
                    MixedFrame mixedFrame = new MixedFrame(mixedImage, mPackedFrames);
                    mixedFrame.setHandle(mInferenceEngine.enqueue(mixedImage, false));
                    mPatchReconstructor.enqueue(mixedFrame);
                } else {
                    MixedFrame mixedFrame = new MixedFrame(null, mPackedFrames);
                    for (Frame f : mixedFrame.packedFrames) {
                        for (RoI roi : f.getRoIs()) {
                            roi.setHandle(mInferenceEngine.enqueue(roi.getBitmap(), false));
                        }
                    }
                    mPatchReconstructor.enqueue(mixedFrame);
                }
                reset();
                return mLastPackedIndices.remove(frame.key);
            } else {
                return CONTINUE_PACKING;
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
