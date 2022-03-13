package hcs.offloading.network.webrtc;

import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import org.json.JSONException;
import org.json.JSONObject;
import org.webrtc.Camera2Enumerator;
import org.webrtc.CameraEnumerator;
import org.webrtc.DefaultVideoDecoderFactory;
import org.webrtc.DefaultVideoEncoderFactory;
import org.webrtc.EglBase;
import org.webrtc.IceCandidate;
import org.webrtc.MediaConstraints;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.PeerConnectionFactory;
import org.webrtc.SessionDescription;
import org.webrtc.SurfaceTextureHelper;
import org.webrtc.VideoCapturer;
import org.webrtc.VideoSource;
import org.webrtc.VideoTrack;

import java.util.ArrayList;
import java.util.List;

import hcs.offloading.network.mqtt.DeviceMqttManager;

public class WebRTCManager {
    private static final String TAG = WebRTCManager.class.getName();

    private final Context mContext;
    private final WebRTCCallback mWebRTCCallback;

    private final DeviceMqttManager mMqttManager;
    private final PeerConnectionFactory mPeerConnectionFactory;

    public WebRTCManager(Context context, DeviceMqttManager mqttManager, EglBase eglBase, WebRTCCallback webRTCCallback) {
        mContext = context;
        mMqttManager = mqttManager;
        mWebRTCCallback = webRTCCallback;

        DefaultVideoEncoderFactory defaultVideoEncoderFactory = new DefaultVideoEncoderFactory(
                eglBase.getEglBaseContext(), true, true
        );
        DefaultVideoDecoderFactory defaultVideoDecoderFactory = new DefaultVideoDecoderFactory(eglBase.getEglBaseContext());
        PeerConnectionFactory.initialize(PeerConnectionFactory.InitializationOptions.builder(context).createInitializationOptions());
        PeerConnectionFactory.Options options = new PeerConnectionFactory.Options();
        mPeerConnectionFactory = PeerConnectionFactory.builder()
                .setOptions(options)
                .setVideoEncoderFactory(defaultVideoEncoderFactory)
                .setVideoDecoderFactory(defaultVideoDecoderFactory)
                .createPeerConnectionFactory();
    }

