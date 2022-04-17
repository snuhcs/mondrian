package hcs.offloading.edgedevice.config;

import org.json.JSONException;
import org.json.JSONObject;

public class ResizeProfileConfig {
    public boolean FIT_RESIZE = false;
    public boolean MERGED_RESIZE = false;
    public int PERSON_THRESHOLD = 160;
    public int CLASS_AGNOSTIC_THRESHOLD = 160;

    ResizeProfileConfig() {
    }

    ResizeProfileConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("fit_resize")) {
            FIT_RESIZE = jsonObject.getBoolean("fit_resize");
        }
        if (jsonObject.has("merged_resize")) {
            MERGED_RESIZE = jsonObject.getBoolean("merged_resize");
        }
        if (jsonObject.has("person_threshold")) {
            PERSON_THRESHOLD = jsonObject.getInt("person_threshold");
        }
        if (jsonObject.has("class_agnostic_threshold")) {
            CLASS_AGNOSTIC_THRESHOLD = jsonObject.getInt("class_agnostic_threshold");
        }
    }
}
