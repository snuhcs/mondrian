package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.media.MediaMetadataRetriever;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.webrtc.TextureBufferImpl;
import org.webrtc.VideoFrame;
import org.webrtc.YuvConverter;

import java.util.List;

import hcs.offloading.edgedevice.config.SourceConfig;
import hcs.offloading.network.webrtc.CustomCapturer;
import hcs.offloading.strm.SpatioTemporalRoIMixer;
import hcs.offloading.strm.datatypes.BoundingBox;

public class VideoSource extends CustomCapturer implements Runnable {
    private static final String TAG = VideoSource.class.getName();

    private final int startIndex = 0;
    private final String VIDEO_PATH;

    private final MediaMetadataRetriever retriever = new MediaMetadataRetriever();

    private final SpatioTemporalRoIMixer strm;

    private final Thread drawThread;
    private final ResultCallback mResultCallback;
    private final float DRAW_CONFIDENCE;

    VideoSource(SourceConfig.VideoConfig config, SpatioTemporalRoIMixer strm, ResultCallback resultCallback, float drawConfidence) {
        VIDEO_PATH = config.PATH;
        DRAW_CONFIDENCE = drawConfidence;
        mResultCallback = resultCallback;
        this.strm = strm;
        this.strm.addSource(VIDEO_PATH);
        retriever.setDataSource(VIDEO_PATH);

        Log.d(TAG, "Start drawThread");
        drawThread = new Thread(this);
        drawThread.start();
    }

    @Override
    public void run() {
        int frameIndex = startIndex;
        try {
            while (true) {
                List<BoundingBox> results = strm.getResults(VIDEO_PATH, frameIndex);
                mResultCallback.log(VIDEO_PATH, frameIndex, results);
                frameIndex++;
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
                Log.v(TAG, VIDEO_PATH + " " + frameIndex + " loaded");
                Bitmap bitmap = retriever.getFrameAtIndex(frameIndex);

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

                Mat mat = new Mat();
                Utils.bitmapToMat(bitmap, mat);
                try {
                    strm.enqueueImage(VIDEO_PATH, frameIndex, mat);
                } catch (InterruptedException e) {
                    Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
                }
            }
        });
        captureThread.start();
    }

    public void close() {
        strm.removeSource(VIDEO_PATH);
        try {
            captureThread.interrupt();
            captureThread.join();
            drawThread.interrupt();
            drawThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        Log.d(TAG, "closed");
    }
}
