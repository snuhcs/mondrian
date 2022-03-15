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

import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.network.webrtc.WebRTCManager;
import hcs.offloading.network.webrtc.YuvFrame;

@RequiresApi(api = Build.VERSION_CODES.N)
public class Dispatcher implements VideoSink {
    private static final String TAG = Dispatcher.class.getName();

    public interface Callback {
        void enqueueFrame(Frame frame);

        void enqueueInferenceRequest(InferenceRequest inferenceRequest);

        void removeStream(String sourceIP);
    }

    private final Callback mCallback;

    private final SurfaceViewRenderer mInputView;

    private int mFrameIndex = 0;

    private final String mSourceIP;
    private MediaStream mMediaStream;
    private VideoTrack mVideoTrack;
    private final PeerConnection mPeerConnection;
    private final WebRTCManager mWebRTCManager;

    Dispatcher(Callback callback, String sourceIP, WebRTCManager webRTCManager, SurfaceViewRenderer inputView) {
        mCallback = callback;

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
        mCallback.removeStream(mSourceIP);
        Log.d(TAG, "closed");
    }

    @Override
    public void onFrame(VideoFrame videoFrame) {
        YuvFrame yuvFrame = new YuvFrame(videoFrame);
        Bitmap bitmap = yuvFrame.getBitmap();

        Frame frame = Frame.createSingleFrame(bitmap, mSourceIP, mFrameIndex);
        if (mFrameIndex == 0) {
            mCallback.enqueueInferenceRequest(InferenceRequest.createFullFrameRequest(frame));
        } else {
            mCallback.enqueueFrame(frame);
        }
        mFrameIndex++;
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
