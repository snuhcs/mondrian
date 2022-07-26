package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.graphics.Matrix;
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

import hcs.offloading.edgedevice.config.Config;
import hcs.offloading.network.webrtc.CustomCapturer;
import hcs.offloading.strm.SpatioTemporalRoIMixer;

public class VideoSource extends CustomCapturer {
    private static final String TAG = VideoSource.class.getName();

    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");
    }

    private final String VIDEO_PATH;

    private final MediaMetadataRetriever retriever = new MediaMetadataRetriever();

    private final SpatioTemporalRoIMixer strm;

    private final boolean DRAW;

    VideoSource(Config.VideoConfig config, SpatioTemporalRoIMixer strm, boolean draw) {
        VIDEO_PATH = config.PATH;
        DRAW = draw;
        this.strm = strm;
        retriever.setDataSource(VIDEO_PATH);
    }

    @Override
    public void startCapture(int width, int height, int fps) {
        Log.d(TAG, "startCapture");
        captureThread = new Thread(() -> {
            capturerObs.onCapturerStarted(true);

            int[] textures = new int[1];
            GLES20.glGenTextures(1, textures, 0);

            YuvConverter yuvConverter = new YuvConverter();
            TextureBufferImpl buffer = new TextureBufferImpl(width, height, VideoFrame.TextureBuffer.Type.RGB, textures[0], new Matrix(), surTexture.getHandler(), yuvConverter, null);

            int frameCount = Integer.parseInt(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
            long startTimeNs = System.nanoTime();
            for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
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
                strm.enqueueImage(VIDEO_PATH, mat);
                long endTimeNs = System.nanoTime();
                long nextStartTimeNs = startTimeNs + (long) (frameIndex + 1) * (long) (1e9 / fps);
                if (nextStartTimeNs > endTimeNs) {
                    try {
                        Thread.sleep((nextStartTimeNs - endTimeNs) / 1000000);
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
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        Log.d(TAG, "closed");
    }
}
