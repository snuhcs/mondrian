package hcs.offloading.mondrian;

import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;

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

import java.io.File;
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

    private static final String CONFIG_PATH = "/data/local/tmp/config.json";

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

        List<VideoLoader.VideoConfig> videoConfigs = parseVideoConfigs();
        for (VideoLoader.VideoConfig config : videoConfigs) {
            videoLoaders.add(new VideoLoader(config, this));
        }


        int numVideos = videoConfigs.stream()
                .map(config -> config.numStreams)
                .reduce(0, Integer::sum);
        int numTotalFrames = videoConfigs.stream()
                .map(config -> config.numStreams * config.numFrames)
                .reduce(0, Integer::sum);

        totalFramesView.setText(String.format("%04d", numTotalFrames));

        String logDir = parseAndCreateLogDir();
        Files.copy(Paths.get(CONFIG_PATH), Paths.get(logDir + "/" + "config.json"), REPLACE_EXISTING);

        handle = createHandle(logDir, numVideos, numTotalFrames);

        for (VideoLoader videoLoader : videoLoaders) {
            videoLoader.start();
        }
    }

    @Override
    public void onFrame(int vid, Mat yuvMat) {
        mondrianUI.onFrame(vid, yuvMat);
        enqueue(handle, vid, yuvMat.getNativeObjAddr());
    }

    public void drawOutput(long rgbMatAddr, List<BoundingBox> results, long device) {
        mondrianUI.drawOutput(rgbMatAddr, results, device);
    }

    public void close() {
        for (VideoLoader videoLoader : videoLoaders) {
            videoLoader.close();
        }
        close(handle);
    }

    private static String parseAndCreateLogDir() throws JSONException, IOException {
        String jsonStr = new String(Files.readAllBytes(Paths.get(CONFIG_PATH)));
        JSONObject configJson = new JSONObject(jsonStr);
        assert (configJson.has("log_dir"));
        String logDirPath = configJson.getString("log_dir");
        File logDir = new File(logDirPath);
        logDir.mkdirs();
        assert (logDir.exists());
        return logDirPath;
    }

    private static List<VideoLoader.VideoConfig> parseVideoConfigs() throws JSONException, IOException {
        String jsonStr = new String(Files.readAllBytes(Paths.get(CONFIG_PATH)));
        JSONObject configJson = new JSONObject(jsonStr);
        JSONArray videoConfigsJson = configJson.getJSONArray("video_configs");

        List<VideoLoader.VideoConfig> videoConfigs = new ArrayList<>();
        for (int i = 0; i < videoConfigsJson.length(); i++) {
            JSONObject videoConfigJson = videoConfigsJson.getJSONObject(i);
            assert (videoConfigJson.has("num_streams")
                    && videoConfigJson.has("path")
                    && videoConfigJson.has("fps"));
            VideoLoader.VideoConfig videoConfig = new VideoLoader.VideoConfig();
            videoConfig.numStreams = videoConfigJson.getInt("num_streams");
            videoConfig.path = videoConfigJson.getString("path");
            videoConfig.numFrames = videoConfigJson.getInt("num_frames");
            videoConfig.fps = videoConfigJson.getInt("fps");
            assert (0 < videoConfig.numStreams);
            assert (0 < videoConfig.numFrames);
            assert (0 <= videoConfig.fps);
            videoConfigs.add(videoConfig);
        }
        return videoConfigs;
    }

    private native long createHandle(String logDir, int numVideos, int numTotalFrames);

    private native void enqueue(long handle, int vid, long yuvMatAddr);

    private native void close(long handle);
}
