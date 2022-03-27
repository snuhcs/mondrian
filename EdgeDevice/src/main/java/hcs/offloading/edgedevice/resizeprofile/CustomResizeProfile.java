package hcs.offloading.edgedevice.resizeprofile;

import android.util.Log;

import hcs.offloading.edgedevice.config.ResizeProfileConfig;
import hcs.offloading.strm.ResizeProfile;

public class CustomResizeProfile implements ResizeProfile {
    private static final String TAG = CustomResizeProfile.class.getName();

    private final ResizeProfileConfig mConfig;

    public CustomResizeProfile(ResizeProfileConfig config) {
        mConfig = config;
    }

    @Override
    public float getScale(String labelName, int width, int height, int minOriginLength) {
        if (labelName != null && !labelName.equals("person")) {
            Log.e(TAG, "Wrong labelName: " + labelName);
        }
        int lengthThreshold = labelName == null ? mConfig.CLASS_AGNOSTIC_THRESHOLD : mConfig.PERSON_THRESHOLD;
        int maxWidthHeight = mConfig.MERGED_RESIZE
                ? minOriginLength
                : Math.max(width, height);
        if (mConfig.FIT_RESIZE || maxWidthHeight > lengthThreshold) {
            return (float) lengthThreshold / maxWidthHeight;
        }
        return 1f;
    }
}
