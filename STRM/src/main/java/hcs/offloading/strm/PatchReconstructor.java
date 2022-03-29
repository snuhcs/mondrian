package hcs.offloading.strm;

import android.graphics.Rect;

import hcs.offloading.strm.config.PatchReconstructorConfig;
import hcs.offloading.strm.datatypes.BoundingBox;
import hcs.offloading.strm.datatypes.Frame;
import hcs.offloading.strm.datatypes.MixedFrame;
import hcs.offloading.strm.datatypes.RoI;

public class PatchReconstructor extends Consumer<MixedFrame> {
    private static final String TAG = PatchReconstructor.class.getName();

    private final PatchReconstructorConfig mConfig;

    private final InferenceEngine mInferenceEngine;

    public PatchReconstructor(PatchReconstructorConfig config, InferenceEngine inferenceEngine,
                              ConsumerCallback<MixedFrame> consumerCallback) {
        super(TAG, config.MAX_QUEUE_SIZE, consumerCallback);
        mConfig = config;
        mInferenceEngine = inferenceEngine;
    }

    @Override
    public void process(MixedFrame mixedFrame) throws InterruptedException {
        if (mixedFrame.packedBitmap != null) {
            mixedFrame.setResults(mInferenceEngine.getResults(mixedFrame.getHandle()));
            updateMixedFrameInferenceResults(mixedFrame, mConfig.MATCH_PADDING, mConfig.USE_IOU_THRESHOLD);
        } else {
            for (Frame frame : mixedFrame.packedFrames) {
                for (RoI roi : frame.getRoIs()) {
                    roi.setResults(mInferenceEngine.getResults(roi.getHandle()));
                }
            }
            updateRoIInferenceResults(mixedFrame);
        }
    }

    private static void updateMixedFrameInferenceResults(
            MixedFrame mixedFrame, int matchPadding, float useIoUThreshold) {
        for (BoundingBox box : mixedFrame.getResults()) {
            float maxOverlap = -1f;
            Rect maxBoxPos = null;
            Frame maxFrame = null;
            for (Frame frame : mixedFrame.packedFrames) {
                for (RoI roi : frame.getRoIs()) {
                    if (!roi.isPacked()) {
                        continue;
                    }
                    Rect paddedRoIPos = new Rect(
                            Math.max(0, roi.location.left - matchPadding),
                            Math.max(0, roi.location.top - matchPadding),
                            Math.min(roi.frame.bitmap.getWidth(), roi.location.right + matchPadding),
                            Math.min(roi.frame.bitmap.getHeight(), roi.location.bottom + matchPadding)
                    );
                    Rect boxPos = box.location;
                    Rect movedAndResizedBoxPos = new Rect(
                            Math.max(0, (int) ((boxPos.left - roi.getPackedLocation()[0]) / roi.getScale()) + roi.location.left),
                            Math.max(0, (int) ((boxPos.top - roi.getPackedLocation()[1]) / roi.getScale()) + roi.location.top),
                            Math.min(roi.frame.bitmap.getWidth(), (int) ((boxPos.right - roi.getPackedLocation()[0]) / roi.getScale()) + roi.location.left),
                            Math.min(roi.frame.bitmap.getHeight(), (int) ((boxPos.bottom - roi.getPackedLocation()[1]) / roi.getScale()) + roi.location.top)
                    );
                    float intersection = STRMUtils.box_intersection(paddedRoIPos, movedAndResizedBoxPos);
                    float overlapRatio = Math.max(
                            intersection / (movedAndResizedBoxPos.width() * movedAndResizedBoxPos.height()),
                            intersection / (paddedRoIPos.width() * paddedRoIPos.height()));
                    if (maxOverlap < overlapRatio) {
                        maxOverlap = overlapRatio;
                        maxBoxPos = movedAndResizedBoxPos;
                        maxFrame = frame;
                    }
                }
            }
            if (maxFrame != null && maxOverlap > useIoUThreshold) {
                maxFrame.addResult(new BoundingBox(
                        maxBoxPos, box.confidence, box.labelName));
            }
        }
    }

    private static void updateRoIInferenceResults(MixedFrame mixedFrame) {
        for (Frame frame : mixedFrame.packedFrames) {
            for (RoI roi : frame.getRoIs()) {
                for (BoundingBox box : roi.getResults()) {
                    Rect newLocation = new Rect(
                            box.location.left + roi.location.left,
                            box.location.top + roi.location.top,
                            box.location.right + roi.location.left,
                            box.location.bottom + roi.location.top
                    );
                    frame.addResult(new BoundingBox(newLocation, box.confidence, box.labelName));
                }
            }
        }
    }
}
