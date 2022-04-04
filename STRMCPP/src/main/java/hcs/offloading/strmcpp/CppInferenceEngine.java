package hcs.offloading.strmcpp;

import android.util.Log;

import org.opencv.core.Mat;

import java.util.List;

public class CppInferenceEngine {
    private static final String TAG = "STRMCPP_JAVA";

    static {
        System.loadLibrary("strmcpp");
        System.loadLibrary("opencv_core");
        System.loadLibrary("opencv_dnn");
        System.loadLibrary("opencv_video");
        System.loadLibrary("opencv_imgcodecs");
        System.loadLibrary("opencv_imgproc");
        System.loadLibrary("MNN");
        System.loadLibrary("MNN_CL");
        System.loadLibrary("MNN_Express");
    }

    private final long handle;

    public CppInferenceEngine() {
        Log.d(TAG, "CppInferenceEngine()");
        handle = createCppInferenceEngine();
    }

    public int enqueue(Mat mat) {
        Log.d(TAG, "enqueue(Mat(" + mat.width() + ", " + mat.height() + ", " + mat.channels() + ")");
        return enqueue(handle, mat.getNativeObjAddr());
    }

    public List<BoundingBox> getResults(int requestHandle) {
        List<BoundingBox> results = getResults(handle, requestHandle);
        Log.d(TAG, "getResults() : " + results.size());
        return results;
    }

    private native long createCppInferenceEngine();

    private native int enqueue(long handle, long mat);

    private native List<BoundingBox> getResults(long handle, int requestHandle);
}
