package hcs.offloading.edgedevice.config;

import org.json.JSONException;
import org.json.JSONObject;

public class InferenceEngineConfig {
    public enum Model {
        YOLO_V4,
        YOLO_V5,
    }

    public Model MODEL = Model.YOLO_V4;
    public boolean USE_TINY = false;
    public float CONF_THRESHOLD = 0.1f;
    public float IOU_THRESHOLD = 0.3f;
    public int NUM_WORKERS = 1;
    public int INPUT_SIZE = 800;
    public int FULL_FRAME_INPUT_SIZE = 800;

    InferenceEngineConfig() {
    }

    InferenceEngineConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("model")) {
            MODEL = Model.valueOf(jsonObject.getString("model").toUpperCase());
        }
        if (jsonObject.has("use_tiny")) {
            USE_TINY = jsonObject.getBoolean("use_tiny");
        }
        if (jsonObject.has("conf_threshold")) {
            CONF_THRESHOLD = (float) jsonObject.getDouble("conf_threshold");
        }
        if (jsonObject.has("iou_threshold")) {
            IOU_THRESHOLD = (float) jsonObject.getDouble("iou_threshold");
        }
        if (jsonObject.has("num_workers")) {
            NUM_WORKERS = jsonObject.getInt("num_workers");
        }
        if (jsonObject.has("input_size")) {
            INPUT_SIZE = jsonObject.getInt("input_size");
        }
        if (jsonObject.has("full_frame_input_size")) {
            FULL_FRAME_INPUT_SIZE = jsonObject.getInt("full_frame_input_size");
        }
    }
}
