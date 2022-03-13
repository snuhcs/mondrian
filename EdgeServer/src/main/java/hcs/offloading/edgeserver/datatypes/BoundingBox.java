package hcs.offloading.edgeserver.datatypes;

import android.graphics.Rect;
import android.support.annotation.NonNull;

public class BoundingBox {
    public final float confidence;
    public final Rect location;
    public final int label;
    public final String labelName;

    public BoundingBox(Rect location, float confidence, int label, String labelName) {
        this.confidence = confidence;
        this.location = location;
        this.label = label;
        this.labelName = labelName;
    }

    public BoundingBox move(Rect location) {
        return new BoundingBox(location, confidence, label, labelName);
    }

    @NonNull
    @Override
    public String toString() {
        return location.left + "," + location.top + "," + location.right + "," + location.bottom + "," + confidence + "," + labelName;
    }
}
