package hcs.offloading.edgeserver.config;

public class RoIExtractorConfig {
    public boolean IS_BASELINE = false;
    public int BATCH_SIZE = 8; // if is_baseline = true, BATCH_SIZE will be 1
    public int FULL_INFERENCE_BATCH_INTERVAL = 4;
    public int MAX_QUEUED_FRAMES = -1;
    public int MIXED_FRAME_SIZE = 640;
    public float MERGE_THRESHOLD = 0.5f;
    public int ROI_PADDING = 0;
    public ExtractionMethod EXTRACTION_METHOD = ExtractionMethod.COMBINED; // combined / pd (pixel diff) / of (optical flow)
    public int PERSON_THRESHOLD = 80;
    public int CLASS_AGNOSTIC_THRESHOLD = 100;
}
