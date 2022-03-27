package hcs.offloading.edgedevice;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Build;
import android.support.annotation.RequiresApi;
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

import hcs.offloading.edgedevice.inferenceengine.TFLiteInferenceEngine;
import hcs.offloading.strm.config.Config;
import hcs.offloading.network.mqtt.DeviceMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;
import hcs.offloading.network.webrtc.WebRTCCallback;
import hcs.offloading.network.webrtc.WebRTCManager;

@RequiresApi(api = Build.VERSION_CODES.P)
public class EdgeDevice implements WebRTCCallback {
    private static final String TAG = EdgeDevice.class.getName();

    private Config mConfig;

    private SurfaceViewRenderer mInputView;

    private final AssetManager mAssetManager;

    private String mTargetEdgeIP;
    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    private Thread mVideoEdgeDeviceThread;

    private final Map<String, VideoDispatcher> mVideoDispatchers = new ConcurrentHashMap<>();
    private final Map<String, Dispatcher> mDispatchers = new ConcurrentHashMap<>();
    private TFLiteInferenceEngine mTFLiteInferenceEngine;

    EdgeDevice(Config config, Context context, String uri, SurfaceViewRenderer inputView, ViewCallback viewCallback) {
        mConfig = config;
        mAssetManager = context.getAssets();

        EglBase eglBase = EglBase.create();
        mInputView = inputView;
        mInputView.init(eglBase.getEglBaseContext(), null);

        mTFLiteInferenceEngine = new TFLiteInferenceEngine(mConfig.inferenceEngineConfig, 800, mAssetManager);

        if (!mConfig.dispatcherConfig.USE_LOCAL_VIDEO) {
            mMqttManager = new DeviceMqttManager(context, uri, Device.EDGE, scheduleTopicHandler, webrtcTopicHandler);
        }
        mWebRTCManager = new WebRTCManager(context, mMqttManager, eglBase, this);

        if (mConfig.dispatcherConfig.USE_LOCAL_VIDEO) {
            mVideoEdgeDeviceThread = new Thread(this::startVideoEdgeDevice);
            mVideoEdgeDeviceThread.start();
        }
    }

    void close() {
        if (mConfig.dispatcherConfig.USE_LOCAL_VIDEO) {
            try {
                mVideoEdgeDeviceThread.interrupt();
                mVideoEdgeDeviceThread.join();
            } catch (InterruptedException e) {
                Log.e(TAG, e.getMessage());
            }
            for (VideoDispatcher videoDispatcher : mVideoDispatchers.values()) {
                videoDispatcher.stopCapture();
            }
        }
        stopEdgeDevice();

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
        for (DispatcherConfig.VideoConfig videoConfig : mConfig.dispatcherConfig.VIDEO_CONFIGS) {
            VideoDispatcher videoDispatcher = new VideoDispatcher(videoConfig, mConfig.roIExtractorConfig);

            Pair<VideoCapturer, VideoTrack> capturerAndTrack = mWebRTCManager.createSavedVideoTrack(videoConfig.PATH, videoDispatcher);
            MediaStream mediaStream = mWebRTCManager.createMediaStream();
            VideoCapturer videoCapturer = capturerAndTrack.first;
            VideoTrack videoTrack = capturerAndTrack.second;
            videoTrack.addSink(mInputView);
            mediaStream.addTrack(videoTrack);
            videoCapturer.startCapture(videoConfig.WIDTH, videoConfig.HEIGHT, videoConfig.FPS);

            mVideoDispatchers.put(videoConfig.PATH, videoDispatcher);
        }
    }

    private void stopEdgeDevice() {
        if (!mConfig.dispatcherConfig.USE_LOCAL_VIDEO) {
            Set<String> IPs = mDispatchers.keySet();
            for (String ip : IPs) {
                Dispatcher dispatcher = mDispatchers.remove(ip);
                if (dispatcher != null) {
                    dispatcher.close();
                }
            }
        } else {
            Set<String> IPs = mVideoDispatchers.keySet();
            for (String ip : IPs) {
                VideoDispatcher dispatcher = mVideoDispatchers.remove(ip);
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
                mDispatchers.put(packet.srcIp, new Dispatcher(
                        packet.srcIp, mWebRTCManager, mInputView,
                        mConfig.roIExtractorConfig, 800));
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
        Dispatcher dispatcher = mDispatchers.remove(ip);
        if (dispatcher != null) {
            dispatcher.close();
        }
    }

    @Override
    public void onAddStream(String ip, MediaStream mediaStream) {
        Dispatcher dispatcher = mDispatchers.get(ip);
        if (dispatcher != null) {
            dispatcher.onAddStream(mediaStream);
        }
    }
}
