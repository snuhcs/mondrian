package hcs.offloading.sensorapplication;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class Config {

    public boolean USE_SAVED_VIDEO = false;
    public int WIDTH = 1920;
    public int HEIGHT = 1080;
    public int FPS = 5;
    public String VIDEO_PATH = "/data/local/tmp";

    Config(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(getStringFromFile(jsonPath));

        if (jsonObject.has("use_saved_video")) {
            USE_SAVED_VIDEO = jsonObject.getBoolean("use_saved_video");
        }
        if (jsonObject.has("width")) {
            WIDTH = jsonObject.getInt("width");
        }
        if (jsonObject.has("height")) {
            HEIGHT = jsonObject.getInt("height");
        }
        if (jsonObject.has("fps")) {
            FPS = jsonObject.getInt("fps");
        }
        if (jsonObject.has("video_path")) {
            VIDEO_PATH = jsonObject.getString("video_path");
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

    private static String getStringFromFile(String filePath) throws IOException {
        File fl = new File(filePath);
        FileInputStream fin = new FileInputStream(fl);
        String ret = convertStreamToString(fin);
        fin.close();
        return ret;
    }
}
