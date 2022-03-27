package hcs.offloading.strm.config;

import org.json.JSONException;
import org.json.JSONObject;

public class PatchGeneratorConfig {
    public boolean FIT_RESIZE = false;
    public boolean MERGED_RESIZE = false;

    PatchGeneratorConfig() {
    }

    PatchGeneratorConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("fit_resize")) {
            FIT_RESIZE = jsonObject.getBoolean("fit_resize");
        }
        if (jsonObject.has("merged_resize")) {
            MERGED_RESIZE = jsonObject.getBoolean("merged_resize");
        }
    }
}
