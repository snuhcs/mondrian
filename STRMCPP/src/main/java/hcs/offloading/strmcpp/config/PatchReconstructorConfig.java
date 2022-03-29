package hcs.offloading.strmcpp.config;

import org.json.JSONException;
import org.json.JSONObject;

public class PatchReconstructorConfig {
    public int MAX_QUEUE_SIZE = 2;
    public int MATCH_PADDING = 40;
    public float USE_IOU_THRESHOLD = 0.1f;

    PatchReconstructorConfig() {
    }

    PatchReconstructorConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("max_queue_size")) {
            MAX_QUEUE_SIZE = jsonObject.getInt("max_queue_size");
        }
        if (jsonObject.has("match_padding")) {
            MATCH_PADDING = jsonObject.getInt("match_padding");
        }
        if (jsonObject.has("use_iou_threshold")) {
            USE_IOU_THRESHOLD = (float) jsonObject.getDouble("use_iou_threshold");
        }
    }
}
