package hcs.offloading.strm.config;

import org.json.JSONException;
import org.json.JSONObject;

public class PatchReconstructorConfig {
    public int MAX_QUEUE_SIZE = 2;
    public int MATCH_PADDING = 40;
    public float USE_IOU_THRESHOLD = 0.1f;
    public float DRAW_CONFIDENCE = 0.5f;
    public String LOG_PATH = null;

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
        if (jsonObject.has("log_path")) {
            LOG_PATH = jsonObject.getString("log_path");
        }
        if (jsonObject.has("draw_confidence")) {
            DRAW_CONFIDENCE = (float) jsonObject.getDouble("draw_confidence");
        }
    }
}
