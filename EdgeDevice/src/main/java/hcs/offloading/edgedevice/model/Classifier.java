package hcs.offloading.edgedevice.model;

import java.nio.ByteBuffer;
import java.util.List;

import hcs.offloading.edgedevice.datatypes.BoundingBox;

public interface Classifier {
    List<BoundingBox> recognizeImage(ByteBuffer byteBuffer, int originalWidth, int originalHeight);
}
