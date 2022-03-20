package hcs.offloading.edgedevice;

import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.stream.Collectors;

import hcs.offloading.edgedevice.config.PatchReconstructorConfig;
import hcs.offloading.edgedevice.datatypes.BoundingBox;
import hcs.offloading.edgedevice.datatypes.Frame;
import hcs.offloading.edgedevice.datatypes.InferenceRequest;
import hcs.offloading.edgedevice.datatypes.RoI;

public class PatchReconstructor implements Runnable {
    private static final String TAG = PatchReconstructor.class.getName();

    private final int MATCH_PADDING;
    private final float USE_IOU_THRESHOLD;
    private final float DRAW_CONFIDENCE;

    public interface Callback {
        void enqueueResults(String sourceIP, int frameIndex, List<BoundingBox> result);

        void enqueueResults(Map<String, Map<Integer, List<BoundingBox>>> reconstructedFrameResults);
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
            log(request.sourceIP, request.frameIndex, results);
            mCallback.enqueueResults(request.sourceIP, request.frameIndex, results);
        } else {
            throw new IllegalArgumentException("Wrong request type! " + request.type);
        }
        if (request.type == InferenceRequest.Type.MIXED || request.type == InferenceRequest.Type.FULL) {
            mViewCallback.drawInferenceResult(Utils.drawBoxes(request.bitmap.copy(request.bitmap.getConfig(), true), results, DRAW_CONFIDENCE));
        }
        request.bitmap.recycle();
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
                Map<String, Map<Integer, List<BoundingBox>>> reconstructedFrameResults;
                if (request.type == InferenceRequest.Type.MIXED) {
                    reconstructedFrameResults = reconstructMixedFrameResults(request, results);
                } else if (request.type == InferenceRequest.Type.PER_ROI) {
                    reconstructedFrameResults = reconstructPerRoIInferenceResults(request);
                } else {
                    throw new IllegalArgumentException("Wrong request type! " + request.type);
                }
                endTime = System.nanoTime();
                Log.v(TAG, "Reconstructing time (us): " + (endTime - startTime) / 1e3);

                mCallback.enqueueResults(reconstructedFrameResults);

                request.rois.stream()
                        .max(Comparator.comparingInt(RoI::getFrameIndex))
                        .ifPresent(lastRoI -> {
                            List<BoundingBox> r = reconstructedFrameResults.get(lastRoI.frame.sourceIP).get(lastRoI.frame.frameIndex);
                            StringBuffer str = new StringBuffer();
                            for (BoundingBox box : r) {
                                str.append(box.location.left + "," + box.location.top + "," + box.location.right + "," + box.location.bottom + " ");
                            }
                            Log.v(TAG, "Draw boxes: " + str);
                            mViewCallback.drawObjectDetectionResult(Utils.drawBoxes(
                                    lastRoI.frame.bitmap.copy(lastRoI.frame.bitmap.getConfig(), true),
                                    r,
                                    DRAW_CONFIDENCE));
                        });
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    private Map<String, Map<Integer, List<BoundingBox>>> reconstructMixedFrameResults(InferenceRequest request, List<BoundingBox> boxes) {
        Map<String, Map<Integer, List<BoundingBox>>> mixedFrameResults = createReconstructedResultHolder(request);

        List<RoI> rois = request.rois;
        for (BoundingBox box : boxes) {
            float maxOverlap = -1f;
            RoI maxRoI = null;
            Rect maxBoxPos = null;
            for (RoI roi : rois) {
                Rect paddedRoIPos = new Rect(
                        Math.max(0, roi.location.left - MATCH_PADDING),
                        Math.max(0, roi.location.top - MATCH_PADDING),
                        Math.min(roi.frame.bitmap.getWidth(), roi.location.right + MATCH_PADDING),
                        Math.min(roi.frame.bitmap.getHeight(), roi.location.bottom + MATCH_PADDING)
                );
                Rect boxPos = box.location;
                Rect movedAndResizedBoxPos = new Rect(
                        Math.max(0, (int) ((boxPos.left - roi.packedLocation[0]) / roi.scale) + roi.location.left),
                        Math.max(0, (int) ((boxPos.top - roi.packedLocation[1]) / roi.scale) + roi.location.top),
                        Math.min(roi.frame.bitmap.getWidth(), (int) ((boxPos.right - roi.packedLocation[0]) / roi.scale) + roi.location.left),
                        Math.min(roi.frame.bitmap.getHeight(), (int) ((boxPos.bottom - roi.packedLocation[1]) / roi.scale) + roi.location.top)
                );
                float intersection = Utils.box_intersection(paddedRoIPos, movedAndResizedBoxPos);
                float overlapRatio = Math.max(
                        intersection / (movedAndResizedBoxPos.width() * movedAndResizedBoxPos.height()),
                        intersection / (paddedRoIPos.width() * paddedRoIPos.height()));
                if (maxRoI == null || maxOverlap < overlapRatio) {
                    maxOverlap = overlapRatio;
                    maxRoI = roi;
                    maxBoxPos = movedAndResizedBoxPos;
                }
            }
            if (maxRoI != null && maxOverlap > USE_IOU_THRESHOLD) {
                mixedFrameResults
                        .get(maxRoI.frame.sourceIP)
                        .get(maxRoI.frame.frameIndex)
                        .add(box.move(maxBoxPos));
            }
        }
        return mixedFrameResults;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private Map<String, Map<Integer, List<BoundingBox>>> reconstructPerRoIInferenceResults(InferenceRequest request) {
        Map<String, Map<Integer, List<BoundingBox>>> perRoIInferenceResults =
                createReconstructedResultHolder(request);
        for (RoI roi : request.rois) {
            for (BoundingBox box : roi.boundingBoxes) {
                Rect newLocation = new Rect(
                        box.location.left + roi.location.left,
                        box.location.top + roi.location.top,
                        box.location.right + roi.location.left,
                        box.location.bottom + roi.location.top
                );
                perRoIInferenceResults.get(roi.frame.sourceIP).get(roi.frame.frameIndex).add(box.move(newLocation));
            }
        }
        return perRoIInferenceResults;
    }

    private Map<String, Map<Integer, List<BoundingBox>>> createReconstructedResultHolder(InferenceRequest request) {
        Map<String, Map<Integer, List<BoundingBox>>> resultHolder = new HashMap<>();
        for (Frame frame : request.frames) {
            if (!resultHolder.containsKey(frame.sourceIP)) {
                resultHolder.put(frame.sourceIP, new HashMap<>());
            }
            resultHolder.get(frame.sourceIP).put(frame.frameIndex, new ArrayList<>());
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
