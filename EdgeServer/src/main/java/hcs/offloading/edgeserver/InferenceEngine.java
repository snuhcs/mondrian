package hcs.offloading.edgeserver;

import android.annotation.SuppressLint;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import org.tensorflow.lite.support.common.ops.NormalizeOp;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.ops.ResizeOp;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;

import hcs.offloading.edgeserver.datatypes.InferenceRequest;

public class InferenceEngine {
    private final static String TAG = InferenceEngine.class.getName();

    public static final int MIXED_FRAME_SIZE = 640;
    public static final int NUM_WORKERS = 1;

    private final List<Worker> mWorkers = new ArrayList<>();
    private final LinkedBlockingQueue<InferenceRequest> mInferenceRequests = new LinkedBlockingQueue<>();

    private final ImageView mOutputView;
    private final TextView mFpsView;

    private int mNumProcessedFrames = 0;
    private long mApplicationStartTime = 0;

    @RequiresApi(api = Build.VERSION_CODES.N)
    InferenceEngine(AssetManager assetManager, EdgeServer edgeServer, ImageView outputView, TextView fpsView) {
        mOutputView = outputView;
        mFpsView = fpsView;

        mNumProcessedFrames = 0;
        mApplicationStartTime = System.nanoTime();

        for (int workerId = 0; workerId < NUM_WORKERS; workerId++) {
            mWorkers.add(new Worker(
                    new YoloV4Classifier(assetManager, MIXED_FRAME_SIZE),
                    new ImageProcessor.Builder()
                            .add(new ResizeOp(MIXED_FRAME_SIZE, MIXED_FRAME_SIZE, ResizeOp.ResizeMethod.BILINEAR))
                            .add(new NormalizeOp(0.0f, 255.0f))
                            .build(),
                    edgeServer,
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

    @SuppressLint({"DefaultLocale", "SetTextI18n"})
    public void updateFPS(int numProcessedFrames) {
        mNumProcessedFrames += numProcessedFrames;
        float fps = mNumProcessedFrames / ((System.nanoTime() - mApplicationStartTime) / 1000000000f);
        mFpsView.post(() -> mFpsView.setText(String.format("%.3f", fps)));
//        mFpsView.post(() -> mFpsView.setText(Integer.toString(mInferenceRequests.size())));
    }
}
