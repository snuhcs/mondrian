package hcs.offloading.mondrian;

public class Utils {
    public static native boolean schedSetAffinityPrimary();

    public static native boolean schedSetAffinityBig();

    public static native boolean schedSetAffinityBigOrPrimary();

    public static native boolean schedSetAffinityLittle();
}
