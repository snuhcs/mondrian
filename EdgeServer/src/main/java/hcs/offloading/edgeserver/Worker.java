package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.tensorflow.lite.DataType;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.TensorImage;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.edgeserver.datatypes.RoI;
import hcs.offloading.edgeserver.model.Classifier;

public class Worker implements Runnable {
    private static final String TAG = Worker.class.getName();

    public interface Callback {
        void enqueueInferenceResults(InferenceRequest request, List<BoundingBox> results) throws InterruptedException;
    }

    private final boolean PER_ROI_KEEP_RATIO;

    private final Classifier model;
    private final ImageProcessor preprocessor;
    private final Classifier fullModel;
    private final ImageProcessor fullPreprocessor;
    private final InferenceEngine engine;

    private final Thread mWorkerThread;
    private final Callback mCallback;

    public Worker(Callback callback, InferenceEngine engine,
                  Classifier model, ImageProcessor preprocessor,
                  Classifier fullModel, ImageProcessor fullPreprocessor,
                  boolean perRoIKeepRatio) {
        mCallback = callback;
        PER_ROI_KEEP_RATIO = perRoIKeepRatio;

        this.engine = engine;
        this.model = model;
        this.preprocessor = preprocessor;
        this.fullModel = fullModel;
        this.fullPreprocessor = fullPreprocessor;

        mWorkerThread = new Thread(this);
        mWorkerThread.start();
    }

    public void close() {
        try {
            mWorkerThread.interrupt();
            mWorkerThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "closed");
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void run() {
        try {
            while (true) {
                long startTime, endTime;
                InferenceRequest request = engine.getRequest();
                request.queueSize = engine.getRequestQueueSize();

                List<BoundingBox> results = new ArrayList<>();
                if (request.type == InferenceRequest.Type.MIXED && request.rois.isEmpty()) {
                } else if (request.type == InferenceRequest.Type.MIXED) {
                    Bitmap input = request.bitmap;

                    startTime = System.nanoTime();
                    ByteBuffer processedBuffer = preprocess(input);
                    endTime = System.nanoTime();
                    request.preprocessingTimeUs = (int) ((endTime - startTime) / 1e3);

                    startTime = System.nanoTime();
                    results = model.recognizeImage(processedBuffer, input.getWidth(), input.getHeight());
                    endTime = System.nanoTime();
                    request.inferenceTimeUs = (int) ((endTime - startTime) / 1e3);

                    results = Utils.filterPerson(results);
                } else if (request.type == InferenceRequest.Type.FULL) {
                    Bitmap input = request.bitmap;

                    startTime = System.nanoTime();
                    ByteBuffer processedBuffer = fullPreprocess(input);
                    endTime = System.nanoTime();
                    request.preprocessingTimeUs = (int) ((endTime - startTime) / 1e3);

                    startTime = System.nanoTime();
                    results = fullModel.recognizeImage(processedBuffer, input.getWidth(), input.getHeight());
                    endTime = System.nanoTime();
                    request.inferenceTimeUs = (int) ((endTime - startTime) / 1e3);

                    results = Utils.filterPerson(results);
                } else if (request.type == InferenceRequest.Type.PER_ROI) {
                    int preprocessingTimeUs = 0;
                    int inferenceTimeUs = 0;

                    for (RoI roi : request.rois) {
                        Bitmap rawInput = roi.getBitmap();
                        Bitmap input;
                        int width = rawInput.getWidth();
                        int height = rawInput.getHeight();
                        if (!PER_ROI_KEEP_RATIO) {
                            input = rawInput;
                        } else {
                            int largerEdge = Math.max(rawInput.getWidth(), rawInput.getHeight());
                            width = largerEdge;
                            height = largerEdge;
                            input = Bitmap.createBitmap(largerEdge, largerEdge, Bitmap.Config.ARGB_8888);
                            input.eraseColor(Color.BLACK);

                            Canvas canvas = new Canvas(input);
                            canvas.drawBitmap(rawInput, 0, 0, null);
                        }

                        startTime = System.nanoTime();
                        ByteBuffer processedBuffer = preprocess(input);
                        endTime = System.nanoTime();
                        preprocessingTimeUs += (int) ((endTime - startTime) / 1e3);

                        startTime = System.nanoTime();
                        List<BoundingBox> roiBoxes = model.recognizeImage(processedBuffer, width, height);
                        endTime = System.nanoTime();
                        inferenceTimeUs += (int) ((endTime - startTime) / 1e3);
                        roiBoxes = Utils.filterPerson(roiBoxes);
                        roi.setBoundingBoxes(roiBoxes);
                    }
                    request.preprocessingTimeUs = preprocessingTimeUs;
                    request.inferenceTimeUs = inferenceTimeUs;
                } else {
                    throw new IllegalArgumentException("Wrong request type! " + request.type);
                }
                mCallback.enqueueInferenceResults(request, results);

                //Log.v(TAG, "InferenceEngine Queue Size: " + request.queueSize);
                //Log.v(TAG, "Preprocessing time (us): " + request.preprocessingTimeUs);
                //Log.v(TAG, "InferenceTime time (us): " + request.inferenceTimeUs);
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    private ByteBuffer preprocess(Bitmap bitmap) {
        TensorImage image = new TensorImage(DataType.UINT8);
        image.load(bitmap);
        TensorImage processedImage = preprocessor.process(image);
        return processedImage.getBuffer();
    }

    private ByteBuffer fullPreprocess(Bitmap bitmap) {
        TensorImage image = new TensorImage(DataType.UINT8);
        image.load(bitmap);
        TensorImage processedImage = fullPreprocessor.process(image);
        return processedImage.getBuffer();
    }
}
