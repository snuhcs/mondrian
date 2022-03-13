package hcs.offloading.edgeserver.config;

public class RoIExtractorConfig {
    public int BATCH_SIZE = 8; // if is_baseline = true, BATCH_SIZE must be 1
    public int MIXED_FRAME_SIZE = 640;
    public float MERGE_THRESHOLD = 0.5f;
    public int ROI_PADDING = 0;
    public ExtractionMethod EXTRACTION_METHOD = ExtractionMethod.COMBINED; // combined / pd (pixel diff) / of (optical flow)
}

