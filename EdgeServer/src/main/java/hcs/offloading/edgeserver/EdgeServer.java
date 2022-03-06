package hcs.offloading.edgeserver;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;
import android.widget.ImageView;
import android.widget.TextView;

import org.json.JSONException;
import org.json.simple.parser.ParseException;
import org.webrtc.EglBase;
import org.webrtc.MediaStream;
import org.webrtc.SurfaceViewRenderer;

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
public class EdgeServer implements WebRTCCallback, RoIExtractor.Callback, Worker.Callback {
    private static final String TAG = EdgeServer.class.getName();

    private Config mConfig;

    private SurfaceViewRenderer mInputView;
    private ImageView[] mOutputViews;
    private final ImageView mInferenceOutputView;
    private final TextView mFpsView;

    private final Context mContext;

    private String mTargetEdgeIP;
    private WebRTCManager mWebRTCManager;
    private DeviceMqttManager mMqttManager;

    private final Map<String, Dispatcher> mDispatchers = new ConcurrentHashMap<>();
    private RoIExtractor mRoIExtractor;
    private InferenceEngine mInferenceEngine;
    private PatchReconstructor mPatchReconstructor;

    private int mNumProcessedFrames;
    private long mApplicationStartTime;

    EdgeServer(Config config, Context context, EglBase eglBase, String uri, SurfaceViewRenderer inputView, ImageView[] outputViews, ImageView inferenceOutputView, TextView fpsView) throws JSONException, ParseException {
        mContext = context;
        mConfig = config;

        mInputView = inputView;
        mOutputViews = outputViews;
        mInferenceOutputView = inferenceOutputView;
        mFpsView = fpsView;

        mMqttManager = new DeviceMqttManager(mContext, uri, Device.EDGE, scheduleTopicHandler, webrtcTopicHandler);
        mWebRTCManager = new WebRTCManager(mContext, mMqttManager, eglBase, this);
    }

    void close() {
        stopEdgeServer();

        mMqttManager.close();
        Log.d(TAG, "closed");
    }

    private void startEdgeServer() {
        mNumProcessedFrames = 0;
        mApplicationStartTime = System.nanoTime();
        synchronized (this) {
            mPatchReconstructor = new PatchReconstructor(mConfig.patchReconstructorConfig, this);
            mInferenceEngine = new InferenceEngine(mConfig.inferenceEngineConfig, this, mContext.getAssets());
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
                int streamID = mDispatchers.size();
                if (streamID >= mOutputViews.length) {
                    return;
                }
                mDispatchers.put(packet.srcIp, new Dispatcher(
                        mConfig.dispatcherConfig, packet.srcIp, mWebRTCManager, mRoIExtractor, mInferenceEngine, mInputView, mOutputViews[streamID]));
                mDispatchers.get(packet.srcIp).handleSdpAndAnswer(packet.message);
            } else if (packet.header.equals(WebRTCHeader.ICE.name())) {
                mDispatchers.get(packet.srcIp).handleIceMessage(packet.message);
            }
        }
    };

    public void updateResult(Map<String, Map<Integer, List<BoundingBox>>> multiStreamResults) {
        int numProcessedFrames = 0;
        Set<String> IPs = multiStreamResults.keySet();
        for (String ip : IPs) {
            Map<Integer, List<BoundingBox>> results = multiStreamResults.get(ip);
            Dispatcher dispatcher = mDispatchers.get(ip);
            if (dispatcher != null) {
                dispatcher.updateResult(results);
            }
            numProcessedFrames += results.size();
        }
        updateFPS(numProcessedFrames);
    }

    @SuppressLint("DefaultLocale")
    private void updateFPS(int numProcessedFrames) {
        mNumProcessedFrames += numProcessedFrames;
        float fps = mNumProcessedFrames / ((System.nanoTime() - mApplicationStartTime) / 1000000000f);
        mFpsView.post(() -> mFpsView.setText(String.format("%.3f", fps)));
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

    @Override
    public void enqueueInferenceRequest(InferenceRequest inferenceRequest) {
        mInferenceEngine.enqueueRequest(inferenceRequest);
    }

    @Override
    public void enqueueInferenceResult(Pair<InferenceRequest, List<BoundingBox>> inferenceResult) {
        mPatchReconstructor.enqueueInferenceResult(inferenceResult);
    }

    @Override
    public void updateResult(Frame frame, List<BoundingBox> boxes) {
        Dispatcher dispatcher = mDispatchers.get(frame.sourceIP);
        if (dispatcher != null) {
            dispatcher.updateResult(frame.frameIndex, boxes);
        }
        updateFPS(1);
    }

    @Override
    public void updateInferenceOutputView(Bitmap result) {
        mInferenceOutputView.post(() -> mInferenceOutputView.setImageBitmap(result));
    }
}
