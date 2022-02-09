package hcs.offloading.edgeserver;

import android.content.Context;
import android.graphics.Bitmap;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;

import androidx.appcompat.app.AppCompatActivity;

import hcs.offloading.network.Header;
import hcs.offloading.network.Message;
import hcs.offloading.network.NetworkManager;
import hcs.offloading.network.Packet;
import hcs.offloading.network.PacketHandler;
import hcs.offloading.network.Topic;

import org.json.JSONObject;
import org.tensorflow.lite.support.image.TensorImage;
import org.webrtc.DefaultVideoDecoderFactory;
import org.webrtc.DefaultVideoEncoderFactory;
import org.webrtc.EglBase;
import org.webrtc.IceCandidate;
import org.webrtc.MediaConstraints;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.PeerConnectionFactory;
import org.webrtc.SessionDescription;
import org.webrtc.VideoTrack;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import hcs.offloading.edgeserver.databinding.ActivityMainBinding;
import hcs.offloading.edgeserver.models.BoundingBox;
import hcs.offloading.edgeserver.models.DetectionModel;
import hcs.offloading.edgeserver.models.MobileNetSSD;
import hcs.offloading.edgeserver.models.Yolov4;

public class MainActivity extends AppCompatActivity implements Runnable {
    private static final String TAG = MainActivity.class.getName();
    private final Lock mTargetModelsLock = new ReentrantLock();
    private final Map<String, AtomicBoolean> mIsInputPrepared = new HashMap<>();
    private final Map<String, TensorImage> mInputImages = new HashMap<>();
    private final Map<String, DetectionModel> mTargetModels = new HashMap<>();
    private final AtomicBoolean mbRunInference = new AtomicBoolean(false);
    // WebRTC
    EglBase mRootEglBase;
    PeerConnectionFactory mPeerConnectionFactory;
    HashMap<String, PeerConnection> mPeerConnectionMap = new HashMap<>();
    // Optimizations
    Bitmap prevFrameBitmap = null;
    // UI Elements
    private ActivityMainBinding mBinding;
    private EditText mIpInput;
    private EditText mPortInput;
    private Switch mConnectButton;
    private ImageView mImageView;
    // Model
    private DetectionModel mObjectDetection;
    private DetectionModel mFaceDetection;
    private int mSensorIdx = 0;
    private Thread mInferenceThread;
    // NetworkManager
    private String mLocalIP;
    private NetworkManager mNetworkManager;
    private final PacketHandler scheduleTopicHandler = packet -> {
        if (packet.dstIp.equals(Packet.ALL) && packet.header.equals(Header.SCHEDULE)) {
            sendRegisterMessage();
            if (mNetworkManager.isLocalIP(packet.message)) {
                Log.d(TAG, "Scheduled to this edge");
            } else {
                Log.d(TAG, "Scheduled to different edge");
                destroyAllConnections();
            }
        }
    };
    private final PacketHandler webrtcTopicHandler = packet -> {
        if (packet.dstIp.equals(mLocalIP)) {
            if (packet.header.equals(Header.ICE)) {
                Log.d(TAG, "Ice candidate received from: " + packet.srcIp);
                handleIceCandidate(packet.srcIp, packet.message);
            } else if (packet.header.equals(Header.SDP)) {
                Log.d(TAG, "Offer received from: " + packet.srcIp);
                createPeerConnection(packet.srcIp);
                handleOffer(packet.srcIp, packet.message);
                answer(packet.srcIp);
                mIsInputPrepared.put(packet.srcIp, new AtomicBoolean(false));
            }
        }
    };
    private final PacketHandler taskTopicHandler = packet -> {
        if (mNetworkManager.isLocalIP(packet.dstIp)) {
            if (packet.header.equals(Header.TASK)) {
                mTargetModelsLock.lock();
                if (Message.TASK_NO_INFERENCE.equals(packet.message)) {
                    mTargetModels.put(packet.srcIp, null);
                } else if (Message.TASK_OBJECT_DETECTION.equals(packet.message)) {
                    mTargetModels.put(packet.srcIp, mObjectDetection);
                } else if (Message.TASK_FACE_DETECTION.equals(packet.message)) {
                    mTargetModels.put(packet.srcIp, mFaceDetection);
                }
                mTargetModelsLock.unlock();
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mBinding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        mIpInput = findViewById(R.id.ip_input);
        mPortInput = findViewById(R.id.port_input);
        mConnectButton = findViewById(R.id.connect_button);
        mImageView = findViewById(R.id.image_view);

        // WebRTC
        initializeWebRTC(getApplicationContext());

        // NetworkManager
        mNetworkManager = new NetworkManager(new HashMap<String, PacketHandler>() {{
            put(Topic.SCHEDULE, scheduleTopicHandler);
            put(Topic.WEBRTC, webrtcTopicHandler);
            put(Topic.TASK, taskTopicHandler);
        }});

        mConnectButton.setOnCheckedChangeListener((compoundButton, b) -> {
            if (b) {
                startConnection();
            } else {
                stopConnection();
            }
        });

        // Model
        mObjectDetection = new MobileNetSSD(getApplicationContext().getAssets());
        mFaceDetection = new Yolov4(getApplicationContext().getAssets());

    }

    private void initializeWebRTC(Context context) {
        Log.d(TAG, "initializeWebRTC()");
        PeerConnectionFactory.initialize(PeerConnectionFactory.InitializationOptions.builder(context).createInitializationOptions());
        mPeerConnectionFactory = PeerConnectionFactory.builder().createPeerConnectionFactory();
        mRootEglBase = EglBase.create();
        //Create a new PeerConnectionFactory instance.
        PeerConnectionFactory.Options options = new PeerConnectionFactory.Options();
        DefaultVideoEncoderFactory defaultVideoEncoderFactory = new DefaultVideoEncoderFactory(
                mRootEglBase.getEglBaseContext(), true, true
        );
        DefaultVideoDecoderFactory defaultVideoDecoderFactory = new DefaultVideoDecoderFactory(mRootEglBase.getEglBaseContext());
        mPeerConnectionFactory = PeerConnectionFactory.builder()
                .setOptions(options)
                .setVideoEncoderFactory(defaultVideoEncoderFactory)
                .setVideoDecoderFactory(defaultVideoDecoderFactory)
                .createPeerConnectionFactory();
    }

    public void startConnection() {
        // Start NetworkManager
        String brokerIP = mIpInput.getText().toString().trim();
        int brokerPort = Integer.parseInt(mPortInput.getText().toString().trim());
        String uri = "tcp://" + brokerIP + ":" + brokerPort;
        mLocalIP = getLocalIPAddress();
        mNetworkManager.start(getApplicationContext(), mLocalIP, uri);
        mNetworkManager.connect(() -> {
            mNetworkManager.subscribe(Topic.SCHEDULE, null);
            mNetworkManager.subscribe(Topic.WEBRTC, null);
            mNetworkManager.subscribe(Topic.TASK, null);
        });

        // Start Inference
        mbRunInference.set(true);
        if (mInferenceThread == null) {
            mInferenceThread = new Thread(this);
            mInferenceThread.start();
        }

        // Register Edge
        sendRegisterMessage();
    }

    public void stopConnection() {
        // Destroy WebRTC connections
        destroyAllConnections();

        // Clear Memory
        mTargetModelsLock.lock();
        mTargetModels.clear();
        mTargetModelsLock.unlock();
        mInputImages.clear();
        mIsInputPrepared.clear();

        // Unregister Edge
        sendUnregisterMessage();

        // Stop Inference
        mbRunInference.set(false);
        if (mInferenceThread != null) {
            try {
                mInferenceThread.join();
            } catch (InterruptedException e) {
                Log.e(TAG, "mInferenceThread.join() failed");
            }
        }
        mInferenceThread = null;

        // Stop NetworkManager
        mNetworkManager.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopConnection();
    }

    private void sendRegisterMessage() {
        mNetworkManager.publish(Topic.REGISTER,
                Packet.ALL, Header.REGISTER, Message.DEVICE_EDGE, null);
    }

    private void sendUnregisterMessage() {
        mNetworkManager.publish(Topic.UNREGISTER,
                Packet.ALL, Header.UNREGISTER, Message.DEVICE_EDGE, null);
    }

    private void handleIceCandidate(String remotePeerIp, String iceMessage) {
        try {
            JSONObject JSONIceCandidate = new JSONObject(iceMessage);
            String id = JSONIceCandidate.getString("id");
            int label = JSONIceCandidate.getInt("label");
            String candidate = JSONIceCandidate.getString("candidate");
            IceCandidate iceCandidate = new IceCandidate(id, label, candidate);
            mPeerConnectionMap.get(remotePeerIp).addIceCandidate(iceCandidate);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void createPeerConnection(String remotePeerIp) {
        Log.d(TAG, "createPeerConnection(): " + remotePeerIp);
        List<PeerConnection.IceServer> iceServers = new ArrayList<>();
        PeerConnection.RTCConfiguration rtcConfig = new PeerConnection.RTCConfiguration(iceServers);
        PeerConnection pc = mPeerConnectionFactory.createPeerConnection(rtcConfig, new CustomPeerConnectionObserver() {
            @Override
            public void onIceCandidate(IceCandidate iceCandidate) {
                super.onIceCandidate(iceCandidate);
                try {
                    JSONObject JSONIceCandidate = new JSONObject();
                    JSONIceCandidate.put("type", "candidate");
                    JSONIceCandidate.put("label", iceCandidate.sdpMLineIndex);
                    JSONIceCandidate.put("id", iceCandidate.sdpMid);
                    JSONIceCandidate.put("candidate", iceCandidate.sdp);
                    String message = JSONIceCandidate.toString();
                    mNetworkManager.publish(Topic.WEBRTC, remotePeerIp, Header.ICE, message, null);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onAddStream(MediaStream mediaStream) {
                super.onAddStream(mediaStream);
                Log.d(TAG, "Remote stream added");
                if (mediaStream.videoTracks != null && mediaStream.videoTracks.size() == 1) {
                    VideoTrack videoTrack = mediaStream.videoTracks.get(0);
                    videoTrack.setEnabled(true);
                    videoTrack.addSink(frame -> {
                        YuvFrame yuvFrame = new YuvFrame(frame);
                        Bitmap bitmap = yuvFrame.getBitmap();
                        Log.d(TAG, "Rendering frame... : " + bitmap.getWidth() + "," + bitmap.getHeight());
                        mImageView.post(() -> mImageView.setImageBitmap(bitmap));

                        /* motion detection filtering */
                        boolean inferenceRequired = false;
                        if (prevFrameBitmap == null) {
                            prevFrameBitmap = bitmap;
                            inferenceRequired = true;
                        } else {
                            double threshold = 2;
                            if (getBitmapDiff(prevFrameBitmap, bitmap) > threshold) {
                                inferenceRequired = true;
                                prevFrameBitmap = bitmap;
                            }
                        }
                        // TODO: sensor app must display last received bounding box until new bounding box arrives

                        if (inferenceRequired) {
                            Log.d(TAG, "inference required");
                            if (!mIsInputPrepared.get(remotePeerIp).get()) {
                                mInputImages.put(remotePeerIp, TensorImage.fromBitmap(bitmap));
                                mIsInputPrepared.get(remotePeerIp).set(true);
                            }
                        } else {
                            Log.d(TAG, "inference not required");
                        }
                    });
                }
            }
        });
        mPeerConnectionMap.put(remotePeerIp, pc);
    }

    private double getBitmapDiff(Bitmap bmp1, Bitmap bmp2) {
        /* scale down for (1) less computation (2) robustness to noise */
        int scale = 8;
        Bitmap scaled1 = Bitmap.createScaledBitmap(bmp1, scale, scale, false);
        Bitmap scaled2 = Bitmap.createScaledBitmap(bmp2, scale, scale, false);
        int width = scaled1.getWidth();
        int height = scaled2.getHeight();

        /* pixel-wise rgb diff */
        double diff = 0.0;
        double diffMax = 3.0 * 255 * width * height;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                /* rgb : 0xRRGGBB */
                int rgb1 = scaled1.getPixel(x, y);
                int rgb2 = scaled2.getPixel(x, y);
                int pixelDiff = 0;
                for (int i = 0; i < 3; i++) { /* R, G, B */
                    int shift = 8 * (2 - i);
                    pixelDiff += Math.abs(((rgb1 >> shift) & 0xff) - ((rgb2 >> shift) & 0xff));
                }
                diff += pixelDiff;
            }
        }
        return 100.0 * diff / diffMax;
    }

    @Override
    public void run() {
        Log.d(TAG, "Inference thread start");
        while (mbRunInference.get()) {
            List<String> sensorIPs = new ArrayList<>(mIsInputPrepared.keySet());
            String sensorIP;
            if (sensorIPs.size() == 0) {
                sensorIP = null;
            } else {
                sensorIP = sensorIPs.get(mSensorIdx % sensorIPs.size());
            }
            mTargetModelsLock.lock();
            DetectionModel model = mTargetModels.get(sensorIP);
            if (sensorIP != null && model != null && mIsInputPrepared.get(sensorIP).get()) {
                Log.d(TAG, "Inference");
                List<BoundingBox> boundingBoxes = model.inference(mInputImages.get(sensorIP));
                Log.d(TAG, "Inference Finished : " + boundingBoxes.size());
                mTargetModelsLock.unlock();
                mIsInputPrepared.get(sensorIP).set(false);
                mSensorIdx++;

                StringBuilder message = new StringBuilder();
                for (BoundingBox boundingBox : boundingBoxes) {
                    message.append(boundingBox.toString());
                    message.append(',');
                }
                if (message.length() > 0) {
                    mNetworkManager.publish(Topic.RESULT, sensorIP, Header.RESULT,
                            message.substring(0, message.length() - 1), null);
                }

            } else {
                mTargetModelsLock.unlock();
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    Log.d(TAG, "mInferenceThread interrupted");
                }
            }
        }
    }


    private void handleOffer(String remotePeerIp, String sdpMessage) {
        try {
            JSONObject JSONSdp = new JSONObject(sdpMessage);
            SessionDescription.Type type = SessionDescription.Type.OFFER;
            String description = JSONSdp.getString("sdp");
            Objects.requireNonNull(mPeerConnectionMap.get(remotePeerIp)).setRemoteDescription(new CustomSdpObserver(), new SessionDescription(type, description));
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private void answer(String remotePeerIp) {
        Log.d(TAG, "answer()");
        MediaConstraints sdpConstraints = new MediaConstraints();
        Objects.requireNonNull(mPeerConnectionMap.get(remotePeerIp)).createAnswer(new CustomSdpObserver() {
            @Override
            public void onCreateSuccess(SessionDescription sessionDescription) {
                super.onCreateSuccess(sessionDescription);
                Objects.requireNonNull(mPeerConnectionMap.get(remotePeerIp)).setLocalDescription(new CustomSdpObserver(), sessionDescription);
                try {
                    JSONObject JSONSdp = new JSONObject();
                    JSONSdp.put("type", sessionDescription.type.canonicalForm());
                    JSONSdp.put("sdp", sessionDescription.description);
                    mNetworkManager.publish(Topic.WEBRTC, remotePeerIp, Header.SDP, JSONSdp.toString(), null);
                } catch (Exception e) {
                    Log.e(TAG, e.getMessage());
                }
            }

            @Override
            public void onCreateFailure(String s) {
                super.onCreateFailure(s);
                Log.d(TAG, "answer onCreateFailure() : " + s);
            }
        }, sdpConstraints);
    }

    private void destroyAllConnections() {
        if (mPeerConnectionMap != null) {
            for (PeerConnection pc : mPeerConnectionMap.values()) {
                if (pc != null) {
                    pc.close();
                }
            }
            mPeerConnectionMap.clear();
        }
    }

    private String getLocalIPAddress() {
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        int ip = wifiManager.getConnectionInfo().getIpAddress();
        return "" + (ip & 0xff) + "." + (ip >> 8 & 0xff) + "." + (ip >> 16 & 0xff) + "." + (ip >> 24 & 0xff);
    }
}
