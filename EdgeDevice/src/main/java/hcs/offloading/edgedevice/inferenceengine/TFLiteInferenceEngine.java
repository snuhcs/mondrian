package hcs.offloading.edgedevice.inferenceengine;

import android.content.res.AssetManager;
import android.util.Log;
import android.util.Pair;

import org.opencv.core.Mat;
import org.tensorflow.lite.support.image.ImageProcessor;

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

    private final LinkedBlockingQueue<Pair<Mat, Boolean>> mInputs;
    private final Map<Integer, List<BoundingBox>> mResults = new HashMap<>();
    private final List<Worker> mWorkers = new ArrayList<>();

    private final ResultCallback mResultCallback;

    public TFLiteInferenceEngine(InferenceEngineConfig config, AssetManager assetManager, ResultCallback resultCallback) {
        mResultCallback = resultCallback;
        Classifier model = getModel(config.MODEL, assetManager, config.INPUT_SIZE, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);

        Classifier fullModel;
        ImageProcessor fullPreprocessor;
        if (config.FULL_FRAME_INPUT_SIZE == config.INPUT_SIZE) {
            fullModel = model;
        } else {
            fullModel = getModel(config.MODEL, assetManager, config.FULL_FRAME_INPUT_SIZE, config.CONF_THRESHOLD, config.IOU_THRESHOLD, config.USE_TINY);
        }

        mInputs = new LinkedBlockingQueue<>(config.NUM_WORKERS * 2);
        for (int workerId = 0; workerId < config.NUM_WORKERS; workerId++) {
            mWorkers.add(new Worker(this, model, config.INPUT_SIZE, fullModel, config.FULL_FRAME_INPUT_SIZE));
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
    public int enqueue(Mat mat, boolean isFull) throws InterruptedException {
        int key = mat.hashCode();
        mInputs.put(new Pair<>(mat, isFull));
        return key;
    }

    Pair<Mat, Boolean> getInput() {
        try {
            return mInputs.take();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        return null;
    }

    void enqueueResults(Mat mat, List<BoundingBox> results) {
        mResultCallback.drawInferenceResult(STRMUtils.drawBoxes(mat.clone(), results, 0.1f));
        synchronized (mResults) {
            mResults.put(mat.hashCode(), results);
            mResults.notifyAll();
        }
    }

    @Override
    public List<BoundingBox> getResults(int key) throws InterruptedException {
        List<BoundingBox> results;
        synchronized (mResults) {
            results = mResults.get(key);
            while (results == null) {
                mResults.wait();
                results = mResults.get(key);
            }
        }
        return results.stream()
                .filter(box -> box.labelName.equals("person"))
                .collect(Collectors.toList());
    }

    public void close() {
        for (Worker worker : mWorkers) {
            worker.close();
        }
        Log.d(TAG, "closed");
    }
}
