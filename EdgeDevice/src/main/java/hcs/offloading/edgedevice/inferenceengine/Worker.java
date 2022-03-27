package hcs.offloading.edgedevice.inferenceengine;

import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.tensorflow.lite.DataType;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.TensorImage;

import java.nio.ByteBuffer;
import java.util.List;

import hcs.offloading.edgedevice.inferenceengine.model.Classifier;
import hcs.offloading.strm.datatypes.BoundingBox;

public class Worker implements Runnable {
    private static final String TAG = Worker.class.getName();

    private final Classifier model;
    private final ImageProcessor preprocessor;
    private final TFLiteInferenceEngine engine;

    private final Thread mWorkerThread;

    Worker(TFLiteInferenceEngine engine, Classifier model, ImageProcessor preprocessor) {
        this.engine = engine;
        this.model = model;
        this.preprocessor = preprocessor;

        mWorkerThread = new Thread(this);
        mWorkerThread.start();
    }

    void close() {
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
        while (true) {
            Bitmap input = engine.getInput();
            ByteBuffer processedBuffer = preprocess(input);
            List<BoundingBox> results = model.recognizeImage(processedBuffer, input.getWidth(), input.getHeight());
            engine.enqueueResults(input, results);
        }
    }

    private ByteBuffer preprocess(Bitmap bitmap) {
        TensorImage image = new TensorImage(DataType.UINT8);
        image.load(bitmap);
        TensorImage processedImage = preprocessor.process(image);
        return processedImage.getBuffer();
    }
}
