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
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

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
    }

    private static class VideoConfig {
        int numStreams;
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

        List<VideoConfig> videoConfigs = parseVideoConfigs();
        int startVid = 0;
        for (VideoConfig config : videoConfigs) {
            videoLoaders.add(new VideoLoader(startVid, config.numStreams, config.path, config.fps, this));
            startVid += config.numStreams;
        }
        int numVideos = startVid;
        handle = createHandle(numVideos);
        for (VideoLoader videoLoader : videoLoaders) {
            videoLoader.start();
        }
    }

    @Override
    public void onFrame(int vid, Mat yuvMat) {
        enqueue(handle, vid, yuvMat.getNativeObjAddr());
    }

    @Override
    public void onVideoEnd() {
        dumpLogs(handle);
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
            assert (videoConfigJson.has("num_streams")
                    && videoConfigJson.has("path")
                    && videoConfigJson.has("fps"));
            VideoConfig videoConfig = new VideoConfig();
            videoConfig.numStreams = videoConfigJson.getInt("num_streams");
            videoConfig.path = videoConfigJson.getString("path");
            videoConfig.fps = videoConfigJson.getInt("fps");
            videoConfigs.add(videoConfig);
        }
        return videoConfigs;
    }

    public void drawOutput(long rgbMatAddr, List<BoundingBox> results, long device) {
        Mat rgbMat = new Mat(rgbMatAddr);
//        if (device == 1) {
//            Imgproc.putText(rgbMat, "GPU", new Point(10, 80), 0, 3, new Scalar(0, 0, 255), 8);
//        } else if (device == 2) {
//            Imgproc.putText(rgbMat, "DSP", new Point(10, 80), 0, 3, new Scalar(255, 0, 0), 8);
//        } else {
//            assert(false);
//        }
        Bitmap bitmap = Bitmap.createBitmap(rgbMat.cols(), rgbMat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(rgbMat, bitmap);
        Bitmap outputBitmap = ImageUtils.drawBoxes(bitmap, results);
        outputView.post(() -> {
            outputView.setImageBitmap(outputBitmap);
            rgbMat.release();
        });
    }

    private native long createHandle(int numVideos);

    private native void enqueue(long handle, int vid, long yuvMatAddr);

    private native void dumpLogs(long handle);

    private native void close(long handle);
}
