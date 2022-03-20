package hcs.offloading.edgedevice.config;

public class RoIExtractorConfig {
    public enum Method {
        COMBINED, // combined
        OF, // pd (pixel diff)
        PD, // of (optical flow)
    }

    public boolean PACKING = true;
    public int FULL_INFERENCE_INTERVAL = 4;
    public float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = 0.3f;
    public float MERGE_THRESHOLD = 0.5f;
    public int ROI_PADDING = 3;
    public Method EXTRACTION_METHOD = Method.COMBINED;
    public int PERSON_THRESHOLD = 160;
    public int CLASS_AGNOSTIC_THRESHOLD = 160;
}
