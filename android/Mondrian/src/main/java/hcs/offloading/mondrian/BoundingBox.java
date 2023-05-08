package hcs.offloading.mondrian;

public class BoundingBox {
    public final int l;
    public final int t;
    public final int r;
    public final int b;
    public final float confidence;
    public final int label;

    public BoundingBox(int l, int t, int r, int b, float confidence, int label) {
        this.l = l;
        this.t = t;
        this.r = r;
        this.b = b;
        this.confidence = confidence;
        this.label = label;
    }
}
