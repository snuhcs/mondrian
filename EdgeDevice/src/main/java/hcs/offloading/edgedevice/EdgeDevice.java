package hcs.offloading.edgedevice;

import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import org.json.JSONException;
import org.webrtc.EglBase;
import org.webrtc.MediaStream;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoCapturer;
import org.webrtc.VideoTrack;

import java.io.IOException;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import hcs.offloading.edgedevice.config.Config;
import hcs.offloading.edgedevice.config.SourceConfig;
import hcs.offloading.edgedevice.inferenceengine.TFLiteInferenceEngine;
import hcs.offloading.edgedevice.resizeprofile.CustomResizeProfile;
import hcs.offloading.edgedevice.roiprioritizer.CustomRoIPrioritizer;
import hcs.offloading.network.mqtt.DeviceMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;
import hcs.offloading.network.webrtc.WebRTCCallback;
import hcs.offloading.network.webrtc.WebRTCManager;
import hcs.offloading.strm.SpatioTemporalRoIMixer;
import hcs.offloading.strm.config.STRMConfig;

@RequiresApi(api = Build.VERSION_CODES.P)
public class EdgeDevice implements WebRTCCallback {
    private static final String TAG = EdgeDevice.class.getName();

    private static final String STRM_CONFIG_PATH = "/data/local/tmp/strm.json";

    private final Config mConfig;

    private SurfaceViewRenderer mInputView;

    private String mTargetEdgeIP;
    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    private Thread mVideoEdgeDeviceThread;

    private final Map<String, VideoSource> mVideoDispatchers = new ConcurrentHashMap<>();
    private final Map<String, WebRTCSource> mDispatchers = new ConcurrentHashMap<>();
    private SpatioTemporalRoIMixer mSpatioTemporalRoIMixer;
    private TFLiteInferenceEngine mTFLiteInferenceEngine;

    EdgeDevice(Config config, Context context, String uri, SurfaceViewRenderer inputView) {
        mConfig = config;

        EglBase eglBase = EglBase.create();
        mInputView = inputView;
        mInputView.init(eglBase.getEglBaseContext(), null);

        mTFLiteInferenceEngine = new TFLiteInferenceEngine(mConfig.inferenceEngineConfig, context.getAssets());

        try {
            mSpatioTemporalRoIMixer = new SpatioTemporalRoIMixer(
                    new STRMConfig(STRM_CONFIG_PATH),
                    new CustomResizeProfile(config.resizeProfileConfig),
                    new CustomRoIPrioritizer(),
                    mTFLiteInferenceEngine);
        } catch (IOException | JSONException e) {
            Log.e(TAG, e.getMessage());
        }

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
            for (VideoSource videoSource : mVideoDispatchers.values()) {
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
            VideoSource videoSource = new VideoSource(videoConfig, mSpatioTemporalRoIMixer);

            Pair<VideoCapturer, VideoTrack> capturerAndTrack = mWebRTCManager.createSavedVideoTrack(videoConfig.PATH, videoSource);
            MediaStream mediaStream = mWebRTCManager.createMediaStream();
            VideoCapturer videoCapturer = capturerAndTrack.first;
            VideoTrack videoTrack = capturerAndTrack.second;
            videoTrack.addSink(mInputView);
            mediaStream.addTrack(videoTrack);
            videoCapturer.startCapture(videoConfig.WIDTH, videoConfig.HEIGHT, videoConfig.FPS);

            mVideoDispatchers.put(videoConfig.PATH, videoSource);
        }
    }

    private void stopEdgeDevice() {
        if (!mConfig.sourceConfig.USE_LOCAL_VIDEO) {
            Set<String> IPs = mDispatchers.keySet();
            for (String ip : IPs) {
                WebRTCSource webRTCSource = mDispatchers.remove(ip);
                if (webRTCSource != null) {
                    webRTCSource.close();
                }
            }
        } else {
            Set<String> IPs = mVideoDispatchers.keySet();
            for (String ip : IPs) {
                VideoSource dispatcher = mVideoDispatchers.remove(ip);
                if (dispatcher != null) {
                    dispatcher.close();
                }
            }
        }
        synchronized (this) {
            if (mTFLiteInferenceEngine != null) {
                mTFLiteInferenceEngine.close();
                mTFLiteInferenceEngine = null;
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
                mDispatchers.put(packet.srcIp, new WebRTCSource(
                        packet.srcIp, mSpatioTemporalRoIMixer, mWebRTCManager, mInputView));
                mDispatchers.get(packet.srcIp).handleSdpAndAnswer(packet.message);
            } else if (packet.header.equals(WebRTCHeader.ICE.name())) {
                mDispatchers.get(packet.srcIp).handleIceMessage(packet.message);
            }
        }
    };

    // WebRTCCallback
    @Override
    public void onConnect(String ip) {

    }

    @Override
    public void onDisconnect(String ip) {
        WebRTCSource webRTCSource = mDispatchers.remove(ip);
        if (webRTCSource != null) {
            webRTCSource.close();
        }
    }

    @Override
    public void onAddStream(String ip, MediaStream mediaStream) {
        WebRTCSource webRTCSource = mDispatchers.get(ip);
        if (webRTCSource != null) {
            webRTCSource.onAddStream(mediaStream);
        }
    }
}
