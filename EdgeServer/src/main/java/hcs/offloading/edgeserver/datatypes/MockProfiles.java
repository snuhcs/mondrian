package hcs.offloading.edgeserver.datatypes;

public class MockProfiles {

    public static int PERSON_THRESHOLD = 10000;
    public static int CLASS_AGNOSTIC_THRESHOLD = 15000;

    public static int get_profile(String labelName) {
        switch (labelName != null ? labelName : "NULL") {
            case "person":
                return PERSON_THRESHOLD;
            default:
                return CLASS_AGNOSTIC_THRESHOLD;
        }
    }
}
