package hcs.offloading.edgedevice.config;

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import hcs.offloading.strm.STRMUtils;

public class Config {
    private static final String TAG = Config.class.getName();

    public String LOG_PATH;
    public final SourceConfig sourceConfig;
    public final ResizeProfileConfig resizeProfileConfig;
    public final InferenceEngineConfig inferenceEngineConfig;

    public Config(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(STRMUtils.getStringFromFile(jsonPath));
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
}
