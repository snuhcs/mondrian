package hcs.offloading.edgeserver.config;

import android.util.Log;

import org.json.JSONArray;
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

        if (jsonObject.has("use_local_video")) {
            dispatcherConfig.USE_LOCAL_VIDEO = jsonObject.getBoolean("use_local_video");
            if (dispatcherConfig.USE_LOCAL_VIDEO) {
                if (!jsonObject.has("video_configs")) {
                    throw new IllegalArgumentException("Video configs should be specified");
                }
                JSONArray videoConfigs = jsonObject.getJSONArray("video_configs");
                for (int i = 0; i < videoConfigs.length(); i++) {
                    DispatcherConfig.VideoConfig videoConfig = new DispatcherConfig.VideoConfig();
                    JSONObject jsonVideoConfig = videoConfigs.getJSONObject(i);
                    if (jsonVideoConfig.has("path")) {
                        videoConfig.PATH = jsonVideoConfig.getString("path");
                    }
                    if (jsonVideoConfig.has("width")) {
                        videoConfig.WIDTH = jsonVideoConfig.getInt("width");
                    }
                    if (jsonVideoConfig.has("height")) {
                        videoConfig.HEIGHT = jsonVideoConfig.getInt("height");
                    }
                    if (jsonVideoConfig.has("fps")) {
                        videoConfig.FPS = jsonVideoConfig.getInt("fps");
                    }
                    dispatcherConfig.VIDEO_CONFIGS.add(videoConfig);
                }
            }
        }

        if (jsonObject.has("is_baseline")) { // if is_baseline = true, BATCH_SIZE must be 1
            roIExtractorConfig.IS_BASELINE = jsonObject.getBoolean("is_baseline");
        }
        if (jsonObject.has("batch_size")) { // else, use "batch_size"
            roIExtractorConfig.BATCH_SIZE = jsonObject.getInt("batch_size");
        }
        if (jsonObject.has("full_inference_batch_interval")) {
            roIExtractorConfig.FULL_INFERENCE_BATCH_INTERVAL = jsonObject.getInt("full_inference_batch_interval");
        }
        roIExtractorConfig.MAX_QUEUED_FRAMES = roIExtractorConfig.BATCH_SIZE * 2;
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
        if (jsonObject.has("person_threshold")) {
            roIExtractorConfig.PERSON_THRESHOLD = jsonObject.getInt("person_threshold");
        }
        if (jsonObject.has("class_agnostic_threshold")) {
            roIExtractorConfig.CLASS_AGNOSTIC_THRESHOLD = jsonObject.getInt("class_agnostic_threshold");
        }

        if (jsonObject.has("num_workers")) {
            inferenceEngineConfig.NUM_WORKERS = jsonObject.getInt("num_workers");
        }
        if (jsonObject.has("frame_size")) {
            inferenceEngineConfig.FRAME_SIZE = jsonObject.getInt("frame_size");
        }
        if (jsonObject.has("full_frame_size")) {
            inferenceEngineConfig.FULL_FRAME_SIZE = jsonObject.getInt("full_frame_size");
        } else {
            inferenceEngineConfig.FULL_FRAME_SIZE = inferenceEngineConfig.FRAME_SIZE;
        }
        inferenceEngineConfig.MAX_QUEUED_REQUESTS = inferenceEngineConfig.NUM_WORKERS * 2;
        if (jsonObject.has("per_roi_keep_ratio")) {
            inferenceEngineConfig.PER_ROI_KEEP_RATIO = jsonObject.getBoolean("per_roi_keep_ratio");
        }

        if (jsonObject.has("match_padding")) {
            patchReconstructorConfig.MATCH_PADDING = jsonObject.getInt("match_padding");
        }
        if (jsonObject.has("use_iou_threshold")) {
            patchReconstructorConfig.USE_IOU_THRESHOLD = (float) jsonObject.getDouble("use_iou_threshold");
        }
        if (jsonObject.has("log_path")) {
            patchReconstructorConfig.LOG_PATH = jsonObject.getString("log_path");
        }

        if (jsonObject.has("minimum_confidence")) {
            utilsConfig.MINIMUM_CONFIDENCE = (float) jsonObject.getDouble("minimum_confidence");
        }

        validate();
    }

    private void validate() throws IllegalArgumentException {
        if (dispatcherConfig.USE_LOCAL_VIDEO) {
            for (DispatcherConfig.VideoConfig videoConfig : dispatcherConfig.VIDEO_CONFIGS) {
                if (videoConfig.PATH == null) {
                    throw new IllegalArgumentException("All video paths should be specified");
                }
            }
        }
        if (!roIExtractorConfig.IS_BASELINE && roIExtractorConfig.MIXED_FRAME_SIZE != inferenceEngineConfig.FRAME_SIZE) {
            throw new IllegalArgumentException("roIExtractorConfig.MIXED_FRAME_SIZE and inferenceEngineConfig.MIXED_FRAME_SIZE must be same");
        }
//        if (roIExtractorConfig.IS_BASELINE && roIExtractorConfig.BATCH_SIZE != 1) {
//            throw new IllegalArgumentException("if is_baseline = true, roIExtractorConfig.BATCH_SIZE must be 1");
//        }
        if (roIExtractorConfig.MAX_QUEUED_FRAMES != -1 && roIExtractorConfig.MAX_QUEUED_FRAMES <= roIExtractorConfig.BATCH_SIZE) {
            throw new IllegalArgumentException("roIExtractorConfig.MAX_QUEUED_FRAMES should be larger then roIExtractorConfig.BATCH_SIZE");
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
