package hcs.offloading.mondrian;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.support.annotation.NonNull;
import android.util.Log;
import android.util.Pair;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.ImageView;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

@SuppressLint("DefaultLocale")
public class MondrianApp implements VideoLoader.Callback, SurfaceHolder.Callback {
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
    private boolean isSurfaceSet = false;
    private final Size inputSize = new Size(640, 360);
    private final List<SurfaceView> inputViews;
    private final List<Mat> inputRgbMats = new ArrayList<>();
    private final List<Mat> inputResizedRgbMats = new ArrayList<>();
    private final List<byte[]> inputBuffers = new ArrayList<>();

    private final ImageView outputView;

    private final List<Pair<Integer, Long>> countTimes = new LinkedList<>();
    private final AtomicInteger frameCount = new AtomicInteger(0);
    private final TextView fpsView;
    private final TextView frameCountView;

    private final List<VideoLoader> videoLoaders = new ArrayList<>();

    public MondrianApp(List<SurfaceView> inputViews, ImageView outputView, TextView fpsView, TextView frameCountView, TextView totalFramesView) throws JSONException, IOException {
        this.inputViews = inputViews;
        for (SurfaceView inputView : inputViews) {
            inputView.getHolder().addCallback(this);
        }
        for (int i = 0; i < inputViews.size(); i++) {
            inputRgbMats.add(new Mat());
            inputResizedRgbMats.add(new Mat());
            inputBuffers.add(new byte[(int) (4 * inputSize.width * inputSize.height)]);
        }
        this.outputView = outputView;
        this.fpsView = fpsView;
        this.frameCountView = frameCountView;

        List<VideoConfig> videoConfigs = parseVideoConfigs();
        int totalFrames = 0;
        int startVid = 0;
        for (VideoConfig config : videoConfigs) {
            VideoLoader videoLoader = new VideoLoader(startVid, config.numStreams, config.path, config.fps, this);
            videoLoaders.add(videoLoader);
            startVid += config.numStreams;
            totalFrames += config.numStreams * videoLoader.numFrames;
        }
        totalFramesView.setText(String.format("%04d", totalFrames));
        int numVideos = startVid;
        handle = createHandle(numVideos);
        for (VideoLoader videoLoader : videoLoaders) {
            videoLoader.start();
        }
    }

    @Override
    public void onFrame(int vid, Mat yuvMat) {
        int count = frameCount.incrementAndGet();
        frameCountView.post(() -> frameCountView.setText(String.format("%04d", count)));
        if (count % 10 == 0) {
            updateFPS(count);
        }
        if (isSurfaceSet && vid < inputViews.size()) {
            Mat inputRgbMat = inputRgbMats.get(vid);
            Mat inputResizedRgbMat = inputResizedRgbMats.get(vid);
            byte[] inputBuffer = inputBuffers.get(vid);
            SurfaceView inputView = inputViews.get(vid);
            Imgproc.cvtColor(yuvMat, inputRgbMat, Imgproc.COLOR_YUV2RGBA_NV12);
            Imgproc.resize(inputRgbMat, inputResizedRgbMat, inputSize, 0, 0, Imgproc.INTER_LINEAR);
            inputResizedRgbMat.get(0, 0, inputBuffer);
            JniRenderer.draw(inputView.getHolder(), inputBuffer, (int) inputSize.width, (int) inputSize.height);
        }
        enqueue(handle, vid, yuvMat.getNativeObjAddr());
    }

    private void updateFPS(int frameCount) {
        long currTime = System.nanoTime();
        countTimes.add(new Pair<>(frameCount, currTime));
        if (countTimes.size() == 1) return;
        Pair<Integer, Long> first = countTimes.get(0);
        Pair<Integer, Long> last = countTimes.get(countTimes.size() - 1);
        double fps = 1e9 * (last.first - first.first) / (last.second - first.second);
        fpsView.post(() -> fpsView.setText(String.format("%02.1f", fps)));
        if (countTimes.size() > 50) {
            countTimes.remove(0);
        }
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
        if (device == 1) {
            Imgproc.putText(rgbMat, "GPU", new Point(10, 80), 0, 3, new Scalar(0, 0, 255), 8);
        } else if (device == 2) {
            Imgproc.putText(rgbMat, "DSP", new Point(10, 80), 0, 3, new Scalar(255, 0, 0), 8);
        } else {
            assert(false);
        }
        Bitmap bitmap = Bitmap.createBitmap(rgbMat.cols(), rgbMat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(rgbMat, bitmap);
        Bitmap outputBitmap = ImageUtils.drawBoxes(bitmap, results);
        outputView.post(() -> {
            outputView.setImageBitmap(outputBitmap);
            rgbMat.release();
        });
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        Log.d(TAG, "surface view created");
        for (SurfaceView inputView : inputViews) {
            inputView.getHolder().setFixedSize((int) inputSize.width, (int) inputSize.height);
        }
        isSurfaceSet = true;
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {}

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {}

    private native long createHandle(int numVideos);

    private native void enqueue(long handle, int vid, long yuvMatAddr);

    private native void dumpLogs(long handle);

    private native void close(long handle);
}
