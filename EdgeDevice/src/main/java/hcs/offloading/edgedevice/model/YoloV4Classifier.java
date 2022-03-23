package hcs.offloading.edgedevice.model;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
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

import hcs.offloading.edgedevice.Utils;
import hcs.offloading.edgedevice.datatypes.BoundingBox;

public class YoloV4Classifier implements Classifier {
    private final static String TAG = YoloV4Classifier.class.getName();

    private static final Vector<String> labels = new Vector<>();

    private static final int NUM_LABELS = 80;
    public final int INPUT_SIZE;
    private final int OUTPUT_WIDTH;
    private final float CONF_THRESHOLD;
    private final float IOU_THRESHOLD;

    private final Interpreter tfLite;

    public YoloV4Classifier(AssetManager assetManager, int size,
                            float confThreshold, float iouThreshold, boolean isTiny) {
        assert size % 32 == 0;
        CONF_THRESHOLD = confThreshold;
        IOU_THRESHOLD = iouThreshold;
        INPUT_SIZE = size;
        String modelFilename = isTiny ?
                "yolov4-tiny-" + size + ".tflite" :
                "yolov4-" + size + ".tflite";
        OUTPUT_WIDTH = (size / 32) * (size / 32) * (isTiny ? 15 : 63);

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

    @Override
    public List<BoundingBox> recognizeImage(ByteBuffer byteBuffer, int originalWidth, int originalHeight) {
        ArrayList<BoundingBox> detections = getDetectionsForFull(byteBuffer, originalWidth, originalHeight);
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

    private ArrayList<BoundingBox> getDetectionsForFull(ByteBuffer byteBuffer, int originalWidth, int originalHeight) {
        ArrayList<BoundingBox> detections = new ArrayList<>();
        Map<Integer, Object> outputMap = new HashMap<>();
        outputMap.put(0, new float[1][OUTPUT_WIDTH][4]);
        outputMap.put(1, new float[1][OUTPUT_WIDTH][labels.size()]);
        Object[] inputArray = {byteBuffer};
        long start = System.nanoTime();
        tfLite.runForMultipleInputsOutputs(inputArray, outputMap);
        long end = System.nanoTime();
        Log.v(TAG, "Inference time (us): " + (end - start) / 1000);

        float[][][] bboxes = (float[][][]) outputMap.get(0);
        float[][][] out_score = (float[][][]) outputMap.get(1);

        for (int i = 0; i < OUTPUT_WIDTH; i++) {
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
            if (score > CONF_THRESHOLD) {
                assert bboxes != null;
                final float xPos = bboxes[0][i][0];
                final float yPos = bboxes[0][i][1];
                final float w = bboxes[0][i][2];
                final float h = bboxes[0][i][3];
                final Rect rect = new Rect(
                        (int) (Math.max(0, xPos - w / 2) * originalWidth / INPUT_SIZE),
                        (int) (Math.max(0, yPos - h / 2) * originalHeight / INPUT_SIZE),
                        (int) (Math.min(INPUT_SIZE - 1, xPos + w / 2) * originalWidth / INPUT_SIZE),
                        (int) (Math.min(INPUT_SIZE - 1, yPos + h / 2) * originalHeight / INPUT_SIZE));
                detections.add(new BoundingBox(rect, score, labels.get(detectedClass)));
            }
        }
        return detections;
    }

    private List<BoundingBox> nms(List<BoundingBox> boxes) {
        ArrayList<BoundingBox> nmsList = new ArrayList<>();

        for (int k = 0; k < NUM_LABELS; k++) {
            String label = labels.get(k);
            PriorityQueue<BoundingBox> pq = new PriorityQueue<>(
                    50, (lhs, rhs) -> Float.compare(rhs.confidence, lhs.confidence));

            for (BoundingBox box : boxes) {
                if (box.labelName.equals(label)) {
                    pq.add(box);
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
                    if (Utils.box_iou(max.location, b) < IOU_THRESHOLD) {
                        pq.add(detection);
                    }
                }
            }
        }
        return nmsList;
    }
}
