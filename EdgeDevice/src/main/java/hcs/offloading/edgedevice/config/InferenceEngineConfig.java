package hcs.offloading.edgedevice.config;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import hcs.offloading.edgedevice.Utils;

public class InferenceEngineConfig {
    public boolean USE_YOLO_V4 = false;
    public boolean USE_TINY = false;
    public float CONF_THRESHOLD = 0.1f;
    public float IOU_THRESHOLD = 0.6f;
    public int NUM_WORKERS = 1;
    public int FRAME_SIZE = 800;
    public int FULL_FRAME_SIZE = 800;
    public boolean PER_ROI_KEEP_RATIO = false;

    InferenceEngineConfig() {
    }

    InferenceEngineConfig(String jsonPath) throws IOException, JSONException {
        this(new JSONObject(Utils.getStringFromFile(jsonPath)));
    }

    InferenceEngineConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("use_yolo_v4")) {
            USE_YOLO_V4 = jsonObject.getBoolean("use_yolo_v4");
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
        if (jsonObject.has("frame_size")) {
            FRAME_SIZE = jsonObject.getInt("frame_size");
        }
        if (jsonObject.has("full_frame_size")) {
            FULL_FRAME_SIZE = jsonObject.getInt("full_frame_size");
        } else {
            FULL_FRAME_SIZE = FRAME_SIZE;
        }
        if (jsonObject.has("per_roi_keep_ratio")) {
            PER_ROI_KEEP_RATIO = jsonObject.getBoolean("per_roi_keep_ratio");
        }
    }
}
