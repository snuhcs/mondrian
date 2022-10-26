package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.media.MediaMetadataRetriever;
import android.util.Log;
import android.util.Pair;

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

public class Emulator {
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
        String PATH;
        Pair<Integer, Integer> FRAME_RANGE;
        int FPS = 0;
    }

    private static final String TAG = Emulator.class.getName();

    private final long handle;
    private final InferenceViewCallback inferenceViewCallback;
    private final List<Thread> videoThreads;

    public Emulator(InferenceViewCallback inferenceViewCallback) throws JSONException, IOException {
        this.inferenceViewCallback = inferenceViewCallback;
        handle = createSpatioTemporalRoIMixer();

        videoThreads = createAndStartVideoThreads(parseVideoConfigs("/data/local/tmp/strm.json"));
    }

    public void close() {
        try {
            for (Thread videoThread : videoThreads) {
                videoThread.join();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        close(handle);
    }

    private List<Thread> createAndStartVideoThreads(List<VideoConfig> videoConfigs) {
        List<Thread> videoThreads = new ArrayList<>();
        for (int videoIndex = 0; videoIndex < videoConfigs.size(); videoIndex++) {
            VideoConfig config = videoConfigs.get(videoIndex);
            int vid = videoIndex;
            videoThreads.add(new Thread(() -> {
                MediaMetadataRetriever retriever = new MediaMetadataRetriever();
                retriever.setDataSource(config.PATH);
                long startTimeNs = System.nanoTime();
                int frameCount = Integer.parseInt(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
                int startIndex = config.FRAME_RANGE.first;
                int endIndex = config.FRAME_RANGE.second == -1 ? frameCount : config.FRAME_RANGE.second;
                for (int frameIndex = startIndex; frameIndex < endIndex; frameIndex++) {
                    Log.v(TAG, "Video " + vid + " frame " + frameIndex + " loaded");
                    Bitmap bitmap = retriever.getFrameAtIndex(frameIndex);
                    Mat mat = new Mat();
                    Utils.bitmapToMat(bitmap, mat);
                    enqueueImage(handle, vid, mat.getNativeObjAddr());
                    if (config.FPS != 0) {
                        long endTimeNs = System.nanoTime();
                        long nextStartTimeNs = startTimeNs +
                                (long) (frameIndex - startIndex + 1) * (long) (1e9 / config.FPS);
                        if (nextStartTimeNs > endTimeNs) {
                            try {
                                Thread.sleep((nextStartTimeNs - endTimeNs) / 1000000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
            }));
        }
        for (Thread videoThread : videoThreads) {
            videoThread.start();
        }
        return videoThreads;
    }

    private List<VideoConfig> parseVideoConfigs(String configPath) throws JSONException, IOException {
        List<VideoConfig> videoConfigs = new ArrayList<>();
        JSONObject jsonObject = new JSONObject(getStringFromFile(configPath));
        JSONArray jsonVideoConfigs = jsonObject.getJSONArray("video_configs");
        for (int i = 0; i < jsonVideoConfigs.length(); i++) {
            VideoConfig videoConfig = new VideoConfig();
            JSONObject jsonVideoConfig = jsonVideoConfigs.getJSONObject(i);
            if (jsonVideoConfig.has("path")) {
                videoConfig.PATH = jsonVideoConfig.getString("path");
            }
            if (jsonVideoConfig.has("frame_range")) {
                JSONArray frame_range = jsonVideoConfig.getJSONArray("frame_range");
                if (frame_range.length() != 2) {
                    throw new JSONException("Frame range should contain only start index and end index");
                }
                videoConfig.FRAME_RANGE = new Pair<>(frame_range.getInt(0), frame_range.getInt(1));
            }
            if (jsonVideoConfig.has("fps")) {
                videoConfig.FPS = jsonVideoConfig.getInt("fps");
            }
            videoConfigs.add(videoConfig);
        }
        return videoConfigs;
    }

    private static String getStringFromFile(String filePath) throws IOException {
        File fl = new File(filePath);
        FileInputStream fin = new FileInputStream(fl);
        String ret = convertStreamToString(fin);
        fin.close();
        return ret;
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

    public void drawOutput0(long addr, List<BoundingBox> results) {
        inferenceViewCallback.drawOutput0(addr, results);
    }

    public void drawOutput1(long addr, List<BoundingBox> results) {
        inferenceViewCallback.drawOutput1(addr, results);
    }

    private native long createSpatioTemporalRoIMixer();

    private native void enqueueImage(long handle, int vid, long matAddr);

    private native void close(long handle);
}
