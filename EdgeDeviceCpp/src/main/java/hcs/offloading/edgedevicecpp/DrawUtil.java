package hcs.offloading.edgedevicecpp;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import hcs.offloading.strmcpp.BoundingBox;

public class DrawUtil {
    private static final String[] LABELS = {
            "person",
            "bicycle",
            "car",
            "motorbike",
            "aeroplane",
            "bus",
            "train",
            "truck",
            "boat",
            "traffic light",
            "fire hydrant",
            "stop sign",
            "parking meter",
            "bench",
            "bird",
            "cat",
            "dog",
            "horse",
            "sheep",
            "cow",
            "elephant",
            "bear",
            "zebra",
            "giraffe",
            "backpack",
            "umbrella",
            "handbag",
            "tie",
            "suitcase",
            "frisbee",
            "skis",
            "snowboard",
            "sports ball",
            "kite",
            "baseball bat",
            "baseball glove",
            "skateboard",
            "surfboard",
            "tennis racket",
            "bottle",
            "wine glass",
            "cup",
            "fork",
            "knife",
            "spoon",
            "bowl",
            "banana",
            "apple",
            "sandwich",
            "orange",
            "broccoli",
            "carrot",
            "hot dog",
            "pizza",
            "donut",
            "cake",
            "chair",
            "sofa",
            "potted plant",
            "bed",
            "dining table",
            "toilet",
            "tvmonitor",
            "laptop",
            "mouse",
            "remote",
            "keyboard",
            "cell phone",
            "microwave",
            "oven",
            "toaster",
            "sink",
            "refrigerator",
            "book",
            "clock",
            "vase",
            "scissors",
            "teddy bear",
            "hair drier",
            "toothbrush"};

    private static final int NUM_CLASSES = LABELS.length;
    private static final Paint[] PAINTS = initPaints();

    private static Paint[] initPaints() {
        Paint[] paints = new Paint[NUM_CLASSES];
        for (int i = 0; i < NUM_CLASSES; i++) {
            Paint paint = new Paint();
            paint.setColor(Color.HSVToColor(new float[]{120f + 240f * i / NUM_CLASSES, 1f, 1f}));
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(5.0f);
            paints[i] = paint;
        }
        return paints;
    }

    private static Map<String, Integer> initLabelMap() {
        Map<String, Integer> labelMap = new HashMap<>();
        for (int i = 0; i < LABELS.length; i++) {
            labelMap.put(LABELS[i], i);
        }
        return labelMap;
    }

    public static Bitmap drawBoxes(Bitmap bitmap, List<BoundingBox> boxes, float drawConfidence) {
        final Canvas canvas = new Canvas(bitmap);
        for (BoundingBox box : boxes) {
            if (box.confidence >= drawConfidence) {
                canvas.drawRect(new Rect(box.left, box.top, box.right, box.bottom), PAINTS[(box.id+1)%NUM_CLASSES]);
            }
        }
        return bitmap;
    }
}
