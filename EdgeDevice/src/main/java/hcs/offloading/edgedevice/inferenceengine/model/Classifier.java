package hcs.offloading.edgedevice.inferenceengine.model;

import org.opencv.core.Mat;

import java.util.List;

import hcs.offloading.strm.datatypes.BoundingBox;

public interface Classifier {
    List<BoundingBox> recognizeImage(Mat mat, int originalWidth, int originalHeight);
}
