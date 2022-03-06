package hcs.offloading.edgeserver.config;

public class InferenceEngineConfig {
    public int MIXED_FRAME_SIZE; // Must be same as RoIExtractorConfig.MIXED_FRAME_SIZE
    public int NUM_WORKERS = 1;
}
