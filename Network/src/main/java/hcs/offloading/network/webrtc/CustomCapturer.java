package hcs.offloading.network.webrtc;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.media.MediaMetadataRetriever;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.webrtc.CapturerObserver;
import org.webrtc.SurfaceTextureHelper;
import org.webrtc.TextureBufferImpl;
import org.webrtc.VideoCapturer;
import org.webrtc.VideoFrame;
import org.webrtc.YuvConverter;

public class CustomCapturer implements VideoCapturer {
    private static final String TAG = CustomCapturer.class.getName();

    private SurfaceTextureHelper surTexture;
    private CapturerObserver capturerObs;
    private Thread captureThread;
    private MediaMetadataRetriever retriever = new MediaMetadataRetriever();

    @Override
    public void initialize(SurfaceTextureHelper surfaceTextureHelper, Context applicationContext, CapturerObserver capturerObserver) {
        surTexture = surfaceTextureHelper;
        capturerObs = capturerObserver;
    }

    public void initializeVideo(String videoFilePath) {
        retriever.setDataSource(videoFilePath);
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void startCapture(int width, int height, int fps) {
        Log.d(TAG, "startCapture");
        captureThread = new Thread(() -> {
            try {
                long startTimeNs = System.nanoTime();
                capturerObs.onCapturerStarted(true);

                int[] textures = new int[1];
                GLES20.glGenTextures(1, textures, 0);

                YuvConverter yuvConverter = new YuvConverter();
                TextureBufferImpl buffer = new TextureBufferImpl(width, height, VideoFrame.TextureBuffer.Type.RGB, textures[0], new Matrix(), surTexture.getHandler(), yuvConverter, null);

                int frameCount = Integer.parseInt(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
                while (true) {
                    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
                        long frameStartTimeNs = System.nanoTime();
                        Bitmap bitmap = retriever.getFrameAtIndex(frameIndex);

                        surTexture.getHandler().post(() -> {
                            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
                            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
                            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

                            VideoFrame.I420Buffer i420Buf = yuvConverter.convert(buffer);

                            long frameTime = System.nanoTime() - startTimeNs;
                            VideoFrame videoFrame = new VideoFrame(i420Buf, 180, frameTime);
                            capturerObs.onFrameCaptured(videoFrame);
                        });
                        long frameEndTimeNs = System.nanoTime();
                        long frameTimeMs = (frameEndTimeNs - frameStartTimeNs) / 1000000;
                        long latencyLimitMs = frameTimeMs / fps;
                        Thread.sleep(frameTimeMs > latencyLimitMs ? 0 : latencyLimitMs - frameTimeMs);
                    }
                }
            } catch (InterruptedException e) {
                Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
            }
        });
        captureThread.start();
    }

    @Override
    public void stopCapture() {
        Log.d(TAG, "stopCapture");
        captureThread.interrupt();
    }

    @Override
    public void changeCaptureFormat(int width, int height, int fps) {

    }

    @Override
    public void dispose() {

    }

    @Override
    public boolean isScreencast() {
        return false;
    }
}
