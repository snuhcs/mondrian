package hcs.offloading.sensorapplication;

import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import org.json.JSONException;
import org.webrtc.EglBase;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoCapturer;
import org.webrtc.VideoTrack;

import hcs.offloading.network.mqtt.DeviceMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.webrtc.WebRTCCallback;
import hcs.offloading.network.webrtc.WebRTCManager;

public class SensorApplication implements WebRTCCallback {
    private static final String TAG = SensorApplication.class.getName();

    private final Config mConfig;

    private final SurfaceViewRenderer mInputView;

    private String mTargetEdgeIP;
    private final VideoCapturer mVideoCapturer;
    private final VideoTrack mVideoTrack;
    private final MediaStream mMediaStream;
    private PeerConnection mPeerConnection;
    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    @RequiresApi(api = Build.VERSION_CODES.P)
    SensorApplication(Config config, Context context, EglBase eglBase, String uri, SurfaceViewRenderer inputView) {
        mConfig = config;

        mInputView = inputView;

        mMqttManager = new DeviceMqttManager(context, uri, Device.SENSOR, scheduleTopicHandler, webrtcTopicHandler);
        mWebRTCManager = new WebRTCManager(context, mMqttManager, eglBase, this);

        mMediaStream = mWebRTCManager.createMediaStream();

        Pair<VideoCapturer, VideoTrack> capturerAndTrack;
        if (mConfig.USE_SAVED_VIDEO) {
            capturerAndTrack = mWebRTCManager.createSavedVideoTrack(eglBase, mConfig.VIDEO_PATH, mConfig.LOG_PATH);
        } else {
            capturerAndTrack = mWebRTCManager.createCameraTrack(eglBase);
        }
        mVideoCapturer = capturerAndTrack.first;
        mVideoTrack = capturerAndTrack.second;
        mVideoTrack.addSink(mInputView);
        mMediaStream.addTrack(mVideoTrack);
    }

    void close() {
        stopSensorApplication();
        mMediaStream.removeTrack(mVideoTrack);
        mVideoTrack.removeSink(mInputView);
        mMqttManager.close();
    }

    void startSensorApplication() {
        synchronized (this) {
            if (mPeerConnection == null) {
                mPeerConnection = mWebRTCManager.createPeerConnection(mTargetEdgeIP);
                mPeerConnection.addStream(mMediaStream);
                mWebRTCManager.offer(mPeerConnection, mTargetEdgeIP);
            }
        }
    }

    void stopSensorApplication() {
        synchronized (this) {
            if (mPeerConnection != null) {
                try {
                    mVideoCapturer.stopCapture();
                } catch (InterruptedException e) {
                    Log.e(TAG, e.getMessage());
                }
                mPeerConnection.removeStream(mMediaStream);
                mPeerConnection.close();
                mPeerConnection = null;
            }
        }
    }

    private final PacketHandler scheduleTopicHandler = packet -> {
        String newTargetEdgeIP = packet.message;
        if (newTargetEdgeIP != null && newTargetEdgeIP.equals(mTargetEdgeIP)) {
            return;
        }
        stopSensorApplication();
        mTargetEdgeIP = newTargetEdgeIP;
        if (mTargetEdgeIP != null) {
            startSensorApplication();
        }
    };

    private final PacketHandler webrtcTopicHandler = packet -> {
        if (mMqttManager.isLocalIP(packet.dstIp)) {
            try {
                if (packet.header.equals(WebRTCHeader.SDP.name())) {
                    mWebRTCManager.handleSdpMessage(mPeerConnection, packet.message);
                } else if (packet.header.equals(WebRTCHeader.ICE.name())) {
                    mWebRTCManager.handleIceMessage(mPeerConnection, packet.message);
                }
            } catch (JSONException e) {
                Log.e(TAG, e.getMessage() + " " + packet.header + " " + packet.message);
            }
        }
    };

    // WebRTCCallback
    @Override
    public void onConnect(String ip) {
        mVideoCapturer.startCapture(mConfig.WIDTH, mConfig.HEIGHT, mConfig.FPS);
    }

    @Override
    public void onDisconnect(String ip) {
        stopSensorApplication();
    }

    @Override
    public void onAddStream(String ip, MediaStream mediaStream) {

    }
}
