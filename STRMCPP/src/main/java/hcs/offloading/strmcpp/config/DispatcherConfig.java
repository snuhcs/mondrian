package hcs.offloading.strmcpp.config;

import org.json.JSONException;
import org.json.JSONObject;

public class DispatcherConfig {
    public int MAX_QUEUE_SIZE = 2;
    public int FULL_INFERENCE_INTERVAL = 4;
    public int ROI_PADDING = 10;

    DispatcherConfig() {
    }

    DispatcherConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("max_queue_size")) {
            MAX_QUEUE_SIZE = jsonObject.getInt("max_queue_size");
        }
        if (jsonObject.has("full_inference_interval")) {
            FULL_INFERENCE_INTERVAL = jsonObject.getInt("full_inference_interval");
        }
        if (jsonObject.has("roi_padding")) {
            ROI_PADDING = jsonObject.getInt("roi_padding");
        }
    }
}
