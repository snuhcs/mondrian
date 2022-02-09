package hcs.offloading.edgeserver.models;

import android.annotation.SuppressLint;
import android.graphics.RectF;

public class BoundingBox {
    public final RectF location;
    public final float confidence;
    public final int label;

    BoundingBox(RectF rectF, float confidence, int label) {
        location = rectF;
        this.confidence = confidence;
        this.label = label;
    }

    BoundingBox(float left, float top, float right, float bottom, float confidence, int label) {
        location = new RectF(left, top, right, bottom);
        this.confidence = confidence;
        this.label = label;
    }

    @SuppressLint("DefaultLocale")
    @Override
    public String toString() {
        return String.format("%f,%f,%f,%f,%d", location.left, location.top, location.right, location.bottom, label);
    }
}
