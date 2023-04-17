package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DashPathEffect;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

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
    private static final Paint fillPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private static final Paint[] PAINTS = initIdPaints();

    private static Paint[] initIdPaints() {
        int NUM_COLORS = 1440;
        Paint[] paints = new Paint[NUM_COLORS];
        for (int i = 0; i < NUM_COLORS; i++) {
            Paint paint = new Paint();
            paint.setColor(Color.HSVToColor(new float[]{(float) (i * 41) % 360, i % 2 == 0 ? 1f : 0.5f, 1f}));
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(5.0f);
            paint.setTextSize(50);
            paints[i] = paint;
        }
        fillPaint.setStyle(Paint.Style.FILL);
        fillPaint.setAntiAlias(true);
        fillPaint.setColor(Color.WHITE);
        fillPaint.setAlpha(100);

        return paints;
    }


    public static Bitmap drawBoxes(Bitmap bitmap, List<BoundingBox> boxes, boolean fancyOutput) {
        final Canvas canvas = new Canvas(bitmap);
        List<Integer> ids = new ArrayList<>();
        if (fancyOutput) {
            for (BoundingBox box : boxes) {
                assert (!ids.contains(box.id));
                ids.add(box.id);
            }
            Collections.sort(ids);
            int xLoc = 0;
            int lineno = 0;
            for (Integer id : ids) {
                Paint mPaint = PAINTS[id % NUM_CLASSES];
                int digit;
                if (id < 10) {
                    digit = 1;
                } else if (id < 100) {
                    digit = 2;
                } else if (id < 1000) {
                    digit = 3;
                } else if (id < 10000) {
                    digit = 4;
                } else {
                    digit = 5;
                }
                int textWidth = 10 + 30 * digit;
                if (xLoc + textWidth > canvas.getWidth()) {
                    lineno++;
                    xLoc = 0;
                }
                canvas.drawText(id.toString(), 20 + xLoc, 60 + lineno * 60, mPaint);
                xLoc += textWidth;
            }
        }

        for (BoundingBox box : boxes) {
            Rect mRect = new Rect(box.left, box.top, box.right, box.bottom);
            RectF mRectF = new RectF(box.left, box.top, box.right, box.bottom);
            Paint mPaint;
            if (box.id == -1 || !fancyOutput) {
                mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
                mPaint.setColor(Color.GREEN);
                mPaint.setStyle(Paint.Style.STROKE);
                mPaint.setStrokeWidth(5.0f);
            } else {
                mPaint = PAINTS[box.id % NUM_CLASSES];
            }

            if (!fancyOutput) {
                canvas.drawRect(mRect, mPaint);
            } else {
                switch (box.origin) {
                    case 0: // origin_Null
                    case 1: // origin_BB
                        canvas.drawRect(mRect, mPaint);
                        break;
                    case 2: // origin_PD
                        int roundSize = 20;
                        canvas.drawRoundRect(mRectF, roundSize, roundSize, mPaint);
                        break;
                    case 3: // origin_IP
                        mPaint.setPathEffect(new DashPathEffect(new float[]{10f, 10f}, 0f));
                        canvas.drawRect(mRect, mPaint);
                        mPaint.setPathEffect(null);
                        break;
                }
                if (box.isNew) {
                    canvas.drawRect(mRect, fillPaint);
                }
            }
        }
        return bitmap;
    }

    private void saveBitmap(Bitmap bmp, String filepath) {
        try (FileOutputStream out = new FileOutputStream(filepath)) {
            bmp.compress(Bitmap.CompressFormat.PNG, 100, out);
            // PNG is a lossless format, the compression factor (100) is ignored
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
