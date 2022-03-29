package hcs.offloading.strmcpp;

import hcs.offloading.strm.datatypes.RoI;

public interface RoIPrioritizer {
    int priority(RoI roi);
}
