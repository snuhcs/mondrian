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

    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    private final SurfaceViewRenderer mInputView;
    private final MediaStream mMediaStream;
    private final VideoCapturer mVideoCapturer;
    private final VideoTrack mVideoTrack;

    private String mTargetEdgeIP;
    private PeerConnection mPeerConnection;
    private boolean isClosed = false;

    @RequiresApi(api = Build.VERSION_CODES.P)
    SensorApplication(Config config, Context context, EglBase eglBase, String uri, SurfaceViewRenderer inputView) {
        mConfig = config;

        mMqttManager = new DeviceMqttManager(context, uri, Device.SENSOR, scheduleTopicHandler, webrtcTopicHandler);
        mWebRTCManager = new WebRTCManager(context, mMqttManager, eglBase, this);

        mInputView = inputView;
        mMediaStream = mWebRTCManager.createMediaStream();

        Pair<VideoCapturer, VideoTrack> capturerAndTrack;
        if (mConfig.USE_SAVED_VIDEO) {
            capturerAndTrack = mWebRTCManager.createSavedVideoTrack(eglBase, mConfig.VIDEO_PATH);
        } else {
            capturerAndTrack = mWebRTCManager.createCameraTrack(eglBase);
        }
        mVideoCapturer = capturerAndTrack.first;
        mVideoTrack = capturerAndTrack.second;
        mVideoTrack.addSink(mInputView);
        mMediaStream.addTrack(mVideoTrack);
    }

    void close() {
        Log.d(TAG, "close()");
        isClosed = true;
        stopSensorApplication();
        mMediaStream.removeTrack(mVideoTrack);
        Log.d(TAG, "mMediaStream.removeTrack(mVideoTrack);");
        mVideoTrack.removeSink(mInputView);
        Log.d(TAG, "mVideoTrack.removeSink(mInputView);");
        mMqttManager.close();
        Log.d(TAG, "mMqttManager.close();");
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
        Log.d(TAG, "stopSensorApplication");
        synchronized (this) {
            Log.d(TAG, "synchronized (this) {");
            if (mPeerConnection != null) {
                Log.d(TAG, "if (mPeerConnection != null)");
                try {
                    mVideoCapturer.stopCapture();
                    Log.d(TAG, "mVideoCapturer.stopCapture();");
                } catch (InterruptedException e) {
                    Log.e(TAG, e.getMessage());
                }
                mPeerConnection.removeStream(mMediaStream);
                Log.d(TAG, "mPeerConnection.removeStream(mMediaStream);");
                if (!mPeerConnection.connectionState().equals(PeerConnection.PeerConnectionState.CLOSED)) {
                    mPeerConnection.close();
                    Log.d(TAG, "mPeerConnection.close();");
                }
                mPeerConnection = null;
                Log.d(TAG, "mPeerConnection = null;");
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
        Log.d(TAG, "onDisconnect(String ip)");
        if (!isClosed) {
            stopSensorApplication();
        }
    }

    @Override
    public void onAddStream(String ip, MediaStream mediaStream) {

    }
}
