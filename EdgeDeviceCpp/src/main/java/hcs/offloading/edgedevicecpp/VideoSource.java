package hcs.offloading.edgedevicecpp;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.media.MediaMetadataRetriever;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;

import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.webrtc.TextureBufferImpl;
import org.webrtc.VideoFrame;
import org.webrtc.YuvConverter;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;

import hcs.offloading.edgedevicecpp.config.SourceConfig;
import hcs.offloading.network.webrtc.CustomCapturer;
import hcs.offloading.strmcpp.SpatioTemporalRoIMixer;
import hcs.offloading.strmcpp.BoundingBox;

public class VideoSource extends CustomCapturer implements Runnable {
    private static final String TAG = VideoSource.class.getName();

    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");
    }

    private final int startIndex = 0;
    private final String VIDEO_PATH;

    private final MediaMetadataRetriever retriever = new MediaMetadataRetriever();
    private final Map<Integer, Bitmap> frames = new ConcurrentHashMap<>();
    private final LinkedBlockingQueue<Integer> frameIndices = new LinkedBlockingQueue<>();

    private final SpatioTemporalRoIMixer strm;

    private final Thread resultThread;
    private final ResultCallback mResultCallback;
    private final boolean DRAW;
    private final float DRAW_CONFIDENCE;
    private final int FPS;

    VideoSource(SourceConfig.VideoConfig config, SpatioTemporalRoIMixer strm, ResultCallback resultCallback, boolean draw, float drawConfidence) {
        FPS = config.FPS;
        VIDEO_PATH = config.PATH;
        DRAW = draw;
        DRAW_CONFIDENCE = drawConfidence;
        mResultCallback = resultCallback;
        this.strm = strm;
        retriever.setDataSource(VIDEO_PATH);

        Log.d(TAG, "Start drawThread");
        resultThread = new Thread(this);
        resultThread.start();
    }

    @Override
    public void run() {
        try {
            while (true) {
                long startTime = System.nanoTime();
                int frameIndex = frameIndices.take();
                List<BoundingBox> results = strm.getResults(VIDEO_PATH, frameIndex);
                mResultCallback.log(VIDEO_PATH, frameIndex, results);
                if (DRAW) {
                    Bitmap bitmap = frames.remove(frameIndex);
                    mResultCallback.drawObjectDetectionResult(DrawUtil.drawBoxes(
                            bitmap, results, DRAW_CONFIDENCE));
                    Thread.sleep((int) (1e9 / FPS - (System.nanoTime() - startTime) / 1e3));
                }
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    @Override
    public void startCapture(int width, int height, int fps) {
        Log.d(TAG, "startCapture");
        captureThread = new Thread(() -> {
            long startTimeNs = System.nanoTime();
            capturerObs.onCapturerStarted(true);

            int[] textures = new int[1];
            GLES20.glGenTextures(1, textures, 0);

            YuvConverter yuvConverter = new YuvConverter();
            TextureBufferImpl buffer = new TextureBufferImpl(width, height, VideoFrame.TextureBuffer.Type.RGB, textures[0], new Matrix(), surTexture.getHandler(), yuvConverter, null);

            int frameCount = Integer.parseInt(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
            for (int frameIndex = startIndex; frameIndex < frameCount; frameIndex++) {
                long startTime = System.nanoTime();
                Log.v(TAG, VIDEO_PATH + " " + frameIndex + " loaded");
                Bitmap bitmap = retriever.getFrameAtIndex(frameIndex);

                if (DRAW) {
                    surTexture.getHandler().post(() -> {
                        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
                        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
                        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

                        VideoFrame.I420Buffer i420Buf = yuvConverter.convert(buffer);

                        long frameTime = System.nanoTime() - startTimeNs;
                        VideoFrame videoFrame = new VideoFrame(i420Buf, 180, frameTime);
                        capturerObs.onFrameCaptured(videoFrame);
                        i420Buf.release();
                    });
                }

                Mat mat = new Mat();
                Utils.bitmapToMat(bitmap, mat);
                int index = strm.enqueueImage(VIDEO_PATH, mat);
                if (DRAW) {
                    frames.put(index, bitmap);
                }
                try {
                    frameIndices.put(index);
                } catch (InterruptedException e) {
                    Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
                }
                long endTime = System.nanoTime();
                long duration = endTime - startTime;
                if (duration < 1e9 / fps) {
                    try {
                        Thread.sleep((int) ((1e9 / fps - duration) / 1e6));
                    } catch (InterruptedException e) {
                        Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
                    }
                }
            }
        });
        captureThread.start();
    }

    public void close() {
        try {
            captureThread.interrupt();
            captureThread.join();
            resultThread.interrupt();
            resultThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        Log.d(TAG, "closed");
    }
}
