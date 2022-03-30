package hcs.offloading.edgedevice.roiprioritizer;

import hcs.offloading.strm.RoIPrioritizer;
import hcs.offloading.strm.datatypes.RoI;

public class CustomRoIPrioritizer implements RoIPrioritizer {
    @Override
    public int priority(RoI roi) {
        return roi.location.width() * roi.location.height();
    }
}
