package hcs.offloading.edgedevice.config;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

public class SourceConfig {
    public static class VideoConfig {
        public String PATH = null;
        public int WIDTH = 1920;
        public int HEIGHT = 1080;
        public int FPS = 5;
    }

    public boolean USE_LOCAL_VIDEO = false;
    public List<VideoConfig> VIDEO_CONFIGS = new ArrayList<>();
    public float DRAW_CONFIDENCE = 0.1f;

    SourceConfig() {
    }

    SourceConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("use_local_video")) {
            USE_LOCAL_VIDEO = jsonObject.getBoolean("use_local_video");
            if (USE_LOCAL_VIDEO) {
                if (!jsonObject.has("video_configs") || jsonObject.getJSONArray("video_configs").length() == 0) {
                    throw new IllegalArgumentException("Video configs should be specified");
                }
                JSONArray videoConfigs = jsonObject.getJSONArray("video_configs");
                for (int i = 0; i < videoConfigs.length(); i++) {
                    SourceConfig.VideoConfig videoConfig = new SourceConfig.VideoConfig();
                    JSONObject jsonVideoConfig = videoConfigs.getJSONObject(i);
                    if (jsonVideoConfig.has("path")) {
                        videoConfig.PATH = jsonVideoConfig.getString("path");
                    }
                    if (jsonVideoConfig.has("width")) {
                        videoConfig.WIDTH = jsonVideoConfig.getInt("width");
                    }
                    if (jsonVideoConfig.has("height")) {
                        videoConfig.HEIGHT = jsonVideoConfig.getInt("height");
                    }
                    if (jsonVideoConfig.has("fps")) {
                        videoConfig.FPS = jsonVideoConfig.getInt("fps");
                    }
                    VIDEO_CONFIGS.add(videoConfig);
                }
            }
        }
        if (jsonObject.has("draw_confidence")) {
            DRAW_CONFIDENCE = (float) jsonObject.getDouble("draw_confidence");
        }
    }
}
