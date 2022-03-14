package hcs.offloading.edgeserver;

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

import hcs.offloading.edgeserver.config.InferenceEngineConfig;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;

public class InferenceEngine {
    private final static String TAG = InferenceEngine.class.getName();

    public final int FRAME_SIZE;
    public final int NUM_WORKERS;
    public final int MAX_QUEUED_REQUESTS;

    private final List<Worker> mWorkers = new ArrayList<>();
    private final Queue<InferenceRequest> mInferenceRequests = new LinkedList<>();

    @RequiresApi(api = Build.VERSION_CODES.N)
    InferenceEngine(InferenceEngineConfig config, Worker.Callback callback, AssetManager assetManager) {
        FRAME_SIZE = config.FRAME_SIZE;
        NUM_WORKERS = config.NUM_WORKERS;
        MAX_QUEUED_REQUESTS = config.MAX_QUEUED_REQUESTS;

        for (int workerId = 0; workerId < NUM_WORKERS; workerId++) {
            mWorkers.add(new Worker(callback, this,
                    new YoloV4Classifier(assetManager, FRAME_SIZE),
                    new ImageProcessor.Builder()
                            .add(new ResizeOp(FRAME_SIZE, FRAME_SIZE, ResizeOp.ResizeMethod.BILINEAR))
                            .add(new NormalizeOp(0.0f, 255.0f))
                            .build()));
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

    public void enqueueRequest(InferenceRequest inferenceRequest) {
        try {
            synchronized (mInferenceRequests) {
                while (MAX_QUEUED_REQUESTS > 0 && mInferenceRequests.size() > MAX_QUEUED_REQUESTS) {
                    mInferenceRequests.wait();
                }
                mInferenceRequests.add(inferenceRequest);
                mInferenceRequests.notifyAll();
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public InferenceRequest getRequest() throws InterruptedException {
        synchronized (mInferenceRequests) {
            while (mInferenceRequests.size() == 0) {
                mInferenceRequests.wait();
            }
            InferenceRequest request = mInferenceRequests.poll();
            mInferenceRequests.notifyAll();
            return request;
        }
    }

    public int getRequestQueueSize() {
        return mInferenceRequests.size();
    }
}
