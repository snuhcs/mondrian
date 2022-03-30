package hcs.offloading.strm;

import hcs.offloading.strm.datatypes.RoI;

public interface RoIPrioritizer {
    int priority(RoI roi);
}
