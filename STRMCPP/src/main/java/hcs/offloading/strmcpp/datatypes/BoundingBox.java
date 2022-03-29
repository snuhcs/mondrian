package hcs.offloading.strmcpp.datatypes;

import android.graphics.Rect;

public class BoundingBox {
    public final float confidence;
    public final Rect location;
    public final String labelName;

    public BoundingBox(Rect location, float confidence, String labelName) {
        this.confidence = confidence;
        this.location = location;
        this.labelName = labelName;
    }
}
