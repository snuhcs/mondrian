package hcs.offloading.network.webrtc;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.media.MediaMetadataRetriever;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Build;
import android.support.annotation.RequiresApi;

import org.webrtc.CapturerObserver;
import org.webrtc.SurfaceTextureHelper;
import org.webrtc.TextureBufferImpl;
import org.webrtc.VideoCapturer;
import org.webrtc.VideoFrame;
import org.webrtc.YuvConverter;

public class CustomCapturer implements VideoCapturer {

    private SurfaceTextureHelper surTexture;
    private Context appContext;
    private CapturerObserver capturerObs;
    private Thread captureThread;
    private MediaMetadataRetriever retriever = new MediaMetadataRetriever();

    @Override
    public void initialize(SurfaceTextureHelper surfaceTextureHelper, Context applicationContext, CapturerObserver capturerObserver) {
        surTexture = surfaceTextureHelper;
        appContext = applicationContext;
        capturerObs = capturerObserver;
    }

    public void initializeVideo(String videoFilePath) {
        retriever.setDataSource(videoFilePath);
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void startCapture(int width, int height, int fps) {
        captureThread = new Thread(() -> {
            try {
                long start = System.nanoTime();
                capturerObs.onCapturerStarted(true);

                int[] textures = new int[1];
                GLES20.glGenTextures(1, textures, 0);

                YuvConverter yuvConverter = new YuvConverter();
                TextureBufferImpl buffer = new TextureBufferImpl(width, height, VideoFrame.TextureBuffer.Type.RGB, textures[0], new Matrix(), surTexture.getHandler(), yuvConverter, null);

                int frameIndex = 0;
                int frameCount = Integer.parseInt(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
                while (true) {
                    long startTime = System.currentTimeMillis();
                    Bitmap bitmap = retriever.getFrameAtIndex(frameIndex++%frameCount);

                    surTexture.getHandler().post(() -> {
                        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
                        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
                        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

                        VideoFrame.I420Buffer i420Buf = yuvConverter.convert(buffer);

                        long frameTime = System.nanoTime() - start;
                        VideoFrame videoFrame = new VideoFrame(i420Buf, 0, frameTime);
                        capturerObs.onFrameCaptured(videoFrame);
                    });
                    long endTime = System.currentTimeMillis();
                    long elapsed = endTime - startTime;
                    long latencyLimit = 1000/fps;
                    Thread.sleep(elapsed > latencyLimit? 0 : latencyLimit - elapsed);
                }
            } catch(InterruptedException ex) {
                ex.printStackTrace();
            }
        });
        captureThread.start();
    }

    @Override
    public void stopCapture() {
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
