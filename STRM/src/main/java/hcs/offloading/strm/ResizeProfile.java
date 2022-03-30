package hcs.offloading.strm;

public interface ResizeProfile {
    float getScale(String labelName, int width, int height, int minOriginLength);
}
