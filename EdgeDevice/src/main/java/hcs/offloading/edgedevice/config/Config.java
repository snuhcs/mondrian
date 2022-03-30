package hcs.offloading.edgedevice.config;

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import hcs.offloading.edgedevice.Utils;

public class Config {
    private static final String TAG = Config.class.getName();

    public final DispatcherConfig dispatcherConfig;
    public final RoIExtractorConfig roIExtractorConfig;
    public final PatchMixerConfig patchMixerConfig;
    public final InferenceEngineConfig inferenceEngineConfig;
    public final PatchReconstructorConfig patchReconstructorConfig;

    public Config(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(Utils.getStringFromFile(jsonPath));
        Log.d(TAG, "Parsed Config: " + jsonObject);

        if (jsonObject.has("dispatcher")) {
            dispatcherConfig = new DispatcherConfig(jsonObject.getJSONObject("dispatcher"));
        } else {
            dispatcherConfig = new DispatcherConfig();
        }

        if (jsonObject.has("roi_extractor")) {
            roIExtractorConfig = new RoIExtractorConfig(jsonObject.getJSONObject("roi_extractor"));
        } else {
            roIExtractorConfig = new RoIExtractorConfig();
        }

        if (jsonObject.has("patch_mixer")) {
            patchMixerConfig = new PatchMixerConfig(jsonObject.getJSONObject("patch_mixer"));
        } else {
            patchMixerConfig = new PatchMixerConfig();
        }

        if (jsonObject.has("inference_engine")) {
            inferenceEngineConfig = new InferenceEngineConfig(jsonObject.getJSONObject("inference_engine"));
        } else {
            inferenceEngineConfig = new InferenceEngineConfig();
        }

        if (jsonObject.has("patch_reconstructor")) {
            patchReconstructorConfig = new PatchReconstructorConfig(jsonObject.getJSONObject("patch_reconstructor"));
        } else {
            patchReconstructorConfig = new PatchReconstructorConfig();
        }
    }
}
