package hcs.offloading.edgedevice;

import android.content.res.AssetManager;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.tensorflow.lite.support.common.ops.NormalizeOp;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.ops.ResizeOp;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

import hcs.offloading.edgedevice.config.InferenceEngineConfig;
import hcs.offloading.edgedevice.datatypes.InferenceRequest;
import hcs.offloading.edgedevice.model.Classifier;
import hcs.offloading.edgedevice.model.YoloV4Classifier;
import hcs.offloading.edgedevice.model.YoloV5Classifier;

public class InferenceEngine {
    private final static String TAG = InferenceEngine.class.getName();

    public final int FRAME_SIZE;
    public final int FULL_FRAME_SIZE;
    public final int NUM_WORKERS;
    public final int MAX_QUEUED_REQUESTS;

    private final List<Worker> mWorkers = new ArrayList<>();
    private final Queue<InferenceRequest> mInferenceRequests = new LinkedList<>();

    @RequiresApi(api = Build.VERSION_CODES.N)
    InferenceEngine(InferenceEngineConfig config, Worker.Callback callback, AssetManager assetManager) {
        FRAME_SIZE = config.FRAME_SIZE;
        FULL_FRAME_SIZE = config.FULL_FRAME_SIZE;
        NUM_WORKERS = config.NUM_WORKERS;
        MAX_QUEUED_REQUESTS = NUM_WORKERS * 2;

        Classifier model = config.USE_YOLO_V4 ?
                new YoloV4Classifier(assetManager, FRAME_SIZE, config.USE_TINY) :
                new YoloV5Classifier(assetManager, FRAME_SIZE);
        ImageProcessor processor = new ImageProcessor.Builder()
                .add(new ResizeOp(FRAME_SIZE, FRAME_SIZE, ResizeOp.ResizeMethod.BILINEAR))
                .add(new NormalizeOp(0.0f, 255.0f))
                .build();

        Classifier fullModel;
        ImageProcessor fullProcessor;
        if (FRAME_SIZE != FULL_FRAME_SIZE) {
            fullModel = config.USE_YOLO_V4 ?
                    new YoloV4Classifier(assetManager, FULL_FRAME_SIZE, config.USE_TINY) :
                    new YoloV5Classifier(assetManager, FULL_FRAME_SIZE);
            fullProcessor = new ImageProcessor.Builder()
                    .add(new ResizeOp(FULL_FRAME_SIZE, FULL_FRAME_SIZE, ResizeOp.ResizeMethod.BILINEAR))
                    .add(new NormalizeOp(0.0f, 255.0f))
                    .build();
        } else {
            fullModel = model;
            fullProcessor = processor;
        }

        for (int workerId = 0; workerId < NUM_WORKERS; workerId++) {
            mWorkers.add(new Worker(callback, this,
                    model, processor, fullModel, fullProcessor,
                    config.PER_ROI_KEEP_RATIO));
        }
    }

    public void close() {
        for (Worker worker : mWorkers) {
            worker.close();
        }
        mWorkers.clear();
        synchronized (mInferenceRequests) {
            mInferenceRequests.clear();
        }
        Log.d(TAG, "closed");
    }

    public void enqueueRequest(InferenceRequest request) {
        //Log.v(TAG, "Start enqueueRequest() : " + request.type);
        try {
            synchronized (mInferenceRequests) {
                while (MAX_QUEUED_REQUESTS > 0 && mInferenceRequests.size() > MAX_QUEUED_REQUESTS) {
                    mInferenceRequests.wait();
                }
                mInferenceRequests.add(request);
                mInferenceRequests.notifyAll();
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        //Log.v(TAG, "End enqueueRequest() : " + request.type);
    }

    public InferenceRequest getRequest() throws InterruptedException {
        ////Log.v(TAG, "Start getRequest()");
        InferenceRequest request;
        synchronized (mInferenceRequests) {
            while (mInferenceRequests.size() == 0) {
                mInferenceRequests.wait();
            }
            request = mInferenceRequests.poll();
            mInferenceRequests.notifyAll();
        }
        ////Log.v(TAG, "End getRequest()");
        return request;
    }

    public int getRequestQueueSize() {
        synchronized (mInferenceRequests) {
            return mInferenceRequests.size();
        }
    }
}
