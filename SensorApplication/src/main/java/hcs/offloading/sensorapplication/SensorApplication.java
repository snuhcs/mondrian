package hcs.offloading.sensorapplication;

import android.content.Context;
import android.util.Log;

import org.json.JSONException;
import org.webrtc.EglBase;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoTrack;

import hcs.offloading.network.mqtt.DeviceMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.webrtc.WebRTCManager;

public class SensorApplication {
    private static final String TAG = SensorApplication.class.getName();

    private static final int WIDTH = 1920;
    private static final int HEIGHT = 1080;
    private static final int FPS = 5;

    private final SurfaceViewRenderer mInputView;

    private String mTargetEdgeIP;
    private final VideoTrack mCameraTrack;
    private final MediaStream mMediaStream;
    private PeerConnection mPeerConnection;
    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    SensorApplication(Context context, EglBase eglBase, String uri, SurfaceViewRenderer inputView) {
        mInputView = inputView;

        mMqttManager = new DeviceMqttManager(context, uri, Device.SENSOR, scheduleTopicHandler, webrtcTopicHandler);
        mWebRTCManager = new WebRTCManager(context, mMqttManager, eglBase, null);

        mCameraTrack = mWebRTCManager.createCameraTrack(eglBase, WIDTH, HEIGHT, FPS);
        mMediaStream = mWebRTCManager.createMediaStream();
        mCameraTrack.addSink(mInputView);
        mMediaStream.addTrack(mCameraTrack);
    }

    void close() {
        stopSensorApplication();
        mMediaStream.removeTrack(mCameraTrack);
        mCameraTrack.removeSink(mInputView);
        mMqttManager.close();
    }

    void startSensorApplication() {
        synchronized (this) {
            mPeerConnection = mWebRTCManager.createPeerConnection(mTargetEdgeIP);
            mPeerConnection.addStream(mMediaStream);
            mWebRTCManager.offer(mPeerConnection, mTargetEdgeIP);
        }
    }

    void stopSensorApplication() {
        synchronized (this) {
            if (mPeerConnection != null) {
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
}
