package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.json.JSONException;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoFrame;
import org.webrtc.VideoSink;
import org.webrtc.VideoTrack;

import java.util.List;
import java.util.Map;

import hcs.offloading.edgeserver.config.RoIExtractorConfig;
import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.network.webrtc.WebRTCManager;
import hcs.offloading.network.webrtc.YuvFrame;

@RequiresApi(api = Build.VERSION_CODES.N)
public class Dispatcher implements VideoSink {
    private static final String TAG = Dispatcher.class.getName();

    private final SurfaceViewRenderer mInputView;

    private int mFrameIndex = 0;

    private final String mSourceIP;
    private MediaStream mMediaStream;
    private VideoTrack mVideoTrack;
    private final PeerConnection mPeerConnection;
    private final WebRTCManager mWebRTCManager;

    private final RoIExtractor mRoIExtractor;

    @RequiresApi(api = Build.VERSION_CODES.P)
    Dispatcher(String sourceIP, WebRTCManager webRTCManager, SurfaceViewRenderer inputView, RoIExtractorConfig config, RoIExtractor.Callback callback, int mixedFrameSize) {
        mRoIExtractor = new RoIExtractor(config, callback, sourceIP);

        mSourceIP = sourceIP;
        mWebRTCManager = webRTCManager;
        mInputView = inputView;

        mPeerConnection = mWebRTCManager.createPeerConnection(sourceIP);
    }

    void onAddStream(MediaStream mediaStream) {
        synchronized (mPeerConnection) {
            mMediaStream = mediaStream;
            mVideoTrack = mMediaStream.videoTracks.get(0);
            mVideoTrack.setEnabled(true);

            mVideoTrack.addSink(mInputView);
            mVideoTrack.addSink(this);
        }
    }

    public void close() {
        mRoIExtractor.close();
        synchronized (mPeerConnection) {
            if (mMediaStream != null) {
                mVideoTrack.removeSink(this);
                mVideoTrack.removeSink(mInputView);
                mMediaStream.removeTrack(mVideoTrack);
                mPeerConnection.removeStream(mMediaStream);
            }
            if (!mPeerConnection.connectionState().equals(PeerConnection.PeerConnectionState.CLOSED)) {
                mPeerConnection.close();
            }
        }
        Log.d(TAG, "closed");
    }

    @Override
    public void onFrame(VideoFrame videoFrame) {
        YuvFrame yuvFrame = new YuvFrame(videoFrame);
        Bitmap bitmap = yuvFrame.getBitmap();

        mRoIExtractor.enqueueFrame(Frame.createSingleFrame(bitmap, mSourceIP, mFrameIndex++));
    }

    void enqueueResults(int frameIndex, List<BoundingBox> results) {
        mRoIExtractor.enqueueResults(frameIndex, results);
    }

    void enqueueResults(Map<Integer, List<BoundingBox>> results) {
        mRoIExtractor.enqueueResults(results);
    }

    void handleSdpAndAnswer(String message) {
        try {
            mWebRTCManager.handleSdpMessage(mPeerConnection, message);
            mWebRTCManager.answer(mPeerConnection, mSourceIP);
        } catch (JSONException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    void handleIceMessage(String message) {
        try {
            mWebRTCManager.handleIceMessage(mPeerConnection, message);
        } catch (JSONException e) {
            Log.e(TAG, e.getMessage());
        }
    }
}
