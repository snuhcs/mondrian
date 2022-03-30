package hcs.offloading.strm;

import android.graphics.Rect;
import android.util.Pair;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;
import java.util.Set;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.PatchMixerConfig;
import hcs.offloading.strm.datatypes.Frame;
import hcs.offloading.strm.datatypes.MixedFrame;
import hcs.offloading.strm.datatypes.RoI;

public class PatchMixer {
    private static final String TAG = PatchMixer.class.getName();

    public enum Status {
        CONTINUE_PACKING,
        FINISHED,
        FINISHED_AND_PROCESS_LAST_FRAME_AGAIN,
    }

    private final PatchMixerConfig mConfig;

    private Set<String> mFinishedKeys;
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

    public Status tryPackAndEnqueueMixedFrame(Frame frame) throws InterruptedException {
        synchronized (this) {
            if (mFinishedKeys != null && mFinishedKeys.contains(frame.key)) {
                mFinishedKeys.remove(frame.key);
                return Status.FINISHED;
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
                Status status = Status.FINISHED;
                if (mPackedFrames.stream().filter(f -> f.key.equals(frame.key)).count() >= 2) {
                    mPackedFrames.remove(frame);
                    status = Status.FINISHED_AND_PROCESS_LAST_FRAME_AGAIN;
                }
                if (mConfig.PACKING) {
                    Mat mixedImage = getMixedImage(mPackedFrames, mConfig.MIXED_FRAME_SIZE);
                    MixedFrame mixedFrame = new MixedFrame(mixedImage, mPackedFrames);
                    mixedFrame.setHandle(mInferenceEngine.enqueue(mixedImage, false));
                    mPatchReconstructor.enqueue(mixedFrame);
                } else {
                    MixedFrame mixedFrame = new MixedFrame(null, mPackedFrames);
                    for (Frame f : mixedFrame.packedFrames) {
                        for (RoI roi : f.getRoIs()) {
                            roi.setHandle(mInferenceEngine.enqueue(roi.getMat(), false));
                        }
                    }
                    mPatchReconstructor.enqueue(mixedFrame);
                }
                mFinishedKeys = mPackedFrames.stream()
                        .map(f -> f.key)
                        .collect(Collectors.toSet());
                mFinishedKeys.remove(frame.key);
                reset();
                return status;
            } else {
                return Status.CONTINUE_PACKING;
            }
        }
    }

    private static Mat getMixedImage(List<Frame> frames, int mixedFrameSize) {
        Mat mat = Mat.zeros(mixedFrameSize, mixedFrameSize, CvType.CV_8UC4);
        for (Frame frame : frames) {
            for (RoI roi : frame.getRoIs()) {
                int[] packedLoc = roi.getPackedLocation();
                if (packedLoc != null) {
                    Mat resizedRoI = roi.getResizedMat();
                    resizedRoI.copyTo(mat.submat(packedLoc[1], packedLoc[1] + resizedRoI.height(),
                            packedLoc[0], packedLoc[0] + resizedRoI.width()));
                }
            }
        }
        return mat;
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
