package hcs.offloading.edgedevice;

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

import java.util.List;
import java.util.Map;

import hcs.offloading.edgedevice.config.DispatcherConfig;
import hcs.offloading.edgedevice.config.RoIExtractorConfig;
import hcs.offloading.edgedevice.datatypes.BoundingBox;
import hcs.offloading.edgedevice.datatypes.Frame;
import hcs.offloading.network.webrtc.CustomCapturer;

public class VideoDispatcher extends CustomCapturer {
    private static final String TAG = VideoDispatcher.class.getName();

    private final String VIDEO_PATH;

    private final MediaMetadataRetriever retriever = new MediaMetadataRetriever();

    private final RoIExtractor mRoIExtractor;

    @RequiresApi(api = Build.VERSION_CODES.P)
    VideoDispatcher(DispatcherConfig.VideoConfig config, RoIExtractorConfig roiConfig, RoIExtractor.Callback roiCallback) {
        mRoIExtractor = new RoIExtractor(roiConfig, roiCallback, config.PATH);

        VIDEO_PATH = config.PATH;

        retriever.setDataSource(VIDEO_PATH);
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
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

//            int frameCount = Integer.parseInt(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
            int frameCount = 600;
            for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
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

                mRoIExtractor.enqueueFrame(Frame.createSingleFrame(bitmap, VIDEO_PATH, frameIndex));
            }
        });
        captureThread.start();
    }

    void enqueueResults(int frameIndex, List<BoundingBox> results) {
        mRoIExtractor.enqueueResults(frameIndex, results);
    }

    void enqueueResults(Map<Integer, List<BoundingBox>> results) {
        mRoIExtractor.enqueueResults(results);
    }

    public void close() {
        try {
            captureThread.interrupt();
            captureThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        mRoIExtractor.close();
        Log.d(TAG, "closed");
    }
}
