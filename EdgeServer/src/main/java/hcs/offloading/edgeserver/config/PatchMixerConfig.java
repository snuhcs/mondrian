package hcs.offloading.edgeserver.config;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import hcs.offloading.edgeserver.Utils;

public class PatchMixerConfig {
    public int MAX_PACKED_FRAMES = 30;
    public int MIXED_FRAME_SIZE = 800;

    PatchMixerConfig() {
    }

    PatchMixerConfig(String jsonPath) throws IOException, JSONException {
        this(new JSONObject(Utils.getStringFromFile(jsonPath)));
    }

    PatchMixerConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("max_packed_frames")) {
            MAX_PACKED_FRAMES = jsonObject.getInt("max_packed_frames");
        }
        if (jsonObject.has("frame_size")) {
            MIXED_FRAME_SIZE = jsonObject.getInt("frame_size");
        }
    }
}
