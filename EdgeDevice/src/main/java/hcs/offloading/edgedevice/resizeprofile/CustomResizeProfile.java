package hcs.offloading.edgedevice.resizeprofile;

import hcs.offloading.edgedevice.config.ResizeProfileConfig;
import hcs.offloading.strm.ResizeProfile;

public class CustomResizeProfile implements ResizeProfile {
    private final ResizeProfileConfig mConfig;

    public CustomResizeProfile(ResizeProfileConfig config) {
        mConfig = config;
    }

    @Override
    public float getScale(String labelName, int width, int height, int minOriginLength) {
        assert labelName == null || labelName.equals("person");
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
