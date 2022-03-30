package hcs.offloading.edgedevice.inferenceengine.model;

import java.nio.ByteBuffer;
import java.util.List;

import hcs.offloading.strm.datatypes.BoundingBox;

public interface Classifier {
    List<BoundingBox> recognizeImage(ByteBuffer byteBuffer, int originalWidth, int originalHeight);
}
