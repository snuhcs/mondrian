package hcs.offloading.edgeserver.config;

public class RoIExtractorConfig {
    public int BATCH_SIZE = 8;
    public int MIXED_FRAME_SIZE = 640;
    public int AREA_THRESHOLD = 10000;
    public int ROI_PADDING = 0;
    public ExtractionMethod EXTRACTION_METHOD = ExtractionMethod.COMBINED; // combined / pd (pixel diff) / of (optical flow)
}

