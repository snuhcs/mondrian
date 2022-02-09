package hcs.offloading.sensorapplication;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Switch;

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
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import hcs.offloading.network.Header;
import hcs.offloading.network.Message;
import hcs.offloading.network.NetworkManager;
import hcs.offloading.network.Packet;
import hcs.offloading.network.PacketHandler;
import hcs.offloading.network.Topic;
import hcs.offloading.sensorapplication.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getName();
    private static final int[] COLORS = {
            Color.BLUE,
            Color.RED,
            Color.GREEN,
            Color.YELLOW,
            Color.CYAN,
            Color.MAGENTA,
            Color.WHITE,
            Color.parseColor("#55FF55"),
            Color.parseColor("#FFA500"),
            Color.parseColor("#FF8888"),
            Color.parseColor("#AAAAFF"),
            Color.parseColor("#FFFFAA"),
            Color.parseColor("#55AAAA"),
            Color.parseColor("#AA33AA"),
            Color.parseColor("#0D0068")
    };
    // Bounding Box
    private final Lock mBoundingBoxLock = new ReentrantLock();
    // UI Elements
    private ActivityMainBinding mBinding;
    private EditText mIpInput;
    private EditText mPortInput;
    private Switch mConnectButton;
    private RadioGroup mTaskGroup;
    private ImageView mImageView;
    private int[] mClasses;
    private float[][] mBoundingBoxes;
    // NetworkManager
    private String mLocalIP;
    private final PacketHandler resultTopicHandler = packet -> {
        if (packet.dstIp.equals(mLocalIP)) {
            if (packet.header.equals(Header.RESULT)) {
                String[] bbs = packet.message.split(",");
                if (bbs.length % 5 == 0) {
                    int numBoxes = bbs.length / 5;
                    mBoundingBoxLock.lock();
                    mBoundingBoxes = new float[numBoxes][4];
                    mClasses = new int[numBoxes];
                    for (int i = 0; i < numBoxes; i++) {
                        mBoundingBoxes[i][0] = Float.parseFloat(bbs[i * 5 + 0]);
                        mBoundingBoxes[i][1] = Float.parseFloat(bbs[i * 5 + 1]);
                        mBoundingBoxes[i][2] = Float.parseFloat(bbs[i * 5 + 2]);
                        mBoundingBoxes[i][3] = Float.parseFloat(bbs[i * 5 + 3]);
                        mClasses[i] = Integer.parseInt(bbs[i * 5 + 4]);
                    }
                    mBoundingBoxLock.unlock();
                } else {
                    Log.e(TAG, "Wrong number of bounding boxes : " + bbs.length);
                }
            }
        }
    };
    private String mTargetEdgeIP;
    private NetworkManager mNetworkManager;
    // WebRTC
    private PeerConnection mLocalPeer;
    private final PacketHandler webrtcTopicHandler = packet -> {
        if (packet.dstIp.equals(mLocalIP)) {
            if (packet.header.equals(Header.ICE)) {
                Log.d(TAG, "Ice candidate received from: " + packet.srcIp);
                handleIceCandidate(packet.message);
            } else if (packet.header.equals(Header.SDP)) {
                Log.d(TAG, "Answer received from: " + packet.srcIp);
                handleAnswer(packet.message);
            }
        }
    };
    private PeerConnectionFactory mPeerConnectionFactory;
    private VideoTrack mLocalVideoTrack;
    private final PacketHandler scheduleTopicHandler = packet -> {
        if (packet.dstIp.equals(Packet.ALL)) {
            if (packet.header.equals(Header.SCHEDULE)) {
                sendRegisterMessage();
                if (mTargetEdgeIP == null || !mTargetEdgeIP.equals(packet.message)) {
                    mTargetEdgeIP = packet.message;
                    sendTaskMessage(getTaskfromID(mTaskGroup.getCheckedRadioButtonId()));
                    destroyConnection();
                    createPeerConnection();
                    offer();
                }
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // UI Elements
        mBinding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        mIpInput = findViewById(R.id.ip_input);
        mPortInput = findViewById(R.id.port_input);
        mConnectButton = findViewById(R.id.connect_button);
        mTaskGroup = findViewById(R.id.task_group);
        mImageView = findViewById(R.id.image_view);
        disableTaskGroup();

        // Camera Permission
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, 1);
        }

        initializeWebRTC(getApplicationContext());

        mNetworkManager = new NetworkManager(new HashMap<String, PacketHandler>() {{
            put(Topic.SCHEDULE, scheduleTopicHandler);
            put(Topic.WEBRTC, webrtcTopicHandler);
            put(Topic.RESULT, resultTopicHandler);
        }});

        mConnectButton.setOnCheckedChangeListener((compoundButton, b) -> {
            if (b) {
                startConnection();
            } else {
                stopConnection();
            }
        });

        mTaskGroup.setOnCheckedChangeListener((radioGroup, checkedId) -> {
            sendTaskMessage(getTaskfromID(checkedId));
        });
    }

    private String getTaskfromID(int checkedId) {
        if (checkedId == R.id.no_inference) {
            return Message.TASK_NO_INFERENCE;
        } else if (checkedId == R.id.object_detection) {
            return Message.TASK_OBJECT_DETECTION;
        } else if (checkedId == R.id.face_detection) {
            return Message.TASK_FACE_DETECTION;
        } else {
            Log.e(TAG, "Wrong checkedId : " + checkedId);
        }
        return Message.TASK_NO_INFERENCE;
    }

    private void startConnection() {
        // Start NetworkManager
        String brokerIP = mIpInput.getText().toString().trim();
        int brokerPort = Integer.parseInt(mPortInput.getText().toString().trim());
        String uri = "tcp://" + brokerIP + ":" + brokerPort;
        mLocalIP = getLocalIPAddress();
        mNetworkManager.start(getApplicationContext(), mLocalIP, uri);
        mNetworkManager.connect(() -> {
            mNetworkManager.subscribe(Topic.SCHEDULE, null);
            mNetworkManager.subscribe(Topic.WEBRTC, null);
            mNetworkManager.subscribe(Topic.RESULT, null);
        });

        // Register Sensor
        sendRegisterMessage();

        // Enable Task Group Button
        enableTaskGroup();
    }

    private void stopConnection() {
        // Destroy WebRTC connection
        destroyConnection();
        mTargetEdgeIP = null;

        // Disable Task Group Button
        disableTaskGroup();

        // Unregister Sensor
        sendUnregisterMessage();

        // Stop NetworkManager
        mNetworkManager.stop();
    }

    private void enableTaskGroup() {
        for (int i = 0; i < mTaskGroup.getChildCount(); i++) {
            mTaskGroup.getChildAt(i).setEnabled(true);
        }
        RadioButton noInferenceButton = findViewById(R.id.no_inference);
        noInferenceButton.setChecked(true);
    }

    private void disableTaskGroup() {
        RadioButton noInferenceButton = findViewById(R.id.no_inference);
        noInferenceButton.setChecked(true);
        for (int i = 0; i < mTaskGroup.getChildCount(); i++) {
            mTaskGroup.getChildAt(i).setEnabled(false);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopConnection();
    }

    private void sendRegisterMessage() {
        mNetworkManager.publish(Topic.REGISTER,
                Packet.ALL, Header.REGISTER, Message.DEVICE_SENSOR, null);
    }

    private void sendUnregisterMessage() {
        mNetworkManager.publish(Topic.UNREGISTER,
                Packet.ALL, Header.UNREGISTER, Message.DEVICE_SENSOR, null);
    }

    private void sendTaskMessage(String task) {
        mNetworkManager.publish(Topic.TASK, mTargetEdgeIP, Header.TASK, task, null);
    }

    private void initializeWebRTC(Context context) {
        Log.d(TAG, "initializeWebRTC()");

        PeerConnectionFactory.initialize(PeerConnectionFactory.InitializationOptions.builder(context).createInitializationOptions());
        EglBase rootEglBase = EglBase.create();

        //Create a new PeerConnectionFactory instance.
        PeerConnectionFactory.Options options = new PeerConnectionFactory.Options();
        DefaultVideoEncoderFactory defaultVideoEncoderFactory = new DefaultVideoEncoderFactory(
                rootEglBase.getEglBaseContext(), true, true
        );
        DefaultVideoDecoderFactory defaultVideoDecoderFactory = new DefaultVideoDecoderFactory(rootEglBase.getEglBaseContext());
        mPeerConnectionFactory = PeerConnectionFactory.builder()
                .setOptions(options)
                .setVideoEncoderFactory(defaultVideoEncoderFactory)
                .setVideoDecoderFactory(defaultVideoDecoderFactory)
                .createPeerConnectionFactory();

        // Now create a VideoCapturer instance. Callback methods are there if you want to do something.
        VideoCapturer videoCapturerAndroid = createCameraCapturer(new Camera2Enumerator(context));

        // Create a VideoSource instance
        SurfaceTextureHelper surfaceTextureHelper = SurfaceTextureHelper.create("CaptureThread", rootEglBase.getEglBaseContext());
        assert videoCapturerAndroid != null;
        VideoSource videoSource = mPeerConnectionFactory.createVideoSource(videoCapturerAndroid.isScreencast());
        videoCapturerAndroid.initialize(surfaceTextureHelper, context, videoSource.getCapturerObserver());
        mLocalVideoTrack = mPeerConnectionFactory.createVideoTrack("100", videoSource);

        // we will start capturing the video from the camera
        // width,height and fps
        videoCapturerAndroid.startCapture(1920, 1080, 5);

        // Show frames as preview
        mLocalVideoTrack.addSink(frame -> {
            // Convert videoFrame to Bitmap
            YuvFrame yuvFrame = new YuvFrame(frame);
            Bitmap bitmap = yuvFrame.getBitmap();

            // Show preview
            mBoundingBoxLock.lock();
            if (mBoundingBoxes != null) {
                Canvas canvas = new Canvas(bitmap);
                for (int i = 0; i < mBoundingBoxes.length; i++) {
                    int label = mClasses[i];
                    Paint paint = new Paint();
                    paint.setColor(COLORS[label % COLORS.length]);
                    paint.setStyle(Paint.Style.STROKE);
                    paint.setStrokeWidth(10);
                    canvas.drawRect(
                            mBoundingBoxes[i][0] * canvas.getWidth(),
                            mBoundingBoxes[i][1] * canvas.getHeight(),
                            mBoundingBoxes[i][2] * canvas.getWidth(),
                            mBoundingBoxes[i][3] * canvas.getHeight(),
                            paint);
                }
            }
            mBoundingBoxLock.unlock();
            mImageView.post(() -> mImageView.setImageBitmap(bitmap));
        });
        Log.d(TAG, "initialization done");
    }

    private VideoCapturer createCameraCapturer(CameraEnumerator enumerator) {
        final String[] deviceNames = enumerator.getDeviceNames();
        // First, try to find front facing camera
        Log.d(TAG, "Looking for front facing cameras.");
        for (String deviceName : deviceNames) {
            if (enumerator.isFrontFacing(deviceName)) {
                Log.d(TAG, "Creating front facing camera capturer.");
                VideoCapturer videoCapturer = enumerator.createCapturer(deviceName, null);
                if (videoCapturer != null) {
                    return videoCapturer;
                }
            }
        }
        // Front facing camera not found, try something else
        Log.d(TAG, "Looking for other cameras.");
        for (String deviceName : deviceNames) {
            if (!enumerator.isFrontFacing(deviceName)) {
                Log.d(TAG, "Creating other camera capturer.");
                VideoCapturer videoCapturer = enumerator.createCapturer(deviceName, null);
                if (videoCapturer != null) {
                    return videoCapturer;
                }
            }
        }
        return null;
    }

    private void createPeerConnection() {
        Log.d(TAG, "createPeerConnection()");
        List<PeerConnection.IceServer> iceServers = new ArrayList<>();
        PeerConnection.RTCConfiguration rtcConfig = new PeerConnection.RTCConfiguration(iceServers);
        mLocalPeer = mPeerConnectionFactory.createPeerConnection(rtcConfig, new CustomPeerConnectionObserver() {
            @Override
            public void onIceCandidate(IceCandidate iceCandidate) {
                Log.d(TAG, "onIceCandidate");
                super.onIceCandidate(iceCandidate);
                try {
                    JSONObject JSONIceCandidate = new JSONObject();
                    JSONIceCandidate.put("type", "candidate");
                    JSONIceCandidate.put("label", iceCandidate.sdpMLineIndex);
                    JSONIceCandidate.put("id", iceCandidate.sdpMid);
                    JSONIceCandidate.put("candidate", iceCandidate.sdp);
                    RadioButton selectedButton = findViewById(mTaskGroup.getCheckedRadioButtonId());
                    JSONIceCandidate.put("task_type", selectedButton.getText());
                    String message = JSONIceCandidate.toString();
                    mNetworkManager.publish(Topic.WEBRTC, mTargetEdgeIP, Header.ICE, message, null);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
        addStreamToLocalPeer();
    }

    private void addStreamToLocalPeer() {
        MediaStream mediaStream = mPeerConnectionFactory.createLocalMediaStream("102");
        mediaStream.addTrack(mLocalVideoTrack);
        mLocalPeer.addStream(mediaStream);
    }

    private void offer() {
        Log.d(TAG, "offer()");
        MediaConstraints sdpConstraints = new MediaConstraints();
        sdpConstraints.mandatory.add(
                new MediaConstraints.KeyValuePair("OfferToReceiveVideo", "true")
        );
        mLocalPeer.createOffer(new CustomSdpObserver() {
            @Override
            public void onCreateSuccess(SessionDescription sessionDescription) {
                Log.d(TAG, "onCreateSuccess" + sessionDescription.description);
                super.onCreateSuccess(sessionDescription);
                mLocalPeer.setLocalDescription(new CustomSdpObserver(), sessionDescription);
                try {
                    JSONObject JSONSdp = new JSONObject();
                    JSONSdp.put("type", sessionDescription.type.canonicalForm());
                    JSONSdp.put("sdp", sessionDescription.description);
                    String message = JSONSdp.toString();
                    mNetworkManager.publish(Topic.WEBRTC, mTargetEdgeIP, Header.SDP, message, null);
                } catch (Exception e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }, sdpConstraints);
    }

    private void destroyConnection() {
        if (mLocalPeer != null) {
            mLocalPeer.close();
            mLocalPeer = null;
        }
    }

    private void handleIceCandidate(String iceMessage) {
        try {
            JSONObject JSONIceCandidate = new JSONObject(iceMessage);
            String id = JSONIceCandidate.getString("id");
            int label = JSONIceCandidate.getInt("label");
            String candidate = JSONIceCandidate.getString("candidate");
            IceCandidate iceCandidate = new IceCandidate(id, label, candidate);
            mLocalPeer.addIceCandidate(iceCandidate);
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private void handleAnswer(String sdpMessage) {
        try {
            JSONObject JSONSdp = new JSONObject(sdpMessage);
            SessionDescription.Type type = SessionDescription.Type.fromCanonicalForm(JSONSdp.getString("type").toLowerCase());
            String description = JSONSdp.getString("sdp");
            mLocalPeer.setRemoteDescription(new CustomSdpObserver(), new SessionDescription(type, description));
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private String getLocalIPAddress() {
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        int ip = wifiManager.getConnectionInfo().getIpAddress();
        return "" + (ip & 0xff) + "." + (ip >> 8 & 0xff) + "." + (ip >> 16 & 0xff) + "." + (ip >> 24 & 0xff);
    }
}
