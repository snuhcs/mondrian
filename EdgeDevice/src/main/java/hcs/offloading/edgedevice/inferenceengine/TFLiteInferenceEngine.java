package hcs.offloading.edgedevice.inferenceengine;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import org.tensorflow.lite.support.common.ops.NormalizeOp;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.ops.ResizeOp;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.stream.Collectors;

import hcs.offloading.edgedevice.ResultCallback;
import hcs.offloading.edgedevice.config.InferenceEngineConfig;
import hcs.offloading.edgedevice.inferenceengine.model.Classifier;
import hcs.offloading.edgedevice.inferenceengine.model.YoloV4Classifier;
import hcs.offloading.edgedevice.inferenceengine.model.YoloV5Classifier;
import hcs.offloading.strm.InferenceEngine;
import hcs.offloading.strm.STRMUtils;
import hcs.offloading.strm.datatypes.BoundingBox;

public class TFLiteInferenceEngine implements InferenceEngine {
    private final static String TAG = TFLiteInferenceEngine.class.getName();

    private final LinkedBlockingQueue<Pair<Bitmap, Boolean>> mInputs;
    private final Map<Integer, List<BoundingBox>> mResults = new HashMap<>();
    private final List<Worker> mWorkers = new ArrayList<>();

    private final ResultCallback mResultCallback;

    @RequiresApi(api = Build.VERSION_CODES.N)
    public TFLiteInferenceEngine(InferenceEngineConfig config, AssetManager assetManager, ResultCallback resultCallback) {
        mResultCallback = resultCallback;
        Classifier model = getModel(config.MODEL, assetManager, config.INPUT_SIZE, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);
        ImageProcessor preprocessor = new ImageProcessor.Builder()
                .add(new ResizeOp(config.INPUT_SIZE, config.INPUT_SIZE, ResizeOp.ResizeMethod.BILINEAR))
                .add(new NormalizeOp(0.0f, 255.0f))
                .build();

        Classifier fullModel;
        ImageProcessor fullPreprocessor;
        if (config.FULL_FRAME_INPUT_SIZE == config.INPUT_SIZE) {
            fullModel = model;
            fullPreprocessor = preprocessor;
        } else {
            fullModel = getModel(config.MODEL, assetManager, config.FULL_FRAME_INPUT_SIZE, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);
            fullPreprocessor = new ImageProcessor.Builder()
                    .add(new ResizeOp(config.FULL_FRAME_INPUT_SIZE, config.FULL_FRAME_INPUT_SIZE, ResizeOp.ResizeMethod.BILINEAR))
                    .add(new NormalizeOp(0.0f, 255.0f))
                    .build();
        }

        mInputs = new LinkedBlockingQueue<>(config.NUM_WORKERS * 2);
        for (int workerId = 0; workerId < config.NUM_WORKERS; workerId++) {
            mWorkers.add(new Worker(this, model, preprocessor, fullModel, fullPreprocessor));
        }
    }

    private Classifier getModel(InferenceEngineConfig.Model model, AssetManager assetManager, int inputSize, float confThreshold, float iouThreshold, boolean useTiny) {
        if (model == InferenceEngineConfig.Model.YOLO_V4) {
            return new YoloV4Classifier(assetManager, inputSize, confThreshold, iouThreshold, useTiny);
        } else if (model == InferenceEngineConfig.Model.YOLO_V5) {
            return new YoloV5Classifier(assetManager, inputSize, confThreshold, iouThreshold, useTiny);
        } else {
            throw new IllegalArgumentException("Wrong model type: " + model);
        }
    }

    @Override
    public int enqueue(Bitmap bitmap, boolean isFull) throws InterruptedException {
        int key = bitmap.hashCode();
        mInputs.put(new Pair<>(bitmap, isFull));
        return key;
    }

    Pair<Bitmap, Boolean> getInput() {
        try {
            return mInputs.take();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        return null;
    }

    void enqueueResults(Bitmap bitmap, List<BoundingBox> results) {
        mResultCallback.drawInferenceResult(STRMUtils.drawBoxes(bitmap.copy(bitmap.getConfig(), true), results, 0.1f));
        synchronized (mResults) {
            mResults.put(bitmap.hashCode(), results);
            mResults.notifyAll();
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public List<BoundingBox> getResults(int key) throws InterruptedException {
        List<BoundingBox> results;
        synchronized (mResults) {
            while (!mResults.containsKey(key)) {
                mResults.wait();
            }
            results = mResults.get(key);
        }
        return results.stream().filter(box -> box.labelName.equals("person")).collect(Collectors.toList());
    }

    public void close() {
        for (Worker worker : mWorkers) {
            worker.close();
        }
        Log.d(TAG, "closed");
    }
}
