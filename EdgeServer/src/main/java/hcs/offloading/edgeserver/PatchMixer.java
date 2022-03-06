package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Pair;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;

import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.edgeserver.datatypes.RoI;

@RequiresApi(api = Build.VERSION_CODES.P)
public class PatchMixer {
    private final static int MATCH_PADDING = 40;
    private final static float USE_IOU_THRESHOLD = 0.1f;

    public static Map<String, Map<Integer, List<BoundingBox>>> reconstructFrames(InferenceRequest inferenceRequest, List<BoundingBox> boxes) {
        Map<String, Map<Integer, List<BoundingBox>>> batchResults = new HashMap<>();
        for (Map.Entry<String, List<Integer>> kv : inferenceRequest.mixedFrameIndices.entrySet()) {
            batchResults.put(kv.getKey(), new HashMap<>());
            for (int frameIndex : kv.getValue()) {
                batchResults.get(kv.getKey()).put(frameIndex, new ArrayList<>());
            }
        }
        List<RoI> rois = inferenceRequest.rois;
        for (BoundingBox box : boxes) {
            float maxIoU = -1f;
            RoI maxRoI = null;
            Rect maxBoxPos = null;
            for (RoI roi : rois) {
                if (!roi.isPacked()) {
                    continue;
                }
                Rect paddedRoIPos = new Rect(
                        roi.position.left - MATCH_PADDING,
                        roi.position.top - MATCH_PADDING,
                        roi.position.right + MATCH_PADDING,
                        roi.position.bottom + MATCH_PADDING
                );
                Rect boxPos = box.location;
                Rect movedAndResizedBoxPos = new Rect(
                        (int) ((boxPos.left - roi.packedLocation[0]) / roi.scale) + roi.position.left,
                        (int) ((boxPos.top - roi.packedLocation[1]) / roi.scale) + roi.position.top,
                        (int) ((boxPos.right - roi.packedLocation[0]) / roi.scale) + roi.position.left,
                        (int) ((boxPos.bottom - roi.packedLocation[1]) / roi.scale) + roi.position.top
                );
                float iou = Utils.box_iou(paddedRoIPos, movedAndResizedBoxPos);
                if (maxRoI == null || maxIoU < iou) {
                    maxIoU = iou;
                    maxRoI = roi;
                    maxBoxPos = movedAndResizedBoxPos;
                }
            }
            if (maxRoI != null && maxIoU > USE_IOU_THRESHOLD) {
                maxRoI.packedLocation = null;
                box = box.move(maxBoxPos);
                batchResults.get(maxRoI.getSourceIP()).get(maxRoI.getFrameIndex()).add(box);
            }
        }
        return batchResults;
    }

    public static Bitmap packRoIs(List<RoI> rois, int targetSize) {
        Bitmap bitmap = Bitmap.createBitmap(targetSize, targetSize, Bitmap.Config.ARGB_8888);
        bitmap.eraseColor(Color.BLACK);
        Canvas canvas = new Canvas(bitmap);

        List<Rect> freeRectList = new ArrayList<>();
        freeRectList.add(new Rect(0, 0, targetSize, targetSize));

        for (RoI roi : rois) {
            int[] WH = roi.resizedWH();
            ListIterator<Rect> iter = freeRectList.listIterator();
            while (iter.hasNext()) {
                Rect freeRect = iter.next();
                if (canFit(WH, freeRect)) {
                    iter.remove();
                    Bitmap resizedRoI = roi.resizedBitmap();
                    roi.packedLocation = new int[]{freeRect.left, freeRect.top};
                    canvas.drawBitmap(resizedRoI, freeRect.left, freeRect.top, null);
                    Pair<Rect, Rect> newFreeRectPair = splitFreeRect(resizedRoI, freeRect);
                    freeRectList.add(newFreeRectPair.first);
                    freeRectList.add(newFreeRectPair.second);
                    break;
                }
            }
        }
        return bitmap;
    }

    private static boolean canFit(int[] WH, Rect rect) {
        return (WH[1] <= rect.height()) && (WH[0] <= rect.width());
    }

    private static Pair<Rect, Rect> splitFreeRect(Bitmap bm, Rect rect) {
        int w = bm.getWidth();
        int h = bm.getHeight();
        Rect rect1, rect2;
        if (rect.width() > rect.height()) {
            rect1 = new Rect(rect.left + w, rect.top, rect.right, rect.bottom);
            rect2 = new Rect(rect.left, rect.top + h, rect.left + w, rect.bottom);
        } else {
            rect1 = new Rect(rect.left, rect.top + h, rect.right, rect.bottom);
            rect2 = new Rect(rect.left + w, rect.top, rect.right, rect.top + h);
        }
        return new Pair<>(rect1, rect2);
    }
}
