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

public class YoloV5Classifier implements Classifier {
    private final static String TAG = YoloV5Classifier.class.getName();

    private static final float CONF_THRESHOLD = 0.001f;
    private static final float IOU_THRESHOLD = 0.6f;
    private static final Vector<String> labels = new Vector<>();

    private static final int NUM_LABELS = 80;
    public final int INPUT_SIZE;
    private final int OUTPUT_WIDTH;

    private final Interpreter tfLite;

    public YoloV5Classifier(AssetManager assetManager, int size) {
        assert size % 32 == 0;
        INPUT_SIZE = size;
        String modelFilename = "yolov5m-" + size + "-fp16.tflite";
        OUTPUT_WIDTH = (size / 32) * (size / 32) * 63;

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
            assert labels.size() == NUM_LABELS;
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
        outputMap.put(0, new float[1][OUTPUT_WIDTH][5 + NUM_LABELS]); // Box + Obj Confidence + Cls Confidences
        Object[] inputArray = {byteBuffer};
        long start = System.nanoTime();
        tfLite.runForMultipleInputsOutputs(inputArray, outputMap);
        long end = System.nanoTime();
        Log.v(TAG, "Inference time (us): " + (end - start) / 1000);

        float[][][] outputs = (float[][][]) outputMap.get(0);
        assert outputs != null;

        for (int i = 0; i < OUTPUT_WIDTH; i++) {
            final float[] output = outputs[0][i]; // 5 + NUM_LABELS : xywh + obj conf + cls confs
            if (output[4] <= CONF_THRESHOLD) {
                continue;
            }
            float maxConf = 0f;
            int maxLabel = -1;
            for (int j = 0; j < NUM_LABELS; j++) {
                if (maxConf < output[j + 5]) {
                    maxConf = output[j + 5];
                    maxLabel = j;
                }
            }
            maxConf *= output[4]; // conf = obj conf * cls conf
            if (maxConf <= CONF_THRESHOLD) {
                continue;
            }
            final float xPos = output[0];
            final float yPos = output[1];
            final float w = output[2];
            final float h = output[3];
            final Rect rect = new Rect(
                    Math.max(0, (int) ((xPos - w / 2) * originalWidth)),
                    Math.max(0, (int) ((yPos - h / 2) * originalHeight)),
                    Math.min(originalWidth - 1, (int) ((xPos + w / 2) * originalWidth)),
                    Math.min(originalHeight - 1, (int) ((yPos + h / 2) * originalHeight)));
            if (rect.left < rect.right && rect.top < rect.bottom) {
                detections.add(new BoundingBox(rect, maxConf, labels.get(maxLabel)));
            }
        }
        return detections;
    }

    private static List<BoundingBox> nms(List<BoundingBox> boxes) {
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
