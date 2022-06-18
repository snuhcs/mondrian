package hcs.offloading.strmcpp;

public class BoundingBox {
    public final int left;
    public final int top;
    public final int right;
    public final int bottom;
    public final float confidence;
    public final int label;

    public BoundingBox(int left, int top, int right, int bottom, float confidence, int label) {
        this.left = left;
        this.top = top;
        this.right = right;
        this.bottom = bottom;
        this.confidence = confidence;
        this.label = label;
    }
}
