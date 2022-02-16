package hcs.offloading.edgeserver;

import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import org.webrtc.EglBase;
import org.webrtc.MediaStream;
import org.webrtc.SurfaceViewRenderer;

import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.network.mqtt.DeviceMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;
import hcs.offloading.network.webrtc.WebRTCCallback;
import hcs.offloading.network.webrtc.WebRTCManager;

@RequiresApi(api = Build.VERSION_CODES.P)
public class EdgeServer implements WebRTCCallback {
    private static final String TAG = EdgeServer.class.getName();

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

    EdgeServer(Context context, EglBase eglBase, String uri, SurfaceViewRenderer inputView, ImageView[] outputViews, ImageView inferenceOutputView, TextView fpsView) {
        mContext = context;

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
        synchronized (this) {
            mInferenceEngine = new InferenceEngine(mContext.getAssets(), this, mInferenceOutputView, mFpsView);
            mRoIExtractor = new RoIExtractor(mInferenceEngine);
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
                        packet.srcIp, mWebRTCManager, mRoIExtractor, mInferenceEngine, mInputView, mOutputViews[streamID]));
                mDispatchers.get(packet.srcIp).handleSdpAndAnswer(packet.message);
            } else if (packet.header.equals(WebRTCHeader.ICE.name())) {
                mDispatchers.get(packet.srcIp).handleIceMessage(packet.message);
            }
        }
    };

    public void updateResult(Frame frame, List<BoundingBox> boxes) {
        Dispatcher dispatcher = mDispatchers.get(frame.sourceIP);
        if (dispatcher != null) {
            dispatcher.updateResult(frame.frameIndex, boxes);
        }
    }

    public int updateResult(Map<String, Map<Integer, List<BoundingBox>>> multiStreamResults) {
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
        return numProcessedFrames;
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
