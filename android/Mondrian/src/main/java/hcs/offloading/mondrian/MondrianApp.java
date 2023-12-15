package hcs.offloading.mondrian;

import android.annotation.SuppressLint;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.ImageView;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.opencv.android.OpenCVLoader;
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
    }

    private static class VideoConfig {
        int numStreams;
        String path;
        int fps;
    }

    private static final String VIDEO_CONFIG_PATH = "/data/local/tmp/config.json";

    private final long handle;
    private final List<VideoLoader> videoLoaders = new ArrayList<>();
    private final MondrianUI mondrianUI;

    @SuppressLint("DefaultLocale")
    public MondrianApp(List<SurfaceView> inputViews,
                       ImageView outputView,
                       TextView fpsView,
                       TextView frameCountView,
                       TextView totalFramesView) throws JSONException, IOException {
        mondrianUI = new MondrianUI(
                inputViews,
                outputView,
                fpsView,
                frameCountView);

        List<VideoConfig> videoConfigs = parseVideoConfigs();
        for (VideoConfig config : videoConfigs) {
            videoLoaders.add(new VideoLoader(config.numStreams, config.path, config.fps, this));
        }

        long totalFrames = videoLoaders.stream()
                .map(videoLoader -> videoLoader.numStreams * videoLoader.numFrames)
                .reduce(0L, Long::sum);
        totalFramesView.setText(String.format("%04d", totalFrames));

        int numVideos = videoLoaders.stream()
                .map(videoLoader -> videoLoader.numStreams)
                .reduce(0, Integer::sum);
        handle = createHandle(numVideos);

        for (VideoLoader videoLoader : videoLoaders) {
            videoLoader.start();
        }
    }

    @Override
    public void onFrame(int vid, Mat yuvMat) {
//        mondrianUI.onFrame(vid, yuvMat);
        enqueue(handle, vid, yuvMat.getNativeObjAddr());
    }

    public void drawOutput(long rgbMatAddr, List<BoundingBox> results, long device) {
        mondrianUI.drawOutput(rgbMatAddr, results, device);
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

    private native long createHandle(int numVideos);

    private native void enqueue(long handle, int vid, long yuvMatAddr);

    private native void dumpLogs(long handle);

    private native void close(long handle);
}
