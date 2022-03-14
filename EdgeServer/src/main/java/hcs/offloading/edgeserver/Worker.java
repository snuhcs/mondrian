package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
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

public class Worker implements Runnable {
    private static final String TAG = Worker.class.getName();

    public interface Callback {
        void enqueueInferenceResult(InferenceRequest request, List<BoundingBox> results);
    }

    private final YoloV4Classifier model;
    private final ImageProcessor preprocessor;
    private final InferenceEngine engine;

    private final Thread mWorkerThread;
    private final Callback mCallback;

    public Worker(Callback callback, InferenceEngine engine, YoloV4Classifier model, ImageProcessor preprocessor) {
        mCallback = callback;

        this.engine = engine;
        this.model = model;
        this.preprocessor = preprocessor;

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
                if (request.type == InferenceRequest.Type.MIXED ||
                        request.type == InferenceRequest.Type.FULL) {
                    Bitmap input = request.frame.bitmap;

                    startTime = System.nanoTime();
                    ByteBuffer processedBuffer = preprocess(input);
                    endTime = System.nanoTime();
                    request.preprocessingTimeUs = (int) ((endTime - startTime) / 1e3);

                    startTime = System.nanoTime();
                    results = model.recognizeImage(processedBuffer, input);
                    endTime = System.nanoTime();
                    request.inferenceTimeUs = (int) ((endTime - startTime) / 1e3);

                    results = Utils.filterPerson(results);
                } else if (request.type == InferenceRequest.Type.PER_ROI) {
                    int preprocessingTimeUs = 0;
                    int inferenceTimeUs = 0;

                    for (RoI roi : request.rois) {
                        Bitmap input = roi.getBitmap();
                        startTime = System.nanoTime();
                        ByteBuffer processedBuffer = preprocess(input);
                        endTime = System.nanoTime();
                        preprocessingTimeUs += (int) ((endTime - startTime) / 1e3);

                        startTime = System.nanoTime();
                        List<BoundingBox> roiBoxes = model.recognizeImage(processedBuffer, input);
                        endTime = System.nanoTime();
                        inferenceTimeUs += (int) ((endTime - startTime) / 1e3);
                        roiBoxes = Utils.filterPerson(roiBoxes);
                        roi.setBoundingBoxes(roiBoxes);
                        results.addAll(roiBoxes);
                    }
                    request.preprocessingTimeUs = preprocessingTimeUs;
                    request.inferenceTimeUs = inferenceTimeUs;
                } else {
                    throw new IllegalArgumentException("Wrong request type! " + request.type);
                }
                mCallback.enqueueInferenceResult(request, results);

                // Log.v(TAG, "InferenceEngine Queue Size: " + request.queueSize);
                // Log.v(TAG, "Preprocessing time (us): " + request.preprocessingTimeUs);
                // Log.v(TAG, "InferenceTime time (us): " + request.inferenceTimeUs);
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
}
