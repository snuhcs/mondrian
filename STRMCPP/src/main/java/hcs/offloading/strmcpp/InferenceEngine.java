package hcs.offloading.strmcpp;

import org.opencv.core.Mat;

import java.util.List;

import hcs.offloading.strm.datatypes.BoundingBox;

public interface InferenceEngine {
    int enqueue(Mat mat, boolean isFull) throws InterruptedException;

    List<BoundingBox> getResults(int handle) throws InterruptedException;

    void close();
}
