package hcs.offloading.strmcpp.config;

import org.json.JSONException;
import org.json.JSONObject;

public class RoIExtractorConfig {
    public int MAX_QUEUE_SIZE = 2;
    public int EXTRACTION_RESIZE_WIDTH = 640;
    public int EXTRACTION_RESIZE_HEIGHT = 360;
    public float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = 0.3f;
    public float MERGE_THRESHOLD = 0.5f;
    public boolean OF_ROI = true;
    public boolean PD_ROI = true;
    public boolean MERGE_ROI = true;

    RoIExtractorConfig() {
    }

    RoIExtractorConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("max_queue_size")) {
            MAX_QUEUE_SIZE = jsonObject.getInt("max_queue_size");
        }
        if (jsonObject.has("extraction_resize_width")) {
            EXTRACTION_RESIZE_WIDTH = jsonObject.getInt("extraction_resize_width");
        }
        if (jsonObject.has("extraction_resize_height")) {
            EXTRACTION_RESIZE_HEIGHT = jsonObject.getInt("extraction_resize_height");
        }
        if (jsonObject.has("optical_flow_roi_confidence_threshold")) {
            OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = (float) jsonObject.getDouble("optical_flow_roi_confidence_threshold");
        }
        if (jsonObject.has("merge_threshold")) {
            MERGE_THRESHOLD = (float) jsonObject.getDouble("merge_threshold");
        }
        if (jsonObject.has("of_roi")) {
            OF_ROI = jsonObject.getBoolean("of_roi");
        }
        if (jsonObject.has("pd_roi")) {
            PD_ROI = jsonObject.getBoolean("pd_roi");
        }
        if (jsonObject.has("merge_roi")) {
            MERGE_ROI = jsonObject.getBoolean("merge_roi");
        }
    }
}
