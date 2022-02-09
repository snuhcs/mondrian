package hcs.offloading.edgeserver.models;

import android.content.res.AssetManager;
import android.graphics.Bitmap;

import org.tensorflow.lite.support.image.TensorImage;

import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class MobileNetSSD extends DetectionModel {
    private static final String TAG = MobileNetSSD.class.getName();
    private static final int NUM_DETECTIONS = 10;
    private final float CONFIDENCE_THRESHOLD = 0.5f;

    // Output Buffer
    float[][][] outputLocations = new float[1][NUM_DETECTIONS][4];
    float[][] outputClasses = new float[1][NUM_DETECTIONS];
    float[][] outputScores = new float[1][NUM_DETECTIONS];
    float[] numDetections = new float[1];

    public MobileNetSSD(AssetManager assetManager) {
        super(assetManager, "mobile-net-ssd.tflite", "mobile-net-ssd.txt", 300);
    }

    @Override
    public List<BoundingBox> inference(TensorImage image) {
        // Prepare Input
        Bitmap resizedBitmap = mImageProcessor.process(image).getBitmap();
        ByteBuffer byteBuffer = convertBitmapToByteBuffer(resizedBitmap, true);
        Object[] inputArray = {byteBuffer};

        // Output Buffer
        Map<Integer, Object> outputMap = new HashMap<>();
        outputMap.put(0, outputLocations);
        outputMap.put(1, outputClasses);
        outputMap.put(2, outputScores);
        outputMap.put(3, numDetections);

        // Inference
        mInterpreter.runForMultipleInputsOutputs(inputArray, outputMap);

        // Process result
        List<BoundingBox> boundingBoxes = new LinkedList<>();
        int numBoxes = (int) numDetections[0];
        for (int i = 0; i < numBoxes; i++) {
            if (outputScores[0][i] > CONFIDENCE_THRESHOLD) {
                boundingBoxes.add(new BoundingBox(
                        outputLocations[0][i][1], // left
                        outputLocations[0][i][0], // top
                        outputLocations[0][i][3], // right
                        outputLocations[0][i][2], // bottom
                        outputScores[0][i], // confidence
                        (int) outputClasses[0][i] // label
                ));
            }
        }
        return boundingBoxes;
    }
}
