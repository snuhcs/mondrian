package hcs.offloading.edgedevice.inferenceengine;

import android.graphics.Bitmap;
import android.util.Log;
import android.util.Pair;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
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
    private final Classifier fullModel;
    private final ImageProcessor fullPreprocessor;
    private final TFLiteInferenceEngine engine;

    private final Thread mWorkerThread;

    Worker(TFLiteInferenceEngine engine,
           Classifier model, ImageProcessor preprocessor,
           Classifier fullModel, ImageProcessor fullPreprocessor) {
        this.engine = engine;
        this.model = model;
        this.preprocessor = preprocessor;
        this.fullModel = fullModel;
        this.fullPreprocessor = fullPreprocessor;

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

    @Override
    public void run() {
        while (true) {
            Pair<Mat, Boolean> input = engine.getInput();
            Mat image = input.first;
            boolean isFull = input.second;
            ByteBuffer processedBuffer = preprocess(image.clone(), isFull);
            List<BoundingBox> results = (isFull ? fullModel : model).recognizeImage(processedBuffer, image.width(), image.height());
            engine.enqueueResults(image, results);
        }
    }

    private ByteBuffer preprocess(Mat mat, boolean isFull) {
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        TensorImage image = new TensorImage(DataType.UINT8);
        image.load(bitmap);
        return (isFull ? fullPreprocessor : preprocessor).process(image).getBuffer();
    }
}
