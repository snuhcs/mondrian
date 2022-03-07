package hcs.offloading.edgeserver.config;

public class DispatcherConfig {
    public int BATCH_SIZE = 8;
    public int FULL_INFERENCE_INTERVAL = 5;
    public int NUM_FRAMES_ON_MEMORY = 10 * BATCH_SIZE;
}
