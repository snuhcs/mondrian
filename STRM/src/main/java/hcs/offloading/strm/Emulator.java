package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaMetadataRetriever;
import android.util.Log;
import android.util.Pair;
import android.widget.ImageView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class Emulator implements Runnable {
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
    private static final String IMPL_CONFIG_PATH = "/data/local/tmp/strm.json";

    private static final int TIMEOUT_US = 1000;
    private long startTime = -1;

    private long handle;
    private final ImageView outputView;
//    private final List<Thread> videoThreads;

    private MediaExtractor extractor;
    private MediaCodec decoder;
    private MediaFormat format;
    private int width;
    private int height;
    private int fps;

    public Emulator(ImageView outputView) {
        this.outputView = outputView;
//        handle = createSpatioTemporalRoIMixer();

//        videoThreads = createAndStartVideoThreads(parseVideoConfigs("/data/local/tmp/strm.json"));

        init();

        Thread thread = new Thread(this);
        thread.start();
    }

    private void init() {
        try {
            extractor = new MediaExtractor();
            extractor.setDataSource("/data/local/tmp/video/mta/test_cam_0.mp4");
            for (int trackIndex = 0; trackIndex < extractor.getTrackCount(); trackIndex++) {
                format = extractor.getTrackFormat(trackIndex);
                String mime = format.getString(MediaFormat.KEY_MIME);
                if (mime.startsWith("video/")) {
                    width = format.getInteger(MediaFormat.KEY_WIDTH);
                    height = format.getInteger(MediaFormat.KEY_HEIGHT);
                    fps = format.getInteger(MediaFormat.KEY_FRAME_RATE);

                    extractor.selectTrack(trackIndex);
                    decoder = MediaCodec.createDecoderByType(mime);
                    decoder.configure(format, null, null, 0);
                    decoder.start();
                    return;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        byte[] yuvBytes = new byte[width * height * 12 / 8]; // 12 bit for each YUV420 pixel
        int frameIndex = 0;
        while (true) {
            long start = System.currentTimeMillis();
            int inputIndex = decoder.dequeueInputBuffer(TIMEOUT_US);
            if (inputIndex < 0) {
                Log.e(TAG, "index < 0");
            } else {
                ByteBuffer inputBuffer = decoder.getInputBuffer(inputIndex);
                int sampleSize = extractor.readSampleData(inputBuffer, 0);
                if (sampleSize < 0) {
                    Log.d(TAG, "InputBuffer BUFFER_FLAG_END_OF_STREAM");
                    decoder.queueInputBuffer(inputIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                } else {
                    frameIndex++;
                    decoder.queueInputBuffer(inputIndex, 0, sampleSize, extractor.getSampleTime(), 0);
                    extractor.advance();
                }
            }

            int outputIndex = decoder.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);
            if (outputIndex < 0) {
                Log.e(TAG, "index " + outputIndex + " < 0");
            } else {
                if (startTime == -1) {
                    startTime = System.currentTimeMillis();
                }
                ByteBuffer outputBuffer = decoder.getOutputBuffer(outputIndex);
                outputBuffer.get(yuvBytes);
                draw(yuvBytes);
                decoder.releaseOutputBuffer(outputIndex, false);
            }
            long end = System.currentTimeMillis();
            Log.d(TAG, "XXX frame " + frameIndex + " decode time: " + (end - start));
        }
    }

    private void draw(byte[] yuvBytes) {
        long start = System.currentTimeMillis();
        Mat rgbMat = yuv2rgbMat(yuvBytes);
        long yuv2rgbEnd = System.currentTimeMillis();
        Bitmap bitmap = Bitmap.createBitmap(rgbMat.cols(), rgbMat.rows(), Bitmap.Config.ARGB_8888);
        long createBitmapEnd = System.currentTimeMillis();
        Utils.matToBitmap(rgbMat, bitmap);
        long matToBitmapEnd = System.currentTimeMillis();
        Log.d(TAG, "XXX yuv2rgb time: " + (yuv2rgbEnd - start) + " createBitmap time: " + (createBitmapEnd - yuv2rgbEnd) + " matToBitmap time: " + (matToBitmapEnd - createBitmapEnd));
//        outputView.post(() -> outputView.setImageBitmap(bitmap));
    }

    private Mat yuv2rgbMat(byte[] data) {
        Mat yuvMat = new Mat(height + height / 2, width, CvType.CV_8UC1);
        yuvMat.put(0, 0, data);
        Mat rgbMat = new Mat();
        Imgproc.cvtColor(yuvMat, rgbMat, Imgproc.COLOR_YUV2RGB_NV21, 3);
        return rgbMat;
    }

    public void close() {
        decoder.stop();
        decoder.release();
        extractor.release();
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

    private static List<VideoConfig> parseVideoConfigs() throws JSONException, IOException {
        List<VideoConfig> videoConfigs = new ArrayList<>();
        JSONObject jsonObject = new JSONObject(getStringFromFile(IMPL_CONFIG_PATH));
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

    public void drawOutput(long rgbMatAddr, List<BoundingBox> results) {
        Mat mat = new Mat(rgbMatAddr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        Bitmap outputBitmap = DrawUtil.drawBoxes(bitmap, results, false);
        outputView.post(() -> outputView.setImageBitmap(outputBitmap));
    }

    private native long createSpatioTemporalRoIMixer();

    private native void enqueueImage(long handle, int vid, long matAddr);

    private native void close(long handle);
}
