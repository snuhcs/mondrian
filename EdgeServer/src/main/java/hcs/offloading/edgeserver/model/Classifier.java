package hcs.offloading.edgeserver.model;

import java.nio.ByteBuffer;
import java.util.List;

import hcs.offloading.edgeserver.datatypes.BoundingBox;

public interface Classifier {
    List<BoundingBox> recognizeImage(ByteBuffer byteBuffer, int originalWidth, int originalHeight);
}
