package hcs.offloading.strm.config;

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import hcs.offloading.strm.STRMUtils;

public class STRMConfig {
    private static final String TAG = STRMConfig.class.getName();

    public final DispatcherConfig dispatcherConfig;
    public final RoIExtractorConfig roIExtractorConfig;
    public final PatchMixerConfig patchMixerConfig;
    public final PatchReconstructorConfig patchReconstructorConfig;

    public STRMConfig(String jsonPath) throws IOException, JSONException {
        JSONObject jsonObject = new JSONObject(STRMUtils.getStringFromFile(jsonPath));
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

        if (jsonObject.has("patch_reconstructor")) {
            patchReconstructorConfig = new PatchReconstructorConfig(jsonObject.getJSONObject("patch_reconstructor"));
        } else {
            patchReconstructorConfig = new PatchReconstructorConfig();
        }
    }
}
