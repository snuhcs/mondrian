package hcs.offloading.edgedevice.resizeprofile;

import hcs.offloading.strm.ResizeProfile;

public class CustomResizeProfile implements ResizeProfile {
    public final boolean FIT_RESIZE;
    public final boolean MERGED_RESIZE;
    public final int PERSON_THRESHOLD;
    public final int CLASS_AGNOSTIC_THRESHOLD;

    public CustomResizeProfile(boolean fitResize, boolean mergedResize,
                               int personThreshold, int classAgnosticThreshold) {
        FIT_RESIZE = fitResize;
        MERGED_RESIZE = mergedResize;
        PERSON_THRESHOLD = personThreshold;
        CLASS_AGNOSTIC_THRESHOLD = classAgnosticThreshold;
    }

    @Override
    public float getScale(String labelName, int width, int height, int minOriginLength) {
        assert labelName == null || labelName.equals("person");
        int lengthThreshold = labelName == null ? CLASS_AGNOSTIC_THRESHOLD : PERSON_THRESHOLD;
        int maxWidthHeight = MERGED_RESIZE
                ? minOriginLength
                : Math.max(width, height);
        if (FIT_RESIZE || maxWidthHeight > lengthThreshold) {
            return (float) lengthThreshold / maxWidthHeight;
        }
        return 1f;
    }
}
