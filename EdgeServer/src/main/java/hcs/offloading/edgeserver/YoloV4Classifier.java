package hcs.offloading.edgeserver;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.util.Log;

import org.tensorflow.lite.Interpreter;
import org.tensorflow.lite.gpu.CompatibilityList;
import org.tensorflow.lite.gpu.GpuDelegate;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Vector;

import hcs.offloading.edgeserver.datatypes.BoundingBox;

public class YoloV4Classifier {
    private final static String TAG = YoloV4Classifier.class.getName();

    private static final float OBJ_THRESHOLD = 0.5f;
    private static final float NMS_THRESHOLD = 0.6f;
    private static final Vector<String> labels = new Vector<>();

    public final int INPUT_SIZE;
    private final int[] OUTPUT_WIDTH_FULL;

    private final Interpreter tfLite;

    YoloV4Classifier(AssetManager assetManager, int size) {
        INPUT_SIZE = size;
        String modelFilename;
        switch (size) {
            case 64:
                modelFilename = "yolov4-64.tflite";
                OUTPUT_WIDTH_FULL = new int[]{252, 252};
                break;
            case 160:
                modelFilename = "yolov4-160.tflite";
                OUTPUT_WIDTH_FULL = new int[]{1575, 1575};
                break;
            case 640:
                modelFilename = "yolov4-640.tflite";
                OUTPUT_WIDTH_FULL = new int[]{25200, 25200};
                break;
            default:
                modelFilename = null;
                OUTPUT_WIDTH_FULL = new int[]{-1, -1};
                Log.e(TAG, "Wrong size : " + size);
        }

        try {
            InputStream labelsInput = assetManager.open("coco.txt");
            BufferedReader br = new BufferedReader(new InputStreamReader(labelsInput));
            String line;
            if (labels.isEmpty()) {
                while ((line = br.readLine()) != null) {
                    labels.add(line);
                }
            }
            br.close();
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
        }

        Interpreter.Options options = (new Interpreter.Options());
        options.setNumThreads(1);
        CompatibilityList compatList = new CompatibilityList();
        if (compatList.isDelegateSupportedOnThisDevice()) {
            Log.d(TAG, "GPU supported");
            GpuDelegate.Options delegateOptions = compatList.getBestOptionsForThisDevice();
            GpuDelegate gpuDelegate = new GpuDelegate(delegateOptions);
            options.addDelegate(gpuDelegate);
        }
        tfLite = new Interpreter(loadModelFile(assetManager, modelFilename), options);
    }

    public List<BoundingBox> recognizeImage(ByteBuffer byteBuffer, Bitmap bitmap) {
        ArrayList<BoundingBox> detections = getDetectionsForFull(byteBuffer, bitmap);
        return nms(detections);
    }

    private static MappedByteBuffer loadModelFile(AssetManager assets, String modelFilename) {
        MappedByteBuffer buffer = null;
        try {
            AssetFileDescriptor fileDescriptor = assets.openFd(modelFilename);
            FileInputStream inputStream = new FileInputStream(fileDescriptor.getFileDescriptor());
            FileChannel fileChannel = inputStream.getChannel();
            long startOffset = fileDescriptor.getStartOffset();
            long declaredLength = fileDescriptor.getDeclaredLength();
            buffer = fileChannel.map(FileChannel.MapMode.READ_ONLY, startOffset, declaredLength);
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
        }
        return buffer;
    }

    private ArrayList<BoundingBox> getDetectionsForFull(ByteBuffer byteBuffer, Bitmap bitmap) {
        ArrayList<BoundingBox> detections = new ArrayList<>();
        Map<Integer, Object> outputMap = new HashMap<>();
        outputMap.put(0, new float[1][OUTPUT_WIDTH_FULL[0]][4]);
        outputMap.put(1, new float[1][OUTPUT_WIDTH_FULL[1]][labels.size()]);
        Object[] inputArray = {byteBuffer};
        long start = System.nanoTime();
        tfLite.runForMultipleInputsOutputs(inputArray, outputMap);
        long end = System.nanoTime();
        // Log.v(TAG, "Inference time (us): " + (end - start) / 1000);

        int gridWidth = OUTPUT_WIDTH_FULL[0];
        float[][][] bboxes = (float[][][]) outputMap.get(0);
        float[][][] out_score = (float[][][]) outputMap.get(1);

        for (int i = 0; i < gridWidth; i++) {
            float maxClass = 0;
            int detectedClass = -1;
            final float[] classes = new float[labels.size()];
            assert out_score != null;
            System.arraycopy(out_score[0][i], 0, classes, 0, labels.size());
            for (int c = 0; c < labels.size(); ++c) {
                if (classes[c] > maxClass) {
                    detectedClass = c;
                    maxClass = classes[c];
                }
            }
            final float score = maxClass;
            if (score > OBJ_THRESHOLD) {
                assert bboxes != null;
                final float xPos = bboxes[0][i][0];
                final float yPos = bboxes[0][i][1];
                final float w = bboxes[0][i][2];
                final float h = bboxes[0][i][3];
                final Rect rect = new Rect(
                        (int) (Math.max(0, xPos - w / 2) * bitmap.getWidth() / INPUT_SIZE),
                        (int) (Math.max(0, yPos - h / 2) * bitmap.getHeight() / INPUT_SIZE),
                        (int) (Math.min(INPUT_SIZE - 1, xPos + w / 2) * bitmap.getWidth() / INPUT_SIZE),
                        (int) (Math.min(INPUT_SIZE - 1, yPos + h / 2) * bitmap.getHeight() / INPUT_SIZE));
                detections.add(new BoundingBox(rect, score, detectedClass, labels.get(detectedClass)));
            }
        }
        return detections;
    }

    public static List<BoundingBox> nms(List<BoundingBox> list) {
        ArrayList<BoundingBox> nmsList = new ArrayList<>();

        for (int k = 0; k < labels.size(); k++) {
            PriorityQueue<BoundingBox> pq = new PriorityQueue<>(
                    50, (lhs, rhs) -> Float.compare(rhs.confidence, lhs.confidence));

            for (int i = 0; i < list.size(); ++i) {
                if (list.get(i).label == k) {
                    pq.add(list.get(i));
                }
            }

            while (pq.size() > 0) {
                BoundingBox[] a = new BoundingBox[pq.size()];
                BoundingBox[] detections = pq.toArray(a);
                BoundingBox max = detections[0];
                nmsList.add(max);
                pq.clear();

                for (int j = 1; j < detections.length; j++) {
                    BoundingBox detection = detections[j];
                    Rect b = detection.location;
                    if (Utils.box_iou(max.location, b) < NMS_THRESHOLD) {
                        pq.add(detection);
                    }
                }
            }
        }
        return nmsList;
    }
}
