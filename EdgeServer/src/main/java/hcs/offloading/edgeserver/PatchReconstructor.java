package hcs.offloading.edgeserver;

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
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.edgeserver.datatypes.RoI;

public class PatchReconstructor implements Runnable {
    private static final String TAG = PatchReconstructor.class.getName();

    private final int MATCH_PADDING;
    private final float USE_IOU_THRESHOLD;

    private final EdgeServer mEdgeServer;

    private final LinkedBlockingQueue<Pair<InferenceRequest, List<BoundingBox>>> mResultsToReconstruct = new LinkedBlockingQueue<>();

    private final Thread mPatchReconstructorThread;

    PatchReconstructor(PatchReconstructorConfig config, EdgeServer edgeServer) {
        MATCH_PADDING = config.MATCH_PADDING;
        USE_IOU_THRESHOLD = config.USE_IOU_THRESHOLD;

        mEdgeServer = edgeServer;

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

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void run() {
        try {
            long startTime, endTime;
            while (true) {
                Pair<InferenceRequest, List<BoundingBox>> resultToReconstruct = mResultsToReconstruct.take();
                startTime = System.nanoTime();
                Map<String, Map<Integer, List<BoundingBox>>> multiStreamResults = reconstructFrames(resultToReconstruct);
                endTime = System.nanoTime();
                Log.v(TAG, "Reconstructing time: " + (endTime - startTime) / 1000000f);
                mEdgeServer.updateResult(multiStreamResults);
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    public void enqueueInferenceResult(Pair<InferenceRequest, List<BoundingBox>> resultToReconstruct) {
        try {
            mResultsToReconstruct.put(resultToReconstruct);
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    public Map<String, Map<Integer, List<BoundingBox>>> reconstructFrames(Pair<InferenceRequest, List<BoundingBox>> resultToReconstruct) {
        InferenceRequest inferenceRequest = resultToReconstruct.first;
        List<BoundingBox> boxes = resultToReconstruct.second;
        Map<String, Map<Integer, List<BoundingBox>>> batchResults = new HashMap<>();
        for (Map.Entry<String, List<Integer>> kv : inferenceRequest.mixedFrameIndices.entrySet()) {
            batchResults.put(kv.getKey(), new HashMap<>());
            for (int frameIndex : kv.getValue()) {
                batchResults.get(kv.getKey()).put(frameIndex, new ArrayList<>());
            }
        }
        List<RoI> rois = inferenceRequest.rois;
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
                maxRoI.packedLocation = null;
                box = box.move(maxBoxPos);
                batchResults.get(maxRoI.getSourceIP()).get(maxRoI.getFrameIndex()).add(box);
            }
        }
        return batchResults;
    }
}
