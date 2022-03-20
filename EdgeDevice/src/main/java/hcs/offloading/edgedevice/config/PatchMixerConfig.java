package hcs.offloading.edgedevice.config;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import hcs.offloading.edgedevice.Utils;

public class PatchMixerConfig {
    public int MAX_OPTICAL_FLOW_INTERVAL = 30;

    PatchMixerConfig() {
    }

    PatchMixerConfig(String jsonPath) throws IOException, JSONException {
        this(new JSONObject(Utils.getStringFromFile(jsonPath)));
    }

    PatchMixerConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("max_optical_flow_interval")) {
            MAX_OPTICAL_FLOW_INTERVAL = jsonObject.getInt("max_optical_flow_interval");
        }
    }
}
