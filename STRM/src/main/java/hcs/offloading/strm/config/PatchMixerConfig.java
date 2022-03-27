package hcs.offloading.strm.config;

import org.json.JSONException;
import org.json.JSONObject;

public class PatchMixerConfig {
    public boolean PACKING = true;
    public int MAX_PACKED_FRAMES = 30;
    public int MIXED_FRAME_SIZE = 800;

    PatchMixerConfig() {
    }

    PatchMixerConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("packing")) {
            PACKING = jsonObject.getBoolean("packing");
        }
        if (jsonObject.has("max_packed_frames")) {
            MAX_PACKED_FRAMES = jsonObject.getInt("max_packed_frames");
        }
        if (jsonObject.has("mixed_frame_size")) {
            MIXED_FRAME_SIZE = jsonObject.getInt("mixed_frame_size");
        }
    }
}
