package hcs.offloading.edgeserver.config;

public class InferenceEngineConfig {
    public boolean USE_YOLO_V4 = false;
    public boolean USE_TINY = false;
    public int NUM_WORKERS = 1;
    public int FRAME_SIZE = 800;
    public int FULL_FRAME_SIZE = 800;
    public boolean PER_ROI_KEEP_RATIO = false;
}
