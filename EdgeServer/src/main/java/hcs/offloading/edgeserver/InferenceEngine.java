package hcs.offloading.edgeserver;

import android.content.res.AssetManager;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.tensorflow.lite.support.common.ops.NormalizeOp;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.ops.ResizeOp;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;

import hcs.offloading.edgeserver.config.InferenceEngineConfig;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;

public class InferenceEngine {
    private final static String TAG = InferenceEngine.class.getName();

    public final int FRAME_SIZE;
    public final int NUM_WORKERS;

    private final List<Worker> mWorkers = new ArrayList<>();
    private final LinkedBlockingQueue<InferenceRequest> mInferenceRequests = new LinkedBlockingQueue<>();

    @RequiresApi(api = Build.VERSION_CODES.N)
    InferenceEngine(InferenceEngineConfig config, Worker.Callback callback, AssetManager assetManager) {
        FRAME_SIZE = config.FRAME_SIZE;
        NUM_WORKERS = config.NUM_WORKERS;

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
        mInferenceRequests.clear();
        Log.d(TAG, "closed");
    }

    public void enqueueRequest(InferenceRequest inferenceRequest) {
        mInferenceRequests.add(inferenceRequest);
    }

    public InferenceRequest getRequest() throws InterruptedException {
        return mInferenceRequests.take();
    }

    public int getRequestQueueSize() {
        return mInferenceRequests.size();
    }
}
