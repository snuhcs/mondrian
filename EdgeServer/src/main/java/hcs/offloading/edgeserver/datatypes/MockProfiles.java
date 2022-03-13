package hcs.offloading.edgeserver.datatypes;

public class MockProfiles {

    public static int PERSON_THRESHOLD = 10000;
    public static int CLASS_AGNOSTIC_THRESHOLD = 15000;

    public static int get_profile(String labelName) {
        if (labelName == null) {
            return CLASS_AGNOSTIC_THRESHOLD;
        } else if (labelName.equals("person") {
            return PERSON_THRESHOLD;
        } else {
            throw new IllegalArgumentException("Wrong labelName!");
        }
    }
}
