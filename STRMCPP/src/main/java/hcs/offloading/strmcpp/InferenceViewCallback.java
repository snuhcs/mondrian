package hcs.offloading.strmcpp;

import java.util.List;

public interface InferenceViewCallback {
    void drawInferenceResult(long addr, List<BoundingBox> results);

    void drawObjectDetectionResult(long addr, List<BoundingBox> results);
}
