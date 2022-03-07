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

    public final DispatcherConfig dispatcherConfig;
    public final RoIExtractorConfig roIExtractorConfig;
    public final InferenceEngineConfig inferenceEngineConfig;
    public final PatchReconstructorConfig patchReconstructorConfig;
    public final UtilsConfig utilsConfig;

    public Config(String jsonPath) throws ParseException, JSONException, IOException, IllegalArgumentException {
        JSONParser jsonParser = new JSONParser();
        JSONObject jsonObject = (JSONObject) jsonParser.parse(new FileReader(new File(jsonPath)));
        Log.d(TAG, "Parsed Config: " + jsonObject.toJSONString());

        dispatcherConfig = new DispatcherConfig();
        if (jsonObject.containsKey("batch_size")) {
            dispatcherConfig.BATCH_SIZE = getInt(jsonObject, "batch_size");
        }
        if (jsonObject.containsKey("full_inference_interval")) {
            dispatcherConfig.FULL_INFERENCE_INTERVAL = getInt(jsonObject, "full_inference_interval");
        }
        if (jsonObject.containsKey("num_frames_on_memory")) {
            dispatcherConfig.NUM_FRAMES_ON_MEMORY = getInt(jsonObject, "num_frames_on_memory");
        }

        roIExtractorConfig = new RoIExtractorConfig();
        if (jsonObject.containsKey("mixed_frame_size")) {
            roIExtractorConfig.MIXED_FRAME_SIZE = getInt(jsonObject, "mixed_frame_size");
        }
        if (jsonObject.containsKey("idle_wait_ms")) {
            roIExtractorConfig.IDLE_WAIT_MS = getInt(jsonObject, "idle_wait_ms");
        }
        if (jsonObject.containsKey("area_threshold")) {
            roIExtractorConfig.AREA_THRESHOLD = getInt(jsonObject, "area_threshold");
        }
        if (jsonObject.containsKey("roi_padding")) {
            roIExtractorConfig.ROI_PADDING = getInt(jsonObject, "roi_padding");
        }

        inferenceEngineConfig = new InferenceEngineConfig();
        inferenceEngineConfig.MIXED_FRAME_SIZE = roIExtractorConfig.MIXED_FRAME_SIZE;
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
        if (roIExtractorConfig.MIXED_FRAME_SIZE != inferenceEngineConfig.MIXED_FRAME_SIZE) {
            throw new IllegalArgumentException("roIExtractorConfig.MIXED_FRAME_SIZE and inferenceEngineConfig.MIXED_FRAME_SIZE must be same");
        }
    }

    private static int getInt(JSONObject jsonObject, String key) {
        return Integer.parseInt(String.valueOf(jsonObject.get(key)));
    }

    private static float getFloat(JSONObject jsonObject, String key) {
        return Float.parseFloat(String.valueOf(jsonObject.get(key)));
    }
}
