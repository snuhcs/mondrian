package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import org.tensorflow.lite.DataType;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.TensorImage;

import java.nio.ByteBuffer;
import java.util.List;

import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;

public class Worker implements Runnable {
    private static final String TAG = Worker.class.getName();

    public interface Callback {
        void enqueueInferenceResult(Pair<InferenceRequest, List<BoundingBox>> inferenceResult);
        void updateResult(Frame frame, List<BoundingBox> boxes);
        void updateInferenceOutputView(Bitmap result);
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
                InferenceRequest inferenceRequest = engine.getRequest();
                inferenceRequest.queueSize = engine.getRequestQueueSize();
                Frame inputFrame = inferenceRequest.frame;
                Bitmap inputImage = inputFrame.bitmap;

                startTime = System.nanoTime();
                ByteBuffer processedBuffer = preprocess(inputImage);
                endTime = System.nanoTime();
                float preprocessingTime = (endTime - startTime) / 1000000f;
                Log.v(TAG, "Preprocessing time: " + preprocessingTime);
                inferenceRequest.preprocessingTime = preprocessingTime;

                startTime = System.nanoTime();
                List<BoundingBox> results = model.recognizeImage(processedBuffer, inputImage);
                endTime = System.nanoTime();
                float inferenceTime = (endTime - startTime) / 1000000f;
                Log.v(TAG, "InferenceTime time: " + inferenceTime);
                inferenceRequest.inferenceTime = inferenceTime;

                List<BoundingBox> filteredResults = Utils.filterResults(results);
                mCallback.updateInferenceOutputView(Utils.drawResult(inputImage, YoloV4Classifier.nms(filteredResults)));

                if (inferenceRequest.frame.isMixedFrame()) {
                    mCallback.enqueueInferenceResult(new Pair<>(inferenceRequest, filteredResults));
                } else {
                    List<BoundingBox> nmsResult = YoloV4Classifier.nms(filteredResults);
                    Log.d(TAG, "Full Inference End: " + nmsResult.size());
                    mCallback.updateResult(inputFrame, nmsResult);
                }
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
