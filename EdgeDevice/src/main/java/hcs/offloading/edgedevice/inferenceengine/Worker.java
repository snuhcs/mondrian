package hcs.offloading.edgedevice.inferenceengine;

import android.util.Log;
import android.util.Pair;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import java.util.List;

import hcs.offloading.edgedevice.inferenceengine.model.Classifier;
import hcs.offloading.strm.datatypes.BoundingBox;

public class Worker implements Runnable {
    private static final String TAG = Worker.class.getName();

    private final Classifier model;
    private final Size inputSize;
    private final Classifier fullModel;
    private final Size fullInputSize;
    private final TFLiteInferenceEngine engine;

    private final Thread mWorkerThread;

    Worker(TFLiteInferenceEngine engine,
           Classifier model, int inputSize,
           Classifier fullModel, int fullInputSize) {
        this.engine = engine;
        this.model = model;
        this.inputSize = new Size(inputSize, inputSize);
        this.fullModel = fullModel;
        this.fullInputSize = new Size(fullInputSize, fullInputSize);

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
            Log.d(TAG, "Image   : " + image.width() + ", " + image.height() + ", " + image.channels() + ", " + image.type());
            byte[] original = new byte[3];
            image.get(0, 0, original);
            Log.d(TAG, "Original: " + original[0] + ", " + original[1] + ", " + original[2]);
            Mat processedBuffer = preprocess(image.clone(), isFull);
            List<BoundingBox> results = (isFull ? fullModel : model).recognizeImage(processedBuffer, image.width(), image.height());
            engine.enqueueResults(image, results);
        }
    }

    private Mat preprocess(Mat mat, boolean isFull) {
        Imgproc.resize(mat, mat, isFull ? fullInputSize : inputSize);
        byte[] original = new byte[3];
        mat.get(0, 0, original);
        Log.d(TAG, "Resized : " + original[0] + ", " + original[1] + ", " + original[2]);
        Mat floatMat = new Mat(mat.width(), mat.height(), CvType.CV_32FC3);
        mat.convertTo(floatMat, CvType.CV_32FC3, 1./255, 0);
        float[] f = new float[3];
        floatMat.get(0, 0, f);
        Log.d(TAG, "Float   : " + f[0] + ", " + f[1] + ", " + f[2]);
        floatMat = floatMat.reshape(1);
        floatMat.get(0, 0, f);
        Log.d(TAG, "Flatten : " + f[0] + ", " + f[1] + ", " + f[2]);
        return floatMat;
    }
}
