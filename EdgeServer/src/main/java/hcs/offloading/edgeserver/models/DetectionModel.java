package hcs.offloading.edgeserver.models;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.RectF;
import android.util.Log;

import org.tensorflow.lite.Interpreter;
import org.tensorflow.lite.gpu.GpuDelegate;
import org.tensorflow.lite.support.image.ImageProcessor;
import org.tensorflow.lite.support.image.TensorImage;
import org.tensorflow.lite.support.image.ops.ResizeOp;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.PriorityQueue;

public abstract class DetectionModel {
    private static final String TAG = DetectionModel.class.getName();
    protected final Interpreter mInterpreter;
    protected final List<String> mLabelList;
    protected final ImageProcessor mImageProcessor;
    protected int mInputSize;
    protected float mNmsThresh = 0.6f;

    public DetectionModel(AssetManager assetManager, String modelPath, String labelPath, int inputSize) {
        Interpreter.Options options = new Interpreter.Options();
        int NUM_CORES = Runtime.getRuntime().availableProcessors();
        Log.d(TAG, "NUM_CORES:" + NUM_CORES);
        options.setNumThreads(NUM_CORES);
        options.addDelegate(new GpuDelegate());
        mInputSize = inputSize;
        mInterpreter = new Interpreter(Objects.requireNonNull(loadModelFile(assetManager, modelPath)), options);
        mLabelList = loadLabelList(assetManager, labelPath);
        mImageProcessor = new ImageProcessor.Builder()
                .add(new ResizeOp(inputSize, inputSize, ResizeOp.ResizeMethod.BILINEAR))
                .build();
    }

    private MappedByteBuffer loadModelFile(AssetManager assetManager, String modelPath) {
        try {
            AssetFileDescriptor fileDescriptor = assetManager.openFd(modelPath);
            FileInputStream inputStream = new FileInputStream(fileDescriptor.getFileDescriptor());
            FileChannel fileChannel = inputStream.getChannel();
            long startOffset = fileDescriptor.getStartOffset();
            long declaredLength = fileDescriptor.getDeclaredLength();
            return fileChannel.map(FileChannel.MapMode.READ_ONLY, startOffset, declaredLength);
        } catch (IOException e) {
            Log.d(TAG, "loadModelFile failed");
            return null;
        }
    }

    private List<String> loadLabelList(AssetManager assetManager, String labelPath) {
        try {
            List<String> labelList = new ArrayList<>();
            BufferedReader reader = new BufferedReader(new InputStreamReader(assetManager.open(labelPath)));
            String line;
            while ((line = reader.readLine()) != null) {
                labelList.add(line);
            }
            reader.close();
            return labelList;
        } catch (IOException e) {
            Log.e(TAG, "loadLabelList failed");
            return null;
        }
    }

    protected ByteBuffer convertBitmapToByteBuffer(Bitmap bitmap, boolean quantized) {
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect((quantized ? 1 : 4) * mInputSize * mInputSize * 3);
        byteBuffer.order(ByteOrder.nativeOrder());
        int[] intValues = new int[mInputSize * mInputSize];
        bitmap.getPixels(intValues, 0, bitmap.getWidth(), 0, 0, bitmap.getWidth(), bitmap.getHeight());
        int pixel = 0;
        for (int i = 0; i < mInputSize; ++i) {
            for (int j = 0; j < mInputSize; ++j) {
                if (quantized) {
                    final int pixelValue = intValues[pixel++];
                    byteBuffer.put((byte) ((pixelValue >> 16) & 0xFF));
                    byteBuffer.put((byte) ((pixelValue >> 8) & 0xFF));
                    byteBuffer.put((byte) (pixelValue & 0xFF));
                } else {
                    final int val = intValues[pixel++];
                    byteBuffer.putFloat(((val >> 16) & 0xFF) / 255.0f);
                    byteBuffer.putFloat(((val >> 8) & 0xFF) / 255.0f);
                    byteBuffer.putFloat((val & 0xFF) / 255.0f);
                }
            }
        }
        return byteBuffer;
    }

    protected List<BoundingBox> nms(List<BoundingBox> list) {
        List<BoundingBox> nmsList = new ArrayList<>();

        for (int k = 0; k < mLabelList.size(); k++) {
            //1.find max confidence per class
            PriorityQueue<BoundingBox> pq =
                    new PriorityQueue<>(
                            50,
                            (lhs, rhs) -> {
                                // Intentionally reversed to put high confidence at the head of the queue.
                                return Float.compare(rhs.confidence, lhs.confidence);
                            });

            for (int i = 0; i < list.size(); ++i) {
                if (list.get(i).label == k) {
                    pq.add(list.get(i));
                }
            }

            //2.do non maximum suppression
            while (pq.size() > 0) {
                //insert detection with max confidence
                BoundingBox[] a = new BoundingBox[pq.size()];
                BoundingBox[] detections = pq.toArray(a);
                BoundingBox max = detections[0];
                nmsList.add(max);
                pq.clear();

                for (int j = 1; j < detections.length; j++) {
                    BoundingBox detection = detections[j];
                    RectF b = detection.location;
                    if (box_iou(max.location, b) < mNmsThresh) {
                        pq.add(detection);
                    }
                }
            }
        }
        return nmsList;
    }

    protected float box_iou(RectF a, RectF b) {
        return box_intersection(a, b) / box_union(a, b);
    }

    protected float box_intersection(RectF a, RectF b) {
        float w = overlap((a.left + a.right) / 2, a.right - a.left,
                (b.left + b.right) / 2, b.right - b.left);
        float h = overlap((a.top + a.bottom) / 2, a.bottom - a.top,
                (b.top + b.bottom) / 2, b.bottom - b.top);
        if (w < 0 || h < 0) return 0;
        float area = w * h;
        return area;
    }

    protected float box_union(RectF a, RectF b) {
        float i = box_intersection(a, b);
        float u = (a.right - a.left) * (a.bottom - a.top) + (b.right - b.left) * (b.bottom - b.top) - i;
        return u;
    }

    protected float overlap(float x1, float w1, float x2, float w2) {
        float l1 = x1 - w1 / 2;
        float l2 = x2 - w2 / 2;
        float left = l1 > l2 ? l1 : l2;
        float r1 = x1 + w1 / 2;
        float r2 = x2 + w2 / 2;
        float right = r1 < r2 ? r1 : r2;
        return right - left;
    }

    public abstract List<BoundingBox> inference(TensorImage image);
}
