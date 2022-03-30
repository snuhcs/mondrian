package hcs.offloading.edgeserver.config;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import hcs.offloading.edgeserver.Utils;

public class DispatcherConfig {
    public static class VideoConfig {
        public String PATH = null;
        public int WIDTH = 1920;
        public int HEIGHT = 1080;
        public int FPS = 5;
    }

    public boolean USE_LOCAL_VIDEO = false;
    public List<VideoConfig> VIDEO_CONFIGS = new ArrayList<>();

    DispatcherConfig() {
    }

    DispatcherConfig(String jsonPath) throws IOException, JSONException {
        this(new JSONObject(Utils.getStringFromFile(jsonPath)));
    }

    DispatcherConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("use_local_video")) {
            USE_LOCAL_VIDEO = jsonObject.getBoolean("use_local_video");
            if (USE_LOCAL_VIDEO) {
                if (!jsonObject.has("video_configs") || jsonObject.getJSONArray("video_configs").length() == 0) {
                    throw new IllegalArgumentException("Video configs should be specified");
                }
                JSONArray videoConfigs = jsonObject.getJSONArray("video_configs");
                for (int i = 0; i < videoConfigs.length(); i++) {
                    DispatcherConfig.VideoConfig videoConfig = new DispatcherConfig.VideoConfig();
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
    }
}
