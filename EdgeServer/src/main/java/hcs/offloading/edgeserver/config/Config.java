package hcs.offloading.edgeserver.config;

import android.util.Log;

import org.json.JSONException;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

public class Config {
    private static final String TAG = Config.class.getName();

    public final boolean isBaseline;

    public final DispatcherConfig dispatcherConfig;
    public final RoIExtractorConfig roIExtractorConfig;
    public final InferenceEngineConfig inferenceEngineConfig;
    public final PatchReconstructorConfig patchReconstructorConfig;
    public final UtilsConfig utilsConfig;

    public Config(String jsonPath) throws ParseException, JSONException, IOException, IllegalArgumentException {
        JSONParser jsonParser = new JSONParser();
        JSONObject jsonObject = (JSONObject) jsonParser.parse(new FileReader(new File(jsonPath)));
        Log.d(TAG, "Parsed Config: " + jsonObject.toJSONString());

        if (jsonObject.containsKey("is_baseline")) { // if is_baseline = true, BATCH_SIZE must be 1
            this.isBaseline = Boolean.getBoolean(String.valueOf(jsonObject.get("is_baseline")));
        } else {
            this.isBaseline = false;
        }

        dispatcherConfig = new DispatcherConfig();
        if (jsonObject.containsKey("full_inference_interval")) {
            dispatcherConfig.FULL_INFERENCE_INTERVAL = getInt(jsonObject, "full_inference_interval");
        }

        roIExtractorConfig = new RoIExtractorConfig();
        if (jsonObject.containsKey("batch_size")) { // if is_baseline = true, BATCH_SIZE must be 1
            roIExtractorConfig.BATCH_SIZE = getInt(jsonObject, "batch_size");
        }
        if (jsonObject.containsKey("mixed_frame_size")) {
            roIExtractorConfig.MIXED_FRAME_SIZE = getInt(jsonObject, "mixed_frame_size");
        }
        if (jsonObject.containsKey("merge_threshold")) {
            roIExtractorConfig.MERGE_THRESHOLD = getFloat(jsonObject, "merge_threshold");
        }
        if (jsonObject.containsKey("roi_padding")) {
            roIExtractorConfig.ROI_PADDING = getInt(jsonObject, "roi_padding");
        }
        if (jsonObject.containsKey("extraction_method")) {
            String method = String.valueOf(jsonObject.get("extraction_method"));
            switch (method) {
                case "combined":
                    roIExtractorConfig.EXTRACTION_METHOD = ExtractionMethod.COMBINED;
                    break;
                case "of":
                    roIExtractorConfig.EXTRACTION_METHOD = ExtractionMethod.OF;
                    break;
                case "pd":
                    roIExtractorConfig.EXTRACTION_METHOD = ExtractionMethod.PD;
                    break;
            }
        }

        inferenceEngineConfig = new InferenceEngineConfig();
        if (jsonObject.containsKey("frame_size")) {
            inferenceEngineConfig.FRAME_SIZE = getInt(jsonObject, "frame_size");
        }
        if (jsonObject.containsKey("num_workers")) {
            inferenceEngineConfig.NUM_WORKERS = getInt(jsonObject, "num_workers");
        }

        patchReconstructorConfig = new PatchReconstructorConfig();
        if (jsonObject.containsKey("match_padding")) {
            patchReconstructorConfig.MATCH_PADDING = getInt(jsonObject, "match_padding");
        }
        if (jsonObject.containsKey("use_iou_threshold")) {
            patchReconstructorConfig.USE_IOU_THRESHOLD = getFloat(jsonObject, "use_iou_threshold");
        }

        utilsConfig = new UtilsConfig();
        if (jsonObject.containsKey("minimum_confidence")) {
            utilsConfig.MINIMUM_CONFIDENCE = getFloat(jsonObject, "minimum_confidence");
        }
        validate();
    }

    private void validate() throws IllegalArgumentException {
        if (!isBaseline && roIExtractorConfig.MIXED_FRAME_SIZE != inferenceEngineConfig.FRAME_SIZE) {
            throw new IllegalArgumentException("roIExtractorConfig.MIXED_FRAME_SIZE and inferenceEngineConfig.MIXED_FRAME_SIZE must be same");
        }
        if (isBaseline && roIExtractorConfig.BATCH_SIZE != 1) {
            throw new IllegalArgumentException("if is_baseline = true, roIExtractorConfig.BATCH_SIZE must be 1");
        }
    }

    private static int getInt(JSONObject jsonObject, String key) {
        return Integer.parseInt(String.valueOf(jsonObject.get(key)));
    }

    private static float getFloat(JSONObject jsonObject, String key) {
        return Float.parseFloat(String.valueOf(jsonObject.get(key)));
    }
}
