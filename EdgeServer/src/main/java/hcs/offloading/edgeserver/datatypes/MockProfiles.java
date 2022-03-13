package hcs.offloading.edgeserver.datatypes;

public class MockProfiles {

    public final int PERSON_THRESHOLD;
    public final int CLASS_AGNOSTIC_THRESHOLD;

    public MockProfiles(int personThreshold, int classAgnosticThreshold) {
        PERSON_THRESHOLD = personThreshold;
        CLASS_AGNOSTIC_THRESHOLD = classAgnosticThreshold;
    }

    public int getProfile(String labelName) {
        if (labelName == null) {
            return CLASS_AGNOSTIC_THRESHOLD;
        } else if (labelName.equals("person")) {
            return PERSON_THRESHOLD;
        } else {
            throw new IllegalArgumentException("Wrong labelName!");
        }
    }
}
