package hcs.offloading.edgedevicecpp;

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

import hcs.offloading.edgedevicecpp.config.Config;
import hcs.offloading.edgedevicecpp.config.SourceConfig;
import hcs.offloading.network.mqtt.DeviceMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;
import hcs.offloading.network.webrtc.WebRTCCallback;
import hcs.offloading.network.webrtc.WebRTCManager;
import hcs.offloading.strmcpp.InferenceViewCallback;
import hcs.offloading.strmcpp.SpatioTemporalRoIMixer;

public class EdgeDevice implements WebRTCCallback {
    private static final String TAG = EdgeDevice.class.getName();

    private Config mConfig;

    private SurfaceViewRenderer mInputView;
    private ResultCallback mResultCallback;

    private String mTargetEdgeIP;
    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    private Thread mVideoEdgeDeviceThread;

    private final Map<String, VideoSource> mVideoSources = new ConcurrentHashMap<>();
    private final Map<String, WebRTCSource> mWebRTCSources = new ConcurrentHashMap<>();
    private SpatioTemporalRoIMixer mSpatioTemporalRoIMixer;

    EdgeDevice(Config config, Context context, String uri, SurfaceViewRenderer inputView,
               ResultCallback resultCallback, InferenceViewCallback inferenceViewCallback) {
        mConfig = config;
        mResultCallback = resultCallback;

        EglBase eglBase = EglBase.create();
        mInputView = inputView;
        mInputView.init(eglBase.getEglBaseContext(), null);

        mSpatioTemporalRoIMixer = new SpatioTemporalRoIMixer(mConfig.DRAW ? inferenceViewCallback : null);

        if (!mConfig.sourceConfig.USE_LOCAL_VIDEO) {
            mMqttManager = new DeviceMqttManager(context, uri, Device.EDGE, scheduleTopicHandler, webrtcTopicHandler);
        }
        mWebRTCManager = new WebRTCManager(context, mMqttManager, eglBase, this);

        if (mConfig.sourceConfig.USE_LOCAL_VIDEO) {
            mVideoEdgeDeviceThread = new Thread(this::startVideoEdgeDevice);
            mVideoEdgeDeviceThread.start();
        }
    }

    void close() {
        if (mConfig.sourceConfig.USE_LOCAL_VIDEO) {
            try {
                mVideoEdgeDeviceThread.interrupt();
                mVideoEdgeDeviceThread.join();
            } catch (InterruptedException e) {
                Log.e(TAG, e.getMessage());
            }
            for (VideoSource videoSource : mVideoSources.values()) {
                videoSource.stopCapture();
            }
        }
        stopEdgeDevice();

        mSpatioTemporalRoIMixer.close();
        mMqttManager.close();
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
        for (SourceConfig.VideoConfig videoConfig : mConfig.sourceConfig.VIDEO_CONFIGS) {
            VideoSource videoSource = new VideoSource(
                    videoConfig, mSpatioTemporalRoIMixer, mResultCallback, mConfig.DRAW, mConfig.sourceConfig.DRAW_CONFIDENCE);
            Log.d(TAG, "VideoSource Added : " + videoConfig.PATH);

            Pair<VideoCapturer, VideoTrack> capturerAndTrack =
                    mWebRTCManager.createSavedVideoTrack(videoConfig.PATH, videoSource);
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
        if (!mConfig.sourceConfig.USE_LOCAL_VIDEO) {
            Set<String> IPs = mWebRTCSources.keySet();
            for (String ip : IPs) {
                WebRTCSource webRTCSource = mWebRTCSources.remove(ip);
                if (webRTCSource != null) {
                    webRTCSource.close();
                }
            }
        } else {
            Set<String> IPs = mVideoSources.keySet();
            for (String ip : IPs) {
                VideoSource dispatcher = mVideoSources.remove(ip);
                if (dispatcher != null) {
                    dispatcher.close();
                }
            }
        }
    }

    private final PacketHandler scheduleTopicHandler = packet -> {
        String newTargetEdgeIP = packet.message;
        if (newTargetEdgeIP != null && newTargetEdgeIP.equals(mTargetEdgeIP)) {
            return;
        }
        if (mMqttManager.isLocalIP(newTargetEdgeIP)) {
            startEdgeDevice();
        } else {
            stopEdgeDevice();
        }
        mTargetEdgeIP = newTargetEdgeIP;
    };

    private final PacketHandler webrtcTopicHandler = packet -> {
        if (mMqttManager.isLocalIP(packet.dstIp)) {
            if (packet.header.equals(WebRTCHeader.SDP.name())) {
                mWebRTCSources.put(packet.srcIp, new WebRTCSource(
                        packet.srcIp, mSpatioTemporalRoIMixer, mWebRTCManager, mInputView, mResultCallback, mConfig.sourceConfig.DRAW_CONFIDENCE));
                mWebRTCSources.get(packet.srcIp).handleSdpAndAnswer(packet.message);
            } else if (packet.header.equals(WebRTCHeader.ICE.name())) {
                mWebRTCSources.get(packet.srcIp).handleIceMessage(packet.message);
            }
        }
    };

    // WebRTCCallback
    @Override
    public void onConnect(String ip) {

    }

    @Override
    public void onDisconnect(String ip) {
        WebRTCSource webRTCSource = mWebRTCSources.remove(ip);
        if (webRTCSource != null) {
            webRTCSource.close();
        }
    }

    @Override
    public void onAddStream(String ip, MediaStream mediaStream) {
        WebRTCSource webRTCSource = mWebRTCSources.get(ip);
        if (webRTCSource != null) {
            webRTCSource.onAddStream(mediaStream);
        }
    }
}
