package hcs.offloading.edgeserver.config;

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class Config {
    private static final String TAG = Config.class.getName();

    public final DispatcherConfig dispatcherConfig = new DispatcherConfig();
    public final RoIExtractorConfig roIExtractorConfig = new RoIExtractorConfig();
    public final InferenceEngineConfig inferenceEngineConfig = new InferenceEngineConfig();
    public final PatchReconstructorConfig patchReconstructorConfig = new PatchReconstructorConfig();
    public final UtilsConfig utilsConfig = new UtilsConfig();

    public Config(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(getStringFromFile(jsonPath));
        Log.d(TAG, "Parsed Config: " + jsonObject);

        if (jsonObject.has("full_inference_interval")) {
            dispatcherConfig.FULL_INFERENCE_INTERVAL = jsonObject.getInt("full_inference_interval");
        }

        if (jsonObject.has("is_baseline")) { // if is_baseline = true, BATCH_SIZE must be 1
            roIExtractorConfig.IS_BASELINE = jsonObject.getBoolean("is_baseline");
            roIExtractorConfig.BATCH_SIZE = 1;
        } else if (jsonObject.has("batch_size")) { // else, use "batch_size"
            roIExtractorConfig.BATCH_SIZE = jsonObject.getInt("batch_size");
        }
        if (jsonObject.has("frame_size")) {
            roIExtractorConfig.MIXED_FRAME_SIZE = jsonObject.getInt("frame_size");
        }
        if (jsonObject.has("merge_threshold")) {
            roIExtractorConfig.MERGE_THRESHOLD = (float) jsonObject.getDouble("merge_threshold");
        }
        if (jsonObject.has("roi_padding")) {
            roIExtractorConfig.ROI_PADDING = jsonObject.getInt("roi_padding");
        }
        if (jsonObject.has("extraction_method")) {
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

        if (jsonObject.has("num_workers")) {
            inferenceEngineConfig.NUM_WORKERS = jsonObject.getInt("num_workers");
        }
        if (jsonObject.has("frame_size")) {
            inferenceEngineConfig.FRAME_SIZE = jsonObject.getInt("frame_size");
        }

        if (jsonObject.has("match_padding")) {
            patchReconstructorConfig.MATCH_PADDING = jsonObject.getInt("match_padding");
        }
        if (jsonObject.has("use_iou_threshold")) {
            patchReconstructorConfig.USE_IOU_THRESHOLD = (float) jsonObject.getDouble("use_iou_threshold");
        }

        if (jsonObject.has("minimum_confidence")) {
            utilsConfig.MINIMUM_CONFIDENCE = (float) jsonObject.getDouble("minimum_confidence");
        }

        validate();
    }

    private void validate() throws IllegalArgumentException {
        if (!roIExtractorConfig.IS_BASELINE && roIExtractorConfig.MIXED_FRAME_SIZE != inferenceEngineConfig.FRAME_SIZE) {
            throw new IllegalArgumentException("roIExtractorConfig.MIXED_FRAME_SIZE and inferenceEngineConfig.MIXED_FRAME_SIZE must be same");
        }
        if (roIExtractorConfig.IS_BASELINE && roIExtractorConfig.BATCH_SIZE != 1) {
            throw new IllegalArgumentException("if is_baseline = true, roIExtractorConfig.BATCH_SIZE must be 1");
        }
    }

    private static String convertStreamToString(InputStream is) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            sb.append(line).append("\n");
        }
        reader.close();
        return sb.toString();
    }

    private static String getStringFromFile(String filePath) throws IOException {
        File fl = new File(filePath);
        FileInputStream fin = new FileInputStream(fl);
        String ret = convertStreamToString(fin);
        fin.close();
        return ret;
    }
}
