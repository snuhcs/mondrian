package hcs.offloading.mondrian;

import android.graphics.Bitmap;
import android.util.Log;
import android.widget.ImageView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class MondrianApp implements VideoLoader.Callback {
    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");

        System.loadLibrary("mondrian");
        System.loadLibrary("opencv_core");
        System.loadLibrary("opencv_dnn");
        System.loadLibrary("opencv_video");
        System.loadLibrary("opencv_imgcodecs");
        System.loadLibrary("opencv_imgproc");
        System.loadLibrary("MNN");
        System.loadLibrary("MNN_CL");
        System.loadLibrary("MNN_Express");
    }

    private static class VideoConfig {
        String path;
        int fps;
    }

    private static final String TAG = MondrianApp.class.getName();
    private static final String VIDEO_CONFIG_PATH = "/data/local/tmp/config.json";

    private final long handle;
    private final ImageView outputView;
    private final List<VideoLoader> videoLoaders = new ArrayList<>();

    public MondrianApp(ImageView outputView) throws JSONException, IOException {
        this.outputView = outputView;
        handle = createHandle();

        List<VideoConfig> videoConfigs = parseVideoConfigs();
        for (int vid = 0; vid < videoConfigs.size(); vid++) {
            VideoConfig config = videoConfigs.get(vid);
            videoLoaders.add(new VideoLoader(vid, config.path, config.fps, this));
        }
    }

    @Override
    public void onFrame(int vid, Mat yuvMat) {
        enqueueImage(handle, vid, yuvMat.getNativeObjAddr());
    }

    public void close() {
        for (VideoLoader videoLoader : videoLoaders) {
            videoLoader.close();
        }
        close(handle);
    }

    private static List<VideoConfig> parseVideoConfigs() throws JSONException, IOException {
        String jsonStr = new String(Files.readAllBytes(Paths.get(VIDEO_CONFIG_PATH)));
        JSONObject configJson = new JSONObject(jsonStr);
        JSONArray videoConfigsJson = configJson.getJSONArray("video_configs");

        List<VideoConfig> videoConfigs = new ArrayList<>();
        for (int i = 0; i < videoConfigsJson.length(); i++) {
            JSONObject videoConfigJson = videoConfigsJson.getJSONObject(i);
            assert (videoConfigJson.has("path") && videoConfigJson.has("fps"));
            VideoConfig videoConfig = new VideoConfig();
            videoConfig.path = videoConfigJson.getString("path");
            videoConfig.fps = videoConfigJson.getInt("fps");
            videoConfigs.add(videoConfig);
        }
        return videoConfigs;
    }

    public void drawOutput(long rgbMatAddr, List<BoundingBox> results) {
        Mat rgbMat = new Mat(rgbMatAddr);
        Bitmap bitmap = Bitmap.createBitmap(rgbMat.cols(), rgbMat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(rgbMat, bitmap);
        Bitmap outputBitmap = ImageUtils.drawBoxes(bitmap, results);
        outputView.post(() -> outputView.setImageBitmap(outputBitmap));
    }

    private native long createHandle();

    private native void enqueueImage(long handle, int vid, long yuvMatAddr);

    private native void close(long handle);
}
