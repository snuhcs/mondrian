package hcs.offloading.edgedevice.config;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import hcs.offloading.edgedevice.Utils;

public class RoIExtractorConfig {
    public enum Method {
        COMBINED, // combined
        OF, // pd (pixel diff)
        PD, // of (optical flow)
    }

    public boolean PACKING = true;
    public int FULL_INFERENCE_INTERVAL = 4;
    public float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = 0.3f;
    public float MERGE_THRESHOLD = 0.5f;
    public int ROI_PADDING = 3;
    public Method EXTRACTION_METHOD = Method.COMBINED;
    public int PERSON_THRESHOLD = 160;
    public int CLASS_AGNOSTIC_THRESHOLD = 160;

    RoIExtractorConfig() {
    }

    RoIExtractorConfig(String jsonPath) throws IOException, JSONException {
        this(new JSONObject(Utils.getStringFromFile(jsonPath)));
    }

    RoIExtractorConfig(JSONObject jsonObject) throws JSONException {
        if (jsonObject.has("packing")) {
            PACKING = jsonObject.getBoolean("packing");
        }
        if (jsonObject.has("full_inference_interval")) {
            FULL_INFERENCE_INTERVAL = jsonObject.getInt("full_inference_interval");
        }
        if (jsonObject.has("optical_flow_roi_confidence_threshold")) {
            OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = (float) jsonObject.getDouble("optical_flow_roi_confidence_threshold");
        }
        if (jsonObject.has("merge_threshold")) {
            MERGE_THRESHOLD = (float) jsonObject.getDouble("merge_threshold");
        }
        if (jsonObject.has("roi_padding")) {
            ROI_PADDING = jsonObject.getInt("roi_padding");
        }
        if (jsonObject.has("extraction_method")) {
            String method = String.valueOf(jsonObject.get("extraction_method"));
            switch (method) {
                case "combined":
                    EXTRACTION_METHOD = RoIExtractorConfig.Method.COMBINED;
                    break;
                case "of":
                    EXTRACTION_METHOD = RoIExtractorConfig.Method.OF;
                    break;
                case "pd":
                    EXTRACTION_METHOD = RoIExtractorConfig.Method.PD;
                    break;
            }
        }
        if (jsonObject.has("person_threshold")) {
            PERSON_THRESHOLD = jsonObject.getInt("person_threshold");
        }
        if (jsonObject.has("class_agnostic_threshold")) {
            CLASS_AGNOSTIC_THRESHOLD = jsonObject.getInt("class_agnostic_threshold");
        }
    }
}