    public void offer(PeerConnection peerConnection, String ip) {
        MediaConstraints sdpConstraints = new MediaConstraints();
        sdpConstraints.mandatory.add(
                new MediaConstraints.KeyValuePair("OfferToReceiveVideo", "true")
        );
        peerConnection.createOffer(new CustomSdpObserver() {
            @Override
            public void onCreateSuccess(SessionDescription sessionDescription) {
                super.onCreateSuccess(sessionDescription);
                Log.d(TAG, "onCreateSuccess " + sessionDescription.description);
                peerConnection.setLocalDescription(new CustomSdpObserver(), sessionDescription);
                try {
                    JSONObject jsonSessionDescription = new JSONObject();
                    jsonSessionDescription.put("type", sessionDescription.type.canonicalForm());
                    jsonSessionDescription.put("sdp", sessionDescription.description);
                    mMqttManager.sendSdpMessage(ip, jsonSessionDescription.toString());
                } catch (Exception e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }, sdpConstraints);
    }

    public void answer(PeerConnection peerConnection, String ip) {
        Log.d(TAG, "answer");
        assert peerConnection != null;
        peerConnection.createAnswer(new CustomSdpObserver() {
            @Override
            public void onCreateSuccess(SessionDescription sessionDescription) {
                super.onCreateSuccess(sessionDescription);
                Log.d(TAG, "onCreateSuccess" + sessionDescription.description);
                peerConnection.setLocalDescription(new CustomSdpObserver(), sessionDescription);
                try {
                    JSONObject jsonSessionDescription = new JSONObject();
                    jsonSessionDescription.put("type", sessionDescription.type.canonicalForm());
                    jsonSessionDescription.put("sdp", sessionDescription.description);
                    mMqttManager.sendSdpMessage(ip, jsonSessionDescription.toString());
                } catch (Exception e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }, new MediaConstraints());
    }

    public PeerConnection createPeerConnection(String ip) {
        Log.d(TAG, "createPeerConnection");
        List<PeerConnection.IceServer> iceServers = new ArrayList<>();
        PeerConnection.RTCConfiguration rtcConfig = new PeerConnection.RTCConfiguration(iceServers);
        return mPeerConnectionFactory.createPeerConnection(rtcConfig, new CustomPeerConnectionObserver() {
            @Override
            public void onIceCandidate(IceCandidate iceCandidate) {
                super.onIceCandidate(iceCandidate);
                Log.d(TAG, "onIceCandidate");
                try {
                    JSONObject jsonIceCandidate = new JSONObject();
                    jsonIceCandidate.put("type", "candidate");
                    jsonIceCandidate.put("label", iceCandidate.sdpMLineIndex);
                    jsonIceCandidate.put("id", iceCandidate.sdpMid);
                    jsonIceCandidate.put("candidate", iceCandidate.sdp);
                    mMqttManager.sendIceMessage(ip, jsonIceCandidate.toString());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onAddStream(MediaStream mediaStream) {
                super.onAddStream(mediaStream);
                Log.d(TAG, "onAddStream " + ip);
                if (mediaStream.videoTracks != null && mediaStream.videoTracks.size() == 1) {
                    mWebRTCCallback.onAddStream(ip, mediaStream);
                }
            }

            @Override
            public void onConnectionChange(PeerConnection.PeerConnectionState state) {
                super.onConnectionChange(state);
                Log.d(TAG, "onConnectionChange: " + state.name());
                if (state == PeerConnection.PeerConnectionState.CONNECTED) {
                    mWebRTCCallback.onConnect(ip);
                } else if (state == PeerConnection.PeerConnectionState.DISCONNECTED ||
                        state == PeerConnection.PeerConnectionState.FAILED ||
                        state == PeerConnection.PeerConnectionState.CLOSED) {
                    mWebRTCCallback.onDisconnect(ip);
                }
            }
        });
    }

    public void handleIceMessage(PeerConnection peerConnection, String message) throws JSONException {
        Log.d(TAG, "handleIceMessage");
        assert peerConnection != null;
        JSONObject json = new JSONObject(message);
        String id = json.getString("id");
        int label = json.getInt("label");
        String candidate = json.getString("candidate");
        IceCandidate iceCandidate = new IceCandidate(id, label, candidate);
        peerConnection.addIceCandidate(iceCandidate);
    }


    public void handleSdpMessage(PeerConnection peerConnection, String message) throws JSONException {
        Log.d(TAG, "handleSdpMessage");
        assert peerConnection != null;
        JSONObject json = new JSONObject(message);
        SessionDescription.Type type = SessionDescription.Type.fromCanonicalForm(json.getString("type").toLowerCase());
        String description = json.getString("sdp");
        peerConnection.setRemoteDescription(new CustomSdpObserver(), new SessionDescription(type, description));
    }

    public MediaStream createMediaStream() {
        return mPeerConnectionFactory.createLocalMediaStream("102");
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    public Pair<VideoCapturer, VideoTrack> createSavedVideoTrack(EglBase eglBase, String videoFilePath) {
        SurfaceTextureHelper surfaceTextureHelper = SurfaceTextureHelper.create("CaptureThread", eglBase.getEglBaseContext());
        CustomCapturer videoCapturer = new CustomCapturer();
        VideoSource videoSource = mPeerConnectionFactory.createVideoSource(videoCapturer.isScreencast());
        videoCapturer.initialize(surfaceTextureHelper, mContext, videoSource.getCapturerObserver());
        videoCapturer.initializeVideo(videoFilePath);
        return new Pair<>(videoCapturer, mPeerConnectionFactory.createVideoTrack("101", videoSource));
    }

    public Pair<VideoCapturer, VideoTrack> createCameraTrack(EglBase eglBase) {
        Log.d(TAG, "createVideoTrack");
        VideoCapturer videoCapturer = createCameraCapturer(new Camera2Enumerator(mContext));
        SurfaceTextureHelper surfaceTextureHelper = SurfaceTextureHelper.create("CaptureThread", eglBase.getEglBaseContext());
        assert videoCapturer != null;
        VideoSource videoSource = mPeerConnectionFactory.createVideoSource(videoCapturer.isScreencast());
        videoCapturer.initialize(surfaceTextureHelper, mContext, videoSource.getCapturerObserver());
        return new Pair<>(videoCapturer, mPeerConnectionFactory.createVideoTrack("100", videoSource));
    }

    private VideoCapturer createCameraCapturer(CameraEnumerator enumerator) {
        final String[] deviceNames = enumerator.getDeviceNames();
        // First, try to find front facing camera
        for (String deviceName : deviceNames) {
            if (enumerator.isFrontFacing(deviceName)) {
                VideoCapturer videoCapturer = enumerator.createCapturer(deviceName, null);
                if (videoCapturer != null) {
                    return videoCapturer;
                }
            }
        }
        // Front facing camera not found, try something else
        for (String deviceName : deviceNames) {
            if (!enumerator.isFrontFacing(deviceName)) {
                VideoCapturer videoCapturer = enumerator.createCapturer(deviceName, null);
                if (videoCapturer != null) {
                    return videoCapturer;
                }
            }
        }
        return null;
    }
}
