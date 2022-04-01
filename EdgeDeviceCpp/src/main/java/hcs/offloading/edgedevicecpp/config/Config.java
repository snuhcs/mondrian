package hcs.offloading.edgedevicecpp.config;

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class Config {
    private static final String TAG = Config.class.getName();

    public String LOG_PATH;
    public final SourceConfig sourceConfig;
    public final ResizeProfileConfig resizeProfileConfig;
    public final InferenceEngineConfig inferenceEngineConfig;

    public Config(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(getStringFromFile(jsonPath));
        Log.d(TAG, "Parsed Config: " + jsonObject);

        if (jsonObject.has("log_path")) {
            LOG_PATH = jsonObject.getString("log_path");
        } else {
            LOG_PATH = null;
        }

        if (jsonObject.has("source")) {
            sourceConfig = new SourceConfig(jsonObject.getJSONObject("source"));
        } else {
            sourceConfig = new SourceConfig();
        }

        if (jsonObject.has("resize_profile")) {
            resizeProfileConfig = new ResizeProfileConfig(jsonObject.getJSONObject("resize_profile"));
        } else {
            resizeProfileConfig = new ResizeProfileConfig();
        }

        if (jsonObject.has("inference_engine")) {
            inferenceEngineConfig = new InferenceEngineConfig(jsonObject.getJSONObject("inference_engine"));
        } else {
            inferenceEngineConfig = new InferenceEngineConfig();
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
