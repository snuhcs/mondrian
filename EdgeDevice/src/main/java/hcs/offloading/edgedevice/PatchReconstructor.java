package hcs.offloading.edgedevice;

import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.stream.Collectors;

import hcs.offloading.edgedevice.config.PatchReconstructorConfig;
import hcs.offloading.edgedevice.datatypes.BoundingBox;
import hcs.offloading.edgedevice.datatypes.InferenceRequest;
import hcs.offloading.edgedevice.datatypes.RoI;

public class PatchReconstructor implements Runnable {
    private static final String TAG = PatchReconstructor.class.getName();

    private final int MATCH_PADDING;
    private final float USE_IOU_THRESHOLD;
    private final float DRAW_CONFIDENCE;

    public interface Callback {
        void enqueueResult(String sourceIP, int frameIndex, List<BoundingBox> result);
    }

    private final Callback mCallback;
    private final ViewCallback mViewCallback;

    private final LinkedBlockingQueue<Pair<InferenceRequest, List<BoundingBox>>> mResultsToReconstruct = new LinkedBlockingQueue<>();

    private final Thread mPatchReconstructorThread;

    private FileWriter logWriter;
    private final long mStartTimeNs = System.nanoTime();

    PatchReconstructor(PatchReconstructorConfig config, Callback callback, ViewCallback viewCallback) {
        MATCH_PADDING = config.MATCH_PADDING;
        USE_IOU_THRESHOLD = config.USE_IOU_THRESHOLD;
        DRAW_CONFIDENCE = config.DRAW_CONFIDENCE;
        if (config.LOG_PATH != null) {
            try {
                logWriter = new FileWriter(config.LOG_PATH);
            } catch (IOException e) {
                Log.e(TAG, e.getMessage());
            }
        }
        mCallback = callback;
        mViewCallback = viewCallback;

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
        if (logWriter != null) {
            try {
                logWriter.flush();
                logWriter.close();
            } catch (IOException e) {
                Log.e(TAG, e.getMessage());
            }
        }
        Log.d(TAG, "closed");
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    public void enqueueInferenceResults(InferenceRequest request, List<BoundingBox> results) {
        Log.v(TAG, "Start enqueueInferenceResults() : " + request.type);
        if (request.type == InferenceRequest.Type.MIXED || request.type == InferenceRequest.Type.PER_ROI) {
            mResultsToReconstruct.add(new Pair<>(request, results));
        } else if (request.type == InferenceRequest.Type.FULL) {
            mCallback.enqueueResult(request.sourceIP, request.frameIndex, results);
        } else {
            throw new IllegalArgumentException("Wrong request type! " + request.type);
        }
        if (request.type == InferenceRequest.Type.MIXED || request.type == InferenceRequest.Type.FULL) {
            mViewCallback.drawInferenceResult(Utils.drawBoxes(request.bitmap, results, DRAW_CONFIDENCE));
        }
        Log.v(TAG, "End enqueueInferenceResults() : " + request.type);
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public void run() {
        try {
            long startTime, endTime;
            while (true) {
                Pair<InferenceRequest, List<BoundingBox>> batchResults = mResultsToReconstruct.take();
                InferenceRequest request = batchResults.first;
                List<BoundingBox> results = batchResults.second;

                startTime = System.nanoTime();
                Map<Pair<String, Integer>, List<BoundingBox>> reconstructedFrameResults;
                if (request.type == InferenceRequest.Type.MIXED) {
                    reconstructedFrameResults = reconstructMixedFrameResults(request, results);
                } else if (request.type == InferenceRequest.Type.PER_ROI) {
                    reconstructedFrameResults = reconstructPerRoIInferenceResults(request);
                } else {
                    throw new IllegalArgumentException("Wrong request type! " + request.type);
                }
                endTime = System.nanoTime();
                Log.v(TAG, "Reconstructing time (us): " + (endTime - startTime) / 1e3);

                for (Pair<String, Integer> ipIndex : reconstructedFrameResults.keySet()) {
                    mCallback.enqueueResult(ipIndex.first, ipIndex.second, reconstructedFrameResults.get(ipIndex));
                }
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    private Map<Pair<String, Integer>, List<BoundingBox>> reconstructMixedFrameResults(InferenceRequest request, List<BoundingBox> boxes) {
        Map<Pair<String, Integer>, List<BoundingBox>> mixedFrameResults = createReconstructedResultHolder(request);

        List<RoI> rois = request.rois;
        for (BoundingBox box : boxes) {
            float maxOverlap = -1f;
            RoI maxRoI = null;
            Rect maxBoxPos = null;
            for (RoI roi : rois) {
                Rect paddedRoIPos = new Rect(
                        roi.location.left - MATCH_PADDING,
                        roi.location.top - MATCH_PADDING,
                        roi.location.right + MATCH_PADDING,
                        roi.location.bottom + MATCH_PADDING
                );
                Rect boxPos = box.location;
                Rect movedAndResizedBoxPos = new Rect(
                        (int) ((boxPos.left - roi.packedLocation[0]) / roi.widthScale) + roi.location.left,
                        (int) ((boxPos.top - roi.packedLocation[1]) / roi.heightScale) + roi.location.top,
                        (int) ((boxPos.right - roi.packedLocation[0]) / roi.widthScale) + roi.location.left,
                        (int) ((boxPos.bottom - roi.packedLocation[1]) / roi.heightScale) + roi.location.top
                );
                float intersection = Utils.box_intersection(paddedRoIPos, movedAndResizedBoxPos);
                float overlapRatio = Math.max(intersection / (movedAndResizedBoxPos.width() * movedAndResizedBoxPos.height()),
                        intersection / (paddedRoIPos.width() * paddedRoIPos.height()));
                if (maxRoI == null || maxOverlap < overlapRatio) {
                    maxOverlap = overlapRatio;
                    maxRoI = roi;
                    maxBoxPos = movedAndResizedBoxPos;
                }
            }
            if (maxRoI != null && maxOverlap > USE_IOU_THRESHOLD) {
                mixedFrameResults
                        .get(new Pair<>(maxRoI.frame.sourceIP, maxRoI.frame.frameIndex))
                        .add(box.move(maxBoxPos));
            }
        }
        return mixedFrameResults;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private Map<Pair<String, Integer>, List<BoundingBox>> reconstructPerRoIInferenceResults(InferenceRequest request) {
        Map<Pair<String, Integer>, List<BoundingBox>> perRoIInferenceResults =
                createReconstructedResultHolder(request);
        for (RoI roi : request.rois) {
            for (BoundingBox box : roi.boundingBoxes) {
                Rect newLocation = new Rect(
                        box.location.left + roi.location.left,
                        box.location.top + roi.location.top,
                        box.location.right + roi.location.left,
                        box.location.bottom + roi.location.top
                );
                perRoIInferenceResults.get(new Pair<>(roi.frame.sourceIP, roi.frame.frameIndex)).add(box.move(newLocation));
            }
        }
        return perRoIInferenceResults;
    }

    private Map<Pair<String, Integer>, List<BoundingBox>> createReconstructedResultHolder(InferenceRequest request) {
        Map<Pair<String, Integer>, List<BoundingBox>> resultHolder = new HashMap<>();
        for (RoI roi : request.rois) {
            resultHolder.put(new Pair<>(roi.getSourceIP(), roi.getFrameIndex()), new ArrayList<>());
        }
        return resultHolder;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private void log(String sourceIP, int frameIndex, List<BoundingBox> boxes) {
        if (logWriter != null) {
            try {
                long timeStamp = System.nanoTime() - mStartTimeNs;
                logWriter.write(sourceIP + "," + frameIndex + "," + timeStamp + "," + boxes.stream().map(BoundingBox::toString).collect(Collectors.joining(",")) + "\n");
                logWriter.flush();
            } catch (IOException e) {
                Log.e(TAG, e.getMessage());
            }
        }
    }
}
