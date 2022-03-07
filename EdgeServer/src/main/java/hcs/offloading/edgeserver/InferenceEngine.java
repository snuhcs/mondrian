package hcs.offloading.edgeserver;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.widget.ImageView;

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

    public final int MIXED_FRAME_SIZE;
    public final int NUM_WORKERS;

    private final List<Worker> mWorkers = new ArrayList<>();
    private final LinkedBlockingQueue<InferenceRequest> mInferenceRequests = new LinkedBlockingQueue<>();

    private final ImageView mOutputView;

    @RequiresApi(api = Build.VERSION_CODES.N)
    InferenceEngine(InferenceEngineConfig config, AssetManager assetManager, EdgeServer edgeServer, PatchReconstructor patchReconstructor, ImageView outputView) {
        MIXED_FRAME_SIZE = config.MIXED_FRAME_SIZE;
        NUM_WORKERS = config.NUM_WORKERS;
  
        mOutputView = outputView;

        for (int workerId = 0; workerId < NUM_WORKERS; workerId++) {
            mWorkers.add(new Worker(
                    new YoloV4Classifier(assetManager, MIXED_FRAME_SIZE),
                    new ImageProcessor.Builder()
                            .add(new ResizeOp(MIXED_FRAME_SIZE, MIXED_FRAME_SIZE, ResizeOp.ResizeMethod.BILINEAR))
                            .add(new NormalizeOp(0.0f, 255.0f))
                            .build(),
                    edgeServer,
                    patchReconstructor,
                    this));
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

    public void updateOutputView(Bitmap result) {
        mOutputView.post(() -> mOutputView.setImageBitmap(result));
    }
}
