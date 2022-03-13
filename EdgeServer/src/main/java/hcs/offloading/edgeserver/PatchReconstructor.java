package hcs.offloading.edgeserver;

import android.annotation.SuppressLint;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;

import hcs.offloading.edgeserver.config.PatchReconstructorConfig;
import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.edgeserver.datatypes.RoI;

public class PatchReconstructor implements Runnable {
    private static final String TAG = PatchReconstructor.class.getName();

    private final int MATCH_PADDING;
    private final float USE_IOU_THRESHOLD;

    private final ViewCallback mCallback;

    private final Map<String, Map<Integer, Pair<Frame, List<BoundingBox>>>> mFramesAndResults = new HashMap<>();
    private final LinkedBlockingQueue<Pair<InferenceRequest, List<BoundingBox>>> mMixedFrameResults = new LinkedBlockingQueue<>();

    private final Thread mPatchReconstructorThread;

    private int mNumProcessedFrames = 0;
    private final long mApplicationStartTime = System.nanoTime();

    PatchReconstructor(PatchReconstructorConfig config, ViewCallback callback) {
        MATCH_PADDING = config.MATCH_PADDING;
        USE_IOU_THRESHOLD = config.USE_IOU_THRESHOLD;

        mCallback = callback;

        mPatchReconstructorThread = new Thread(this);
        mPatchReconstructorThread.start();
    }

    public void close() {
        try {
            mPatchReconstructorThread.interrupt();
            mPatchReconstructorThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "closed");
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    public void enqueueInferenceResult(InferenceRequest request, List<BoundingBox> results) {
        mCallback.drawInferenceResult(Utils.drawBoxes(request.frame.bitmap, results));
        if (!request.isMixed()) {
            updateFPS(1);
            Frame frame = request.frame;
            synchronized (mFramesAndResults) {
                if (!mFramesAndResults.containsKey(frame.sourceIP)) {
                    mFramesAndResults.put(frame.sourceIP, new HashMap<>());
                }
                mFramesAndResults.get(frame.sourceIP).put(frame.index, new Pair<>(request.frame, results));
                mFramesAndResults.notifyAll();
            }
        } else {
            mMixedFrameResults.add(new Pair<>(request, results));
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public void run() {
        try {
            long startTime, endTime;
            while (true) {
                Pair<InferenceRequest, List<BoundingBox>> batchResults = mMixedFrameResults.take();

                startTime = System.nanoTime();
                Map<String, Map<Integer, List<BoundingBox>>> mixedFrameResults =
                        reconstructResults(batchResults.first, batchResults.second);
                endTime = System.nanoTime();
                Log.v(TAG, "Reconstructing time (us): " + (endTime - startTime) / 1e3);

                Map<Pair<String, Integer>, Frame> frameMap = new HashMap<>();
                for (Frame frame : batchResults.first.frames) {
                    frameMap.put(new Pair<>(frame.sourceIP, frame.index), frame);
                }

                synchronized (mFramesAndResults) {
                    for (String sourceIP : mixedFrameResults.keySet()) {
                        if (mFramesAndResults.containsKey(sourceIP)) {
                            Map<Integer, List<BoundingBox>> multiFrameResults = mixedFrameResults.get(sourceIP);
                            for (Integer frameIndex : multiFrameResults.keySet()) {
                                Frame frame = frameMap.get(new Pair<>(sourceIP, frameIndex));
                                List<BoundingBox> boxes = multiFrameResults.get(frameIndex);
                                mFramesAndResults.get(sourceIP).put(frameIndex, new Pair<>(frame, boxes));
                            }
                        }
                    }
                    mFramesAndResults.notifyAll();
                }

                updateFPS(batchResults.first.frames.size());
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    public Pair<Frame, List<BoundingBox>> getRemoveDrawResult(String sourceIP, int frameIndex) throws InterruptedException {
        Pair<Frame, List<BoundingBox>> result = null;
        synchronized (mFramesAndResults) {
            Map<Integer, Pair<Frame, List<BoundingBox>>> streamResults = mFramesAndResults.get(sourceIP);
            while (streamResults != null && !streamResults.containsKey(frameIndex)) {
                mFramesAndResults.wait();
                streamResults = mFramesAndResults.get(sourceIP);
            }
            if (streamResults != null) {
                result = streamResults.get(frameIndex);
                streamResults.remove(frameIndex);
            }
        }
        if (result != null) {
            mCallback.drawObjectDetectionResult(Utils.drawBoxes(result.first.bitmap, result.second));
        }
        return result;
    }

    public void removeStream(String sourceIP) {
        synchronized (mFramesAndResults) {
            mFramesAndResults.remove(sourceIP);
        }
    }

    private Map<String, Map<Integer, List<BoundingBox>>> reconstructResults(InferenceRequest request, List<BoundingBox> boxes) {
        Map<String, Map<Integer, List<BoundingBox>>> mixedFrameResults = new HashMap<>();
        for (Frame frame : request.frames) {
            String sourceIP = frame.sourceIP;
            int frameIndex = frame.index;
            if (!mixedFrameResults.containsKey(sourceIP)) {
                mixedFrameResults.put(sourceIP, new HashMap<>());
            }
            mixedFrameResults.get(sourceIP).put(frameIndex, new ArrayList<>());
        }

        List<RoI> rois = request.rois;
        for (BoundingBox box : boxes) {
            float maxIoU = -1f;
            RoI maxRoI = null;
            Rect maxBoxPos = null;
            for (RoI roi : rois) {
                if (!roi.isPacked()) {
                    continue;
                }
                Rect paddedRoIPos = new Rect(
                        roi.position.left - MATCH_PADDING,
                        roi.position.top - MATCH_PADDING,
                        roi.position.right + MATCH_PADDING,
                        roi.position.bottom + MATCH_PADDING
                );
                Rect boxPos = box.location;
                Rect movedAndResizedBoxPos = new Rect(
                        (int) ((boxPos.left - roi.packedLocation[0]) / roi.scale) + roi.position.left,
                        (int) ((boxPos.top - roi.packedLocation[1]) / roi.scale) + roi.position.top,
                        (int) ((boxPos.right - roi.packedLocation[0]) / roi.scale) + roi.position.left,
                        (int) ((boxPos.bottom - roi.packedLocation[1]) / roi.scale) + roi.position.top
                );
                float iou = Utils.box_iou(paddedRoIPos, movedAndResizedBoxPos);
                if (maxRoI == null || maxIoU < iou) {
                    maxIoU = iou;
                    maxRoI = roi;
                    maxBoxPos = movedAndResizedBoxPos;
                }
            }
            if (maxRoI != null && maxIoU > USE_IOU_THRESHOLD) {
                box = box.move(maxBoxPos);
                mixedFrameResults.get(maxRoI.getSourceIP()).get(maxRoI.getFrameIndex()).add(box);
            }
        }
        return mixedFrameResults;
    }

    @SuppressLint("DefaultLocale")
    private void updateFPS(int numProcessedFrames) {
        mNumProcessedFrames += numProcessedFrames;
        float fps = mNumProcessedFrames / ((System.nanoTime() - mApplicationStartTime) / 1e9f);
        mCallback.drawFPS(String.format("%.3f", fps));
    }
}
