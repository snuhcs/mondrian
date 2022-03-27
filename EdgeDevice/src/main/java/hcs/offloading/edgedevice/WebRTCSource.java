package hcs.offloading.edgedevice;

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

import hcs.offloading.strm.SpatioTemporalRoIMixer;
import hcs.offloading.network.webrtc.WebRTCManager;
import hcs.offloading.network.webrtc.YuvFrame;

@RequiresApi(api = Build.VERSION_CODES.N)
public class WebRTCSource implements VideoSink {
    private static final String TAG = WebRTCSource.class.getName();

    private final SurfaceViewRenderer mInputView;

    private int mFrameIndex = 0;
    private final int key;
    private final SpatioTemporalRoIMixer strm;

    private final String mSourceIP;
    private MediaStream mMediaStream;
    private VideoTrack mVideoTrack;
    private final PeerConnection mPeerConnection;
    private final WebRTCManager mWebRTCManager;

    @RequiresApi(api = Build.VERSION_CODES.P)
    WebRTCSource(String sourceIP, SpatioTemporalRoIMixer strm, WebRTCManager webRTCManager, SurfaceViewRenderer inputView) {
        mSourceIP = sourceIP;
        key = mSourceIP.hashCode();
        this.strm = strm;
        this.strm.addSource(key);
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
        try {
            strm.enqueueImage(key, mFrameIndex++, bitmap);
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
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
