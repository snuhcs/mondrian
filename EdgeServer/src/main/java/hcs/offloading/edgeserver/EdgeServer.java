package hcs.offloading.edgeserver;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import org.json.JSONException;
import org.json.simple.parser.ParseException;
import org.webrtc.EglBase;
import org.webrtc.MediaStream;
import org.webrtc.SurfaceViewRenderer;

import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import hcs.offloading.edgeserver.config.Config;
import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.network.mqtt.DeviceMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;
import hcs.offloading.network.webrtc.WebRTCCallback;
import hcs.offloading.network.webrtc.WebRTCManager;

@RequiresApi(api = Build.VERSION_CODES.P)
public class EdgeServer implements WebRTCCallback, Dispatcher.Callback, RoIExtractor.Callback, Worker.Callback {
    private static final String TAG = EdgeServer.class.getName();

    private static final String CONFIG_FILEPATH = "/data/local/tmp/edgeserver.json";

    private Config mConfig;

    private SurfaceViewRenderer mInputView;
    private final ViewCallback mViewCallback;

    private final AssetManager mAssetManager;

    private String mTargetEdgeIP;
    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    private final Map<String, Dispatcher> mDispatchers = new ConcurrentHashMap<>();
    private RoIExtractor mRoIExtractor;
    private InferenceEngine mInferenceEngine;
    private PatchReconstructor mPatchReconstructor;

    EdgeServer(Context context, String uri, SurfaceViewRenderer inputView, ViewCallback viewCallback) throws JSONException, ParseException, IOException {
        mConfig = new Config(CONFIG_FILEPATH);
        mAssetManager = context.getAssets();

        mInputView = inputView;
        mViewCallback = viewCallback;

        mMqttManager = new DeviceMqttManager(context, uri, Device.EDGE, scheduleTopicHandler, webrtcTopicHandler);

        EglBase eglBase = EglBase.create();
        mInputView.init(eglBase.getEglBaseContext(), null);
        mWebRTCManager = new WebRTCManager(context, mMqttManager, eglBase, this);
    }

    void close() {
        stopEdgeServer();

        mMqttManager.close();
        Log.d(TAG, "closed");
    }

    private void startEdgeServer() {
        synchronized (this) {
            mPatchReconstructor = new PatchReconstructor(mConfig.patchReconstructorConfig, mViewCallback);
            mInferenceEngine = new InferenceEngine(mConfig.inferenceEngineConfig, this, mAssetManager);
            mRoIExtractor = new RoIExtractor(mConfig.roIExtractorConfig, this);
        }
    }

    private void stopEdgeServer() {
        Set<String> IPs = mDispatchers.keySet();
        for (String ip : IPs) {
            Dispatcher dispatcher = mDispatchers.remove(ip);
            if (dispatcher != null) {
                dispatcher.close();
            }
        }
        synchronized (this) {
            if (mRoIExtractor != null) {
                mRoIExtractor.close();
                mRoIExtractor = null;
            }
            if (mInferenceEngine != null) {
                mInferenceEngine.close();
                mInferenceEngine = null;
            }
            if (mPatchReconstructor != null) {
                mPatchReconstructor.close();
                mPatchReconstructor = null;
            }
        }
    }

    private final PacketHandler scheduleTopicHandler = packet -> {
        String newTargetEdgeIP = packet.message;
        if (newTargetEdgeIP != null && newTargetEdgeIP.equals(mTargetEdgeIP)) {
            return;
        }
        if (mMqttManager.isLocalIP(newTargetEdgeIP)) {
            startEdgeServer();
        } else {
            stopEdgeServer();
        }
        mTargetEdgeIP = newTargetEdgeIP;
    };

    private final PacketHandler webrtcTopicHandler = packet -> {
        if (mMqttManager.isLocalIP(packet.dstIp)) {
            if (packet.header.equals(WebRTCHeader.SDP.name())) {
                mDispatchers.put(packet.srcIp, new Dispatcher(mConfig.dispatcherConfig, this,
                        packet.srcIp, mWebRTCManager, mInputView));
                mDispatchers.get(packet.srcIp).handleSdpAndAnswer(packet.message);
            } else if (packet.header.equals(WebRTCHeader.ICE.name())) {
                mDispatchers.get(packet.srcIp).handleIceMessage(packet.message);
            }
        }
    };

    // WebRTCManager.StreamCallback
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

    // Dispatcher.Callback
    @Override
    public void enqueueFrame(Frame frame) {
        mRoIExtractor.enqueueFrame(frame);
    }

    @Override
    public void removeStream(String sourceIP) {
        mPatchReconstructor.removeStream(sourceIP);
    }

    // RoIExtractor.Callback
    @Override
    public Pair<Bitmap, List<BoundingBox>> getFrameAndResults(String sourceIP, int frameIndex) throws InterruptedException {
        return mPatchReconstructor.getRemoveDrawResult(sourceIP, frameIndex);
    }

    @Override
    public void enqueueInferenceRequest(InferenceRequest inferenceRequest) {
        mInferenceEngine.enqueueRequest(inferenceRequest);
    }

    // Worker.Callback
    @Override
    public void enqueueInferenceResult(InferenceRequest request, List<BoundingBox> results) {
        mPatchReconstructor.enqueueInferenceResult(request, results);
    }
}
