package hcs.offloading.scheduler;

import java.util.Random;
import java.util.Set;

public class Scheduler {
    private static final String TAG = Scheduler.class.getName();

    static String schedule(Set<String> edgeIps) {
        // TODO: Schedule with edge information
        if (edgeIps == null || edgeIps.isEmpty()) {
            return null;
        }
        Random rand = new Random();
        int edgeIdx = rand.nextInt(edgeIps.size());
        return (String) edgeIps.toArray()[edgeIdx];
    }
}
