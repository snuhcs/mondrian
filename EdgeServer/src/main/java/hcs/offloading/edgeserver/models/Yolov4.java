package hcs.offloading.edgeserver.models;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.RectF;
import android.util.Log;

import org.tensorflow.lite.support.image.TensorImage;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Yolov4 extends DetectionModel {
    private static final String TAG = Yolov4.class.getName();
    private static final int NUM_DETECTIONS = 2535;
    private static final float OBJECT_THRESHOLD = 0.2f;
    float[][][] outputLocations;
    float[][][] outputConfidences;

    public Yolov4(AssetManager assetManager) {
        super(assetManager, "yolov4-tiny-416.tflite", "coco.txt", 416);
        outputLocations = new float[1][NUM_DETECTIONS][4];
        outputConfidences = new float[1][NUM_DETECTIONS][mLabelList.size()];
    }

    @Override
    public List<BoundingBox> inference(TensorImage image) {
        // Prepare Input
        Bitmap resizedBitmap = mImageProcessor.process(image).getBitmap();
        ByteBuffer byteBuffer = convertBitmapToByteBuffer(resizedBitmap, false);
        Object[] inputArray = {byteBuffer};

        // Output Buffer
        Map<Integer, Object> outputMap = new HashMap<>();
        outputMap.put(0, outputLocations);
        outputMap.put(1, outputConfidences);

        // Inference
        mInterpreter.runForMultipleInputsOutputs(inputArray, outputMap);

        // Process result
        float maxMaxConfidence = 0f;
        List<BoundingBox> boundingBoxes = new ArrayList<>();
        for (int i = 0; i < NUM_DETECTIONS; i++) {
            float maxConfidence = 0;
            int detectedClass = -1;
            final float[] confidences = new float[mLabelList.size()];
            for (int c = 0; c < mLabelList.size(); c++) {
                confidences[c] = outputConfidences[0][i][c];
            }
            for (int c = 0; c < mLabelList.size(); ++c) {
                if (confidences[c] > maxConfidence) {
                    detectedClass = c;
                    maxConfidence = confidences[c];
                }
            }
            if (maxMaxConfidence < maxConfidence) {
                maxMaxConfidence = maxConfidence;
            }
            if (maxConfidence > OBJECT_THRESHOLD) {
                final float xPos = outputLocations[0][i][0];
                final float yPos = outputLocations[0][i][1];
                final float w = outputLocations[0][i][2];
                final float h = outputLocations[0][i][3];
                final RectF rectF = new RectF(
                        Math.max(0, xPos - w / 2) / resizedBitmap.getWidth(),
                        Math.max(0, yPos - h / 2) / resizedBitmap.getHeight(),
                        Math.min(resizedBitmap.getWidth() - 1, xPos + w / 2) / resizedBitmap.getWidth(),
                        Math.min(resizedBitmap.getHeight() - 1, yPos + h / 2) / resizedBitmap.getHeight());
                boundingBoxes.add(new BoundingBox(rectF, maxConfidence, detectedClass));
            }
        }
        Log.d(TAG, "boundingBoxes.size(): " + boundingBoxes.size());
        Log.d(TAG, "maxMaxConfidence: " + maxMaxConfidence);
        boundingBoxes = nms(boundingBoxes);
        Log.d(TAG, "After NMS: " + boundingBoxes.size());
        return boundingBoxes;
    }
}
