package hcs.offloading.edgedevice.inferenceengine;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.tensorflow.lite.support.common.ops.NormalizeOp;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.ops.ResizeOp;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;

import hcs.offloading.edgedevice.inferenceengine.model.Classifier;
import hcs.offloading.edgedevice.inferenceengine.model.YoloV4Classifier;
import hcs.offloading.edgedevice.inferenceengine.model.YoloV5Classifier;
import hcs.offloading.strm.InferenceEngine;
import hcs.offloading.edgedevice.InferenceEngineConfig;
import hcs.offloading.strm.datatypes.BoundingBox;

public class TFLiteInferenceEngine implements InferenceEngine {
    private final static String TAG = TFLiteInferenceEngine.class.getName();

    private final LinkedBlockingQueue<Bitmap> mInputs;
    private final Map<Integer, List<BoundingBox>> mResults = new HashMap<>();
    private final List<Worker> mWorkers = new ArrayList<>();

    @RequiresApi(api = Build.VERSION_CODES.N)
    public TFLiteInferenceEngine(InferenceEngineConfig config, int frameSize, AssetManager assetManager) {
        Classifier model;
        if (config.MODEL == InferenceEngineConfig.Model.YOLO_V4) {
            model = new YoloV4Classifier(assetManager, frameSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);
        } else if (config.MODEL == InferenceEngineConfig.Model.YOLO_V5) {
            model = new YoloV5Classifier(assetManager, frameSize, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);
        } else {
            throw new IllegalArgumentException("Wrong model type: " + config.MODEL);
        }
        ImageProcessor processor = new ImageProcessor.Builder()
                .add(new ResizeOp(frameSize, frameSize, ResizeOp.ResizeMethod.BILINEAR))
                .add(new NormalizeOp(0.0f, 255.0f))
                .build();

        mInputs = new LinkedBlockingQueue<>(config.NUM_WORKERS * 2);
        for (int workerId = 0; workerId < config.NUM_WORKERS; workerId++) {
            mWorkers.add(new Worker(this, model, processor));
        }
    }

    @Override
    public int enqueue(Bitmap bitmap) {
        int key = bitmap.hashCode();
        try {
            mInputs.put(bitmap);
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        return key;
    }

    Bitmap getInput() {
        try {
            return mInputs.take();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        return null;
    }

    void enqueueResults(Bitmap bitmap, List<BoundingBox> results) {
        synchronized (mResults) {
            mResults.put(bitmap.hashCode(), results);
            mResults.notifyAll();
        }
    }

    @Override
    public List<BoundingBox> getResults(int key) {
        List<BoundingBox> results = null;
        try {
            synchronized (mResults) {
                while (!mResults.containsKey(key)) {
                    mResults.wait();
                }
                results = mResults.get(key);
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        return results;
    }

    public void close() {
        for (Worker worker : mWorkers) {
            worker.close();
        }
        Log.d(TAG, "closed");
    }
}
