package hcs.offloading.edgeserver.config;

public class RoIExtractorConfig {
    public int MIXED_FRAME_SIZE = 640;
    public int IDLE_WAIT_MS = 1000;
    public int AREA_THRESHOLD = 10000;
    public int ROI_PADDING = 0;
    public String EXTRACTION_METHOD = "combined"; // combined / pd (pixel diff) / of (optical flow)
}
