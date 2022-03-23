package hcs.offloading.edgedevice.datatypes;

import android.graphics.Rect;
import android.support.annotation.NonNull;

public class BoundingBox {
    public final float confidence;
    public final Rect location;
    public final String labelName;

    public BoundingBox(Rect location, float confidence, String labelName) {
        this.confidence = confidence;
        this.location = location;
        this.labelName = labelName;
    }

    public BoundingBox move(Rect location) {
        return new BoundingBox(location, confidence, labelName);
    }

    @NonNull
    @Override
    public String toString() {
        return location.left + "," + location.top + "," + location.right + "," + location.bottom + "," + confidence + "," + labelName;
    }
}
