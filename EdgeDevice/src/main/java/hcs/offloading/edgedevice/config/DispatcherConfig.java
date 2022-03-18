package hcs.offloading.edgedevice.config;

import java.util.ArrayList;
import java.util.List;

public class DispatcherConfig {
    public static class VideoConfig {
        public String PATH = null;
        public int WIDTH = 1920;
        public int HEIGHT = 1080;
        public int FPS = 5;
    }

    public boolean USE_LOCAL_VIDEO = false;
    public List<VideoConfig> VIDEO_CONFIGS = new ArrayList<>();
}
