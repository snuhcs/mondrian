package hcs.offloading.edgeserver.config;

public class RoIExtractorConfig {
    public enum Method {
        COMBINED, // combined
        OF, // pd (pixel diff)
        PD, // of (optical flow)
    }

    public boolean IS_BASELINE = false;
    public int BATCH_SIZE = 10;
    public int FULL_INFERENCE_BATCH_INTERVAL = 4;
    public float MERGE_THRESHOLD = 0.5f;
    public int ROI_PADDING = 3;
    public Method EXTRACTION_METHOD = Method.COMBINED;
    public int PERSON_THRESHOLD = 160;
    public int CLASS_AGNOSTIC_THRESHOLD = 160;
}
