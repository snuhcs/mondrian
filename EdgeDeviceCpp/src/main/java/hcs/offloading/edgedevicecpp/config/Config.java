package hcs.offloading.edgedevicecpp.config;

import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class Config {
    private static final String TAG = Config.class.getName();

    public static class VideoConfig {
        public String PATH = null;
        public int WIDTH = 1920;
        public int HEIGHT = 1080;
        public int FPS = 30;
    }

    public float DRAW_CONFIDENCE = 0f;
    public boolean DRAW_VIDEO = false;
    public boolean USE_LOCAL_VIDEO = false;
    public List<VideoConfig> VIDEO_CONFIGS = new ArrayList<>();

    public Config(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(getStringFromFile(jsonPath));
        Log.d(TAG, "Parsed Config: " + jsonObject);

        if (jsonObject.has("source")) {
            jsonObject = jsonObject.getJSONObject("source");
        } else {
            return;
        }

        if (jsonObject.has("draw_video")) {
            DRAW_VIDEO = jsonObject.getBoolean("draw_video");
        }
        if (jsonObject.has("draw_confidence")) {
            DRAW_CONFIDENCE = (float) jsonObject.getDouble("draw_confidence");
        }
        if (jsonObject.has("use_local_video")) {
            USE_LOCAL_VIDEO = jsonObject.getBoolean("use_local_video");
            if (USE_LOCAL_VIDEO) {
                if (!jsonObject.has("video_configs") || jsonObject.getJSONArray("video_configs").length() == 0) {
                    throw new IllegalArgumentException("Video configs should be specified");
                }
                JSONArray videoConfigs = jsonObject.getJSONArray("video_configs");
                for (int i = 0; i < videoConfigs.length(); i++) {
                    VideoConfig videoConfig = new VideoConfig();
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

    private static String convertStreamToString(InputStream is) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            sb.append(line).append("\n");
        }
        reader.close();
        return sb.toString();
    }

    public static String getStringFromFile(String filePath) throws IOException {
        File fl = new File(filePath);
        FileInputStream fin = new FileInputStream(fl);
        String ret = convertStreamToString(fin);
        fin.close();
        return ret;
    }
}
