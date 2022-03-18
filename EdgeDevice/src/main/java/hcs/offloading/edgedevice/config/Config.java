package hcs.offloading.edgedevice.config;

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

    public Config(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(getStringFromFile(jsonPath));
        Log.d(TAG, "Parsed Config: " + jsonObject);

        if (jsonObject.has("use_local_video")) {
            dispatcherConfig.USE_LOCAL_VIDEO = jsonObject.getBoolean("use_local_video");
            if (dispatcherConfig.USE_LOCAL_VIDEO) {
                if (!jsonObject.has("video_configs") || jsonObject.getJSONArray("video_configs").length() == 0) {
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

        if (jsonObject.has("packing")) {
            roIExtractorConfig.PACKING = jsonObject.getBoolean("packing");
        }
        if (jsonObject.has("full_inference_interval")) {
            roIExtractorConfig.FULL_INFERENCE_INTERVAL = jsonObject.getInt("full_inference_interval");
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
                    roIExtractorConfig.EXTRACTION_METHOD = RoIExtractorConfig.Method.COMBINED;
                    break;
                case "of":
                    roIExtractorConfig.EXTRACTION_METHOD = RoIExtractorConfig.Method.OF;
                    break;
                case "pd":
                    roIExtractorConfig.EXTRACTION_METHOD = RoIExtractorConfig.Method.PD;
                    break;
            }
        }
        if (jsonObject.has("person_threshold")) {
            roIExtractorConfig.PERSON_THRESHOLD = jsonObject.getInt("person_threshold");
        }
        if (jsonObject.has("class_agnostic_threshold")) {
            roIExtractorConfig.CLASS_AGNOSTIC_THRESHOLD = jsonObject.getInt("class_agnostic_threshold");
        }

        if (jsonObject.has("use_yolo_v4")) {
            inferenceEngineConfig.USE_YOLO_V4 = jsonObject.getBoolean("use_yolo_v4");
        }
        if (jsonObject.has("use_tiny")) {
            inferenceEngineConfig.USE_TINY = jsonObject.getBoolean("use_tiny");
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
        if (jsonObject.has("draw_confidence")) {
            patchReconstructorConfig.DRAW_CONFIDENCE = (float) jsonObject.getDouble("draw_confidence");
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
