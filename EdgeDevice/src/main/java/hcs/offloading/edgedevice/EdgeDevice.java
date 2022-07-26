package hcs.offloading.edgedevice;

import android.content.Context;
import android.util.Log;
import android.util.Pair;

import org.webrtc.EglBase;
import org.webrtc.MediaStream;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoCapturer;
import org.webrtc.VideoTrack;

import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import hcs.offloading.edgedevice.config.Config;
import hcs.offloading.strm.InferenceViewCallback;
import hcs.offloading.strm.SpatioTemporalRoIMixer;

public class EdgeDevice {
    private static final String TAG = EdgeDevice.class.getName();

    private final Config mConfig;

    private final SurfaceViewRenderer mInputView;

    private final Thread mVideoEdgeDeviceThread;

    private final Map<String, VideoSource> mVideoSources = new ConcurrentHashMap<>();
    private final SpatioTemporalRoIMixer mSpatioTemporalRoIMixer;

    EdgeDevice(Config config, Context context, String uri, SurfaceViewRenderer inputView, InferenceViewCallback inferenceViewCallback) {
        mConfig = config;

        EglBase eglBase = EglBase.create();
        mInputView = inputView;
        mInputView.init(eglBase.getEglBaseContext(), null);

        mSpatioTemporalRoIMixer = new SpatioTemporalRoIMixer(inferenceViewCallback);

        mVideoEdgeDeviceThread = new Thread(this::startVideoEdgeDevice);
        mVideoEdgeDeviceThread.start();
    }

    void close() {
        try {
            mVideoEdgeDeviceThread.interrupt();
            mVideoEdgeDeviceThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
        stopEdgeDevice();
        mSpatioTemporalRoIMixer.close();
        Log.d(TAG, "closed");
    }

    private void startEdgeDevice() {
        Log.d(TAG, "startEdgeDevice");
        synchronized (this) {

        }
    }

    private void startVideoEdgeDevice() {
        Log.d(TAG, "startVideoEdgeDevice");
        startEdgeDevice();
        for (Config.VideoConfig videoConfig : mConfig.VIDEO_CONFIGS) {
            VideoSource videoSource = new VideoSource(videoConfig, mSpatioTemporalRoIMixer, mConfig.DRAW_INPUT);
            Log.d(TAG, "VideoSource Added : " + videoConfig.PATH);

            Pair<VideoCapturer, VideoTrack> capturerAndTrack = mWebRTCManager.createSavedVideoTrack(videoConfig.PATH, videoSource);
            MediaStream mediaStream = mWebRTCManager.createMediaStream();
            VideoCapturer videoCapturer = capturerAndTrack.first;
            VideoTrack videoTrack = capturerAndTrack.second;
            videoTrack.addSink(mInputView);
            mediaStream.addTrack(videoTrack);
            videoCapturer.startCapture(videoConfig.WIDTH, videoConfig.HEIGHT, videoConfig.FPS);

            mVideoSources.put(videoConfig.PATH, videoSource);
        }
    }

    private void stopEdgeDevice() {
        Set<String> IPs = mVideoSources.keySet();
        for (String ip : IPs) {
            VideoSource dispatcher = mVideoSources.remove(ip);
            if (dispatcher != null) {
                dispatcher.close();
            }
        }
    }
}
