package hcs.offloading.strm;

import java.util.List;

public interface InferenceViewCallback {
    void drawOutput0(long addr, List<BoundingBox> results);

    void drawOutput1(long addr, List<BoundingBox> results);
}
