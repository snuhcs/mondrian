package hcs.offloading.mondrian;

public class BoundingBox {
    public final int id;
    public final int left;
    public final int top;
    public final int right;
    public final int bottom;
    public final float confidence;
    public final int label;
    public final int origin;
    public final boolean isNew;

    public BoundingBox(int id, int left, int top, int right, int bottom, float confidence, int label, int origin, boolean isNew) {
        this.id = id;
        this.left = left;
        this.top = top;
        this.right = right;
        this.bottom = bottom;
        this.confidence = confidence;
        this.label = label;
        this.origin = origin;
        this.isNew = isNew;
    }
}
