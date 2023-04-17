package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.util.Log;
import android.util.Pair;
import android.widget.ImageView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class Emulator implements VideoLoader.Callback {
    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");

        System.loadLibrary("strm");
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
        Pair<Integer, Integer> frame_range;
        int fps;
    }

    private static final String TAG = Emulator.class.getName();
    private static final String VIDEO_CONFIG_PATH = "/data/local/tmp/strm.json";

    private final long handle;
    private final ImageView outputView;
    private final List<VideoLoader> videoLoaders = new ArrayList<>();

    public Emulator(ImageView outputView) throws JSONException, IOException {
        this.outputView = outputView;
        handle = createSpatioTemporalRoIMixer();

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
        List<VideoConfig> videoConfigs = new ArrayList<>();
        JSONObject jsonObject = getConfigJson();
        JSONArray jsonVideoConfigs = jsonObject.getJSONArray("video_configs");
        for (int i = 0; i < jsonVideoConfigs.length(); i++) {
            VideoConfig videoConfig = new VideoConfig();
            JSONObject jsonVideoConfig = jsonVideoConfigs.getJSONObject(i);
            if (jsonVideoConfig.has("path")) {
                videoConfig.path = jsonVideoConfig.getString("path");
            }
            if (jsonVideoConfig.has("frame_range")) {
                JSONArray frame_range = jsonVideoConfig.getJSONArray("frame_range");
                if (frame_range.length() != 2) {
                    throw new JSONException("Frame range should contain only start index and end index");
                }
                videoConfig.frame_range = new Pair<>(frame_range.getInt(0), frame_range.getInt(1));
            }
            if (jsonVideoConfig.has("fps")) {
                videoConfig.fps = jsonVideoConfig.getInt("fps");
            }
            videoConfigs.add(videoConfig);
        }
        return videoConfigs;
    }

    private static JSONObject getConfigJson() throws IOException, JSONException {
        File fl = new File(VIDEO_CONFIG_PATH);
        FileInputStream fin = new FileInputStream(fl);
        String jsonStr = convertStreamToString(fin);
        fin.close();
        return new JSONObject(jsonStr);
    }

    private static String convertStreamToString(InputStream is) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();
        String line;
        while ((line = reader.readLine()) != null) {
            sb.append(line).append("\n");
        }
        reader.close();
        return sb.toString();
    }

    public void drawOutput(long rgbMatAddr, List<BoundingBox> results) {
        Mat rgbMat = new Mat(rgbMatAddr);
        Bitmap bitmap = Bitmap.createBitmap(rgbMat.cols(), rgbMat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(rgbMat, bitmap);
        Bitmap outputBitmap = ImageUtils.drawBoxes(bitmap, results, false);
        outputView.post(() -> outputView.setImageBitmap(outputBitmap));
    }

    private native long createSpatioTemporalRoIMixer();

    private native void enqueueImage(long handle, int vid, long yuvMatAddr);

    private native void close(long handle);
}
