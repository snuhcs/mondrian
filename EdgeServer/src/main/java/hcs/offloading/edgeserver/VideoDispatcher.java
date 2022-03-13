package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.media.MediaMetadataRetriever;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.webrtc.TextureBufferImpl;
import org.webrtc.VideoFrame;
import org.webrtc.YuvConverter;

import hcs.offloading.edgeserver.config.DispatcherConfig;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.network.webrtc.CustomCapturer;

public class VideoDispatcher extends CustomCapturer {
    private static final String TAG = VideoDispatcher.class.getName();

    private final DispatcherConfig.VideoConfig mConfig;
    private final int FULL_INFERENCE_INTERVAL;
    private final Dispatcher.Callback mCallback;

    private final MediaMetadataRetriever retriever = new MediaMetadataRetriever();

    VideoDispatcher(DispatcherConfig.VideoConfig config, Dispatcher.Callback callback, int fullInferenceInterval) {
        Log.d(TAG, config.PATH + " videoDispatcher created");
        mConfig = config;
        FULL_INFERENCE_INTERVAL = fullInferenceInterval;
        mCallback = callback;

        retriever.setDataSource(mConfig.PATH);
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

                        Frame frame = Frame.createSingleFrame(bitmap, mConfig.PATH, frameIndex);
                        if (frameIndex % FULL_INFERENCE_INTERVAL == 0) {
                            mCallback.enqueueInferenceRequest(InferenceRequest.createFullFrameRequest(frame));
                        } else {
                            mCallback.enqueueFrame(frame);
                        }

                        long frameEndTimeNs = System.nanoTime();
                        long frameTimeMs = (frameEndTimeNs - frameStartTimeNs) / 1000000;
                        long latencyLimitMs = 1000 / fps;
                        Log.d(TAG, "Frame Extraction Time (ms): " + frameTimeMs);
                        Thread.sleep(frameTimeMs > latencyLimitMs ? 0 : latencyLimitMs - frameTimeMs);
                    }
                }
            } catch (InterruptedException e) {
                Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
            }
        });
        captureThread.start();
    }
}
